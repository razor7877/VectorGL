#version 330 core

in vec2 TexCoord;

out vec4 FragColor;

uniform sampler2D ssaoInput;

void main()
{
    vec2 texelSize = 1.0 / vec2(textureSize(ssaoInput, 0));
    float result = 0.0;

    for (int x = -2; x < 2; x++)
    {
        for (int y = -2; y < 2; y++)
        {
            vec2 offset = vec2(float(x), float(y)) * texelSize;
            result += texture(ssaoInput, TexCoord + offset).r;
        }
    }

    FragColor = vec4(result / (4.0 * 4.0));
}