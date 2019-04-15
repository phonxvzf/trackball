#version 130

in vec3 out_normal;
in vec2 out_tex_coords;

out vec4 frag_color;

uniform sampler2D u_tex_sampler;
uniform int u_use_texture;

void main() {
  // The following code is equivalent to:
  //frag_color = (1 - u_use_texture) * vec4(0.5f * out_normal + vec3(0.5f, 0.5f, 0.5f), 1.0f)
  //  + u_use_texture * texture(u_tex_sampler, out_tex_coords);
  frag_color = mix(
      vec4(0.5f * out_normal + vec3(0.5f, 0.5f, 0.5f), 1.0f),
      texture(u_tex_sampler, out_tex_coords),
      u_use_texture
      );
}
