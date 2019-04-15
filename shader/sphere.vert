#version 130

in vec3 in_position;
in vec3 in_normal;
in vec2 in_tex_coords;
uniform mat4 u_model;
uniform mat4 u_view;
uniform mat4 u_proj;

out vec3 out_normal;
out vec2 out_tex_coords;

void main() {
  out_normal = in_normal;
  out_tex_coords = in_tex_coords;
  gl_Position = u_proj * u_view * u_model * vec4(in_position, 1.0f);
}
