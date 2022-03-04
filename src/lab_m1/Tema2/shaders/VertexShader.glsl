#version 330

// Input
layout(location = 0) in vec3 v_position;
layout(location = 1) in vec3 v_normal;

// Uniform properties
uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;

// Uniforms for properties
uniform vec3 object_color;

uniform int noise;
uniform float time;

// Output value to fragment shader
out vec3 color;

// Generic 1,2,3 Noise
float rand(float n) {
    return fract(sin(n) * 43758.5453123);
}

float noise_f1(float p){
    float fl = floor(p);
    float fc = fract(p);
	return sin(mix(rand(fl), rand(fl + 1.0), fc));
}

void main()
{
    color = object_color;
	vec3 aux_position = v_position;
	float total_noise = 0;
	float aux_value = 1.0;
	int iterations = 100;

    if (noise == 1) {
		color = mix(vec3(1, 0, 1), vec3(0, 0.5, 1), vec3(1, 1, 0));

	    for( int i = 0; i < iterations; i++ )
	    {
		    total_noise += noise_f1(aux_value * length(v_position.yxz) * (abs(sin(time)) + 1) * (abs(cos(time)) + 1)) / aux_value;
		    aux_value *= 2;
	    }
	
	    aux_position = v_position * total_noise;
	}

    gl_Position = Projection * View * Model * vec4(aux_position, 1.0);
}