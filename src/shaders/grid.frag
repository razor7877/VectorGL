#version 330 core

in vec3 nearPoint;
in vec3 farPoint;

out vec4 FragColor;

layout (std140) uniform Matrices
{
	mat4 view;
	mat4 projection;
};

float near, far;

vec4 grid(vec3 fragPos3D, float scale, bool drawAxis)
{
	vec2 coord = fragPos3D.xz * scale; // Use the scale variable to set the distance between the lines
	vec2 derivative = fwidth(coord);
	vec2 grid = abs(fract(coord - 0.5) - 0.5) / derivative;
	float line = min(grid.x, grid.y);
	float minimumz = min(derivative.y, 1);
	float minimumx = min(derivative.x, 1);

	vec4 color = vec4(0.2, 0.2, 0.2, 1.0 - min(line, 1.0));

	// Z axis
	if (drawAxis && fragPos3D.x > -0.5 * minimumx && fragPos3D.x < 0.5 * minimumx)
		color.z = 1.0;

	// X axis
	if (drawAxis && fragPos3D.z > -0.5 * minimumx && fragPos3D.z < 0.5 * minimumx)
		color.x = 1.0;

	return color;
}

float computeDepth(vec3 pos)
{
	vec4 clip_space_pos = projection * view * vec4(pos, 1.0);
	return clip_space_pos.z / clip_space_pos.w;
}

// Calculates the near and far values from the projection matrix
void computeProj()
{
	float a = projection[2][2];
	float b = projection[2][3];

	near = (2.0 * b) / (2.0 * a - 2.0);
	far = ((a - 1.0) * near) / (a + 1.0);
}

float computeLinearDepth(vec3 pos)
{
	computeProj();

	vec4 clip_space_pos = projection * view * vec4(pos, 1.0);
	float clip_space_depth = (clip_space_pos.z / clip_space_pos.w) * 2.0 - 1.0; // Put back between -1;1
	float linearDepth = (2.0 * near * far) / (far + near - clip_space_depth * (far - near));

	return linearDepth / far; // Normalize
}

void main()
{	
	float t = -nearPoint.y / (farPoint.y - nearPoint.y);
	vec3 fragPos3D = nearPoint + t * (farPoint - nearPoint);

	gl_FragDepth = computeDepth(fragPos3D);

	float linearDepth = computeLinearDepth(fragPos3D);
	float fading = max(0, (0.6 - linearDepth));

	// Multiple grid resolutions (a smaller and a larger grid)
	FragColor = (grid(fragPos3D, 1, true) + grid(fragPos3D, 0.1, true)) * float(t > 0);
	FragColor.a *= fading;
}