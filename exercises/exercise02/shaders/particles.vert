#version 330 core

layout (location = 0) in vec2 ParticlePosition;
layout (location = 1) in float ParticleSize;
layout (location = 2) in float Birth;
layout (location = 3) in float Duration;
layout (location = 4) in vec4 ParticleColor;
layout (location = 5) in vec2 ParticleVelocity;


// (todo) 02.X: Add more vertex attributes

// (todo) 02.5: Add Color output variable here
out vec4 Color;

// (todo) 02.X: Add uniforms
uniform float CurrentTime;
uniform vec2 Gravity;

void main()
{
    float age = CurrentTime - Birth;
    vec2 newPos = ParticlePosition + ParticleVelocity * age + 0.5 * Gravity * (age * age);
	gl_Position = vec4(newPos, 0.0, 1.0) * 2;
	Color = ParticleColor;

	if((Birth + Duration) < CurrentTime) {
	    gl_PointSize = 0;
	} else {
	    gl_PointSize = ParticleSize;
	}
}
