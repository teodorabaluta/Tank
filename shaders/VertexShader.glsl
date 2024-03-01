#version 330

// Input
// TODO(student): Get vertex attributes from each location
layout(location = 0) in vec3 v_position;
layout(location = 1) in vec3 v_color;
layout(location = 2) in vec2 v_coordinate;
layout(location = 3) in vec3 v_normal;

// Uniform properties
uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;
uniform float Time;

// Output
// TODO(student): Output values to fragment shader
out vec3 frag_color;
out vec3 frag_normal;
out vec2 frag_coordinate;
out vec3 frag_position;


void main()
{   
    // TODO(student): Send output to fragment shader
    frag_color = v_color;
    frag_normal = v_normal;
    frag_coordinate = v_coordinate;
    frag_position = v_position;


    gl_Position = Projection * View * Model * vec4(frag_position, 1.0f);
    // TODO(student): Compute gl_Position

}
