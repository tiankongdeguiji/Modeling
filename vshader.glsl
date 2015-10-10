#version 330 core
#ifdef GL_ES
// Set default precision to medium
precision mediump int;
precision mediump float;
#endif

layout (location = 0) in vec3 a_position;

uniform mat4 mvp_matrix;
uniform mat4 mv_matrix;

out vec3 v_position;

void main()
{
    gl_Position = mvp_matrix * vec4(a_position,1.0);

    v_position = vec3(mv_matrix * vec4(a_position, 1.0));
}
