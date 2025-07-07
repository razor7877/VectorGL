#include <glm/glm/ext/matrix_transform.hpp>
#include <glm/glm/ext/matrix_clip_space.hpp>

#include "render/ShadowPass.hpp"
#include "Renderer.hpp"
#include "materials/PBRMaterial.hpp"

ShadowPass::ShadowPass() : RenderPass()
{
}

ShadowPass::~ShadowPass()
{
    this->renderTarget.release();
}


void ShadowPass::execute(Renderer& renderer, const Scene& scene, float deltaTime)
{
    float near = CameraComponent::NEAR;
    float far = CameraComponent::FAR;
    float cascadeLevels[3] = {
        far * SHADOW_CASCADE_DISTANCES[0],
        far * SHADOW_CASCADE_DISTANCES[1],
        far * SHADOW_CASCADE_DISTANCES[2]
    };

    std::vector<glm::mat4> lightSpaceMatrices = {
        this->getLightSpaceMatrix(renderer, scene, near, cascadeLevels[0]),
        this->getLightSpaceMatrix(renderer, scene, cascadeLevels[0], cascadeLevels[1]),
        this->getLightSpaceMatrix(renderer, scene, cascadeLevels[1], cascadeLevels[2]),
        this->getLightSpaceMatrix(renderer, scene, cascadeLevels[2], far)
    };

    for (int i = 0; i < 4; i++)
        PBRMaterial::lightSpaceMatrices[i] = lightSpaceMatrices[i];

    PBRMaterial::cascadePlaneDistances[0] = cascadeLevels[0];
    PBRMaterial::cascadePlaneDistances[1] = cascadeLevels[1];
    PBRMaterial::cascadePlaneDistances[2] = cascadeLevels[2];

    PBRMaterial::farPlane = far;

    Shader* depthShader = renderer.shaderManager.getShader(ShaderType::DEPTH_CASCADED);

    depthShader->use()
        ->setMat4("lightSpaceMatrices[0]", lightSpaceMatrices[0])
        ->setMat4("lightSpaceMatrices[1]", lightSpaceMatrices[1])
        ->setMat4("lightSpaceMatrices[2]", lightSpaceMatrices[2])
        ->setMat4("lightSpaceMatrices[3]", lightSpaceMatrices[3]);

    this->renderTarget->bind();
    this->renderTarget->clear();

    glEnable(GL_DEPTH_TEST);

    const std::vector<MeshComponent*>* meshes = this->renderDynamicMeshes
                                                    ? &scene.sortedSceneData.dynamicMeshes
                                                    : &scene.sortedSceneData.staticMeshes;

    for (MeshComponent* mesh : *meshes)
        mesh->drawGeometry(depthShader);

    this->renderTarget->unbind();
}

glm::mat4 ShadowPass::getLightSpaceMatrix(Renderer& renderer, const Scene& scene, const float nearPlane, const float farPlane) const
{
    glm::vec2 renderSize = renderer.getRenderSize();

    glm::mat4 cameraProjection = glm::perspective(
        glm::radians(scene.currentCamera->getZoom()),
        renderSize.x / renderSize.y,
        nearPlane,
        farPlane
    );
    glm::mat4 cameraView = scene.currentCamera->getViewMatrix();

    // Get the corners of the camera frustum
    std::vector<glm::vec4> frustumCorners = Frustum::getFrustumCornersWorldSpace(cameraProjection, cameraView);

    auto frustumCenter = glm::vec3(0.0f);

    for (const auto& v : frustumCorners)
        frustumCenter += glm::vec3(v);

    // Average the positions to get the frustum center
    frustumCenter /= frustumCorners.size();

    glm::vec3 lightDir = glm::normalize(scene.directionalLight->parent->getTransform()->getPosition());
    const glm::mat4 lightView = glm::lookAt(
        frustumCenter + lightDir,
        frustumCenter,
        glm::vec3(0.0f, 1.0f, 0.0f)
    );

    float minX = std::numeric_limits<float>::max();
    float maxX = std::numeric_limits<float>::lowest();
    float minY = std::numeric_limits<float>::max();
    float maxY = std::numeric_limits<float>::lowest();
    float minZ = std::numeric_limits<float>::max();
    float maxZ = std::numeric_limits<float>::lowest();

    for (const auto& v : frustumCorners)
    {
        const auto trf = lightView * v;
        minX = std::min(minX, trf.x);
        maxX = std::max(maxX, trf.x);
        minY = std::min(minY, trf.y);
        maxY = std::max(maxY, trf.y);
        minZ = std::min(minZ, trf.z);
        maxZ = std::max(maxZ, trf.z);
    }

    constexpr float zMult = 10.0f;

    if (minZ < 0)
        minZ *= zMult;
    else
        minZ /= zMult;

    if (maxZ < 0)
        maxZ /= zMult;
    else
        maxZ *= zMult;

    const glm::mat4 lightProjection = glm::ortho(minX, maxX, minY, maxY, minZ, maxZ);

    return lightProjection * lightView;
}
