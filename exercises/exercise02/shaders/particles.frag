#version 330 core

out vec4 FragColor;

// (todo) 02.5: Add Color input variable here
in vec4 Color;

float intensity;

void main()
{
	// (todo) 02.3: Compute alpha using the built-in variable gl_PointCoord

    intensity = 1 - length(gl_PointCoord * 2 - 1);
    FragColor = vec4(Color.rgb, Color.a * intensity);
}
