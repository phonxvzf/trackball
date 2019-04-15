#include <cstdint>
#include <iostream>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <algorithm>

#include "main.hpp"
#include "shader.hpp"
#include "glad.h"
#include "sphere.hpp"

inline glm::mat4 calc_rot_mat(const glm::vec3& position_old, const glm::vec3& position_new) {
  glm::vec3 rot_axis = glm::normalize(glm::cross(position_old, position_new));
  float cosine = glm::dot(position_old, position_new) 
    / glm::length(position_old) 
    / glm::length(position_new);
  float cosine_half = glm::sqrt((cosine + 1) / 2);
  if (cosine < 0) cosine_half = -cosine_half;
  glm::quat q(cosine_half, glm::sqrt(1 - cosine_half * cosine_half) * rot_axis);
  return glm::mat4_cast(q);
}

void main_loop(SDL_Window *window) {
  GLuint texture;
  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

  SDL_Surface *image = IMG_Load("texture.png");
  if (!image) {
    std::cerr << "Could not load texture.png: " << IMG_GetError() << std::endl;
    std::exit(1);
  }
  glTexImage2D(
      GL_TEXTURE_2D,
      0,
      GL_RGBA,
      image->w,
      image->h,
      0,
      GL_RGB,
      GL_UNSIGNED_BYTE,
      image->pixels
      );
  SDL_FreeSurface(image);

  shader sphere_shader("shader/sphere.vert", "shader/sphere.frag");
  sphere s(1.0f, 36, 72);

  const std::vector<float>& vertices = s.vertices();
  const std::vector<float>& normals = s.normals();
  const std::vector<float>& tex_coords = s.tex_coords();
  const std::vector<uint32_t>& indices = s.indices();

  std::vector<float> sphere_buffer;
  for (size_t i = 0; i < vertices.size() / 3; ++i) {
    // Vertices
    sphere_buffer.push_back(vertices[i * 3]);
    sphere_buffer.push_back(vertices[i * 3 + 1]);
    sphere_buffer.push_back(vertices[i * 3 + 2]);
    // Normals
    sphere_buffer.push_back(normals[i * 3]);
    sphere_buffer.push_back(normals[i * 3 + 1]);
    sphere_buffer.push_back(normals[i * 3 + 2]);
    // Texture coordinates
    sphere_buffer.push_back(tex_coords[i * 2]);
    sphere_buffer.push_back(tex_coords[i * 2 + 1]);
  }

  GLuint vao, vbo, ebo;
  glGenVertexArrays(1, &vao);
  glGenBuffers(1, &vbo);
  glGenBuffers(1, &ebo);

  GLint in_position_loc = glGetAttribLocation(sphere_shader.program(), "in_position");
  GLint in_normal_loc = glGetAttribLocation(sphere_shader.program(), "in_normal");
  GLint in_tex_coords_loc = glGetAttribLocation(sphere_shader.program(), "in_tex_coords");
  GLint u_tex_sampler_loc = glGetUniformLocation(sphere_shader.program(), "u_tex_sampler_loc");
  GLint u_use_texture_loc = glGetUniformLocation(sphere_shader.program(), "u_use_texture");
  GLint u_model_loc = glGetUniformLocation(sphere_shader.program(), "u_model");
  GLint u_view_loc = glGetUniformLocation(sphere_shader.program(), "u_view");
  GLint u_proj_loc = glGetUniformLocation(sphere_shader.program(), "u_proj");

  glm::mat4 mat_model(1.0f);
  glm::mat4 mat_view = glm::lookAt(
      glm::vec3(0.0f, 0.0f, 3.0f),
      glm::vec3(0.0f, 0.0f, 0.0f),
      glm::vec3(0.0f, 1.0f, 0.0f)
      );

  float fovy = 2 * glm::atan(1.0f, 3.0f);
#ifndef GLM_FORCE_RADIANS
  fovy = glm::radians(fovy);
#endif

  glm::mat4 mat_proj = glm::perspective(
      fovy,
      (float) WINDOW_WIDTH / WINDOW_HEIGHT,
      2.0f,
      10.0f
      );
  mat_proj = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, 2.0f, 10.0f);

  glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
      glBufferData(
          GL_ARRAY_BUFFER,
          sizeof(float) * sphere_buffer.size(),
          sphere_buffer.data(),
          GL_STATIC_DRAW
          );
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
      glBufferData(
          GL_ELEMENT_ARRAY_BUFFER,
          sizeof(uint32_t) * indices.size(),
          indices.data(),
          GL_STATIC_DRAW
          );
    const size_t chunk_size = sizeof(float) * 8;
    glVertexAttribPointer(in_position_loc, 3, GL_FLOAT, GL_FALSE, chunk_size, nullptr);
    glVertexAttribPointer(in_normal_loc, 3, GL_FLOAT, GL_FALSE, chunk_size, (void*) (sizeof(float) * 3));
    glVertexAttribPointer(in_tex_coords_loc, 2, GL_FLOAT, GL_FALSE, chunk_size, (void*) (sizeof(float) * 6));
    glEnableVertexAttribArray(in_position_loc);
    glEnableVertexAttribArray(in_normal_loc);
    glEnableVertexAttribArray(in_tex_coords_loc);
  glBindVertexArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

  // Setup drawing mode
  glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
  glEnable(GL_DEPTH_TEST);

  bool continue_loop = true;
  bool wire_frame = false;
  int32_t use_texture = 1;
  glm::vec2 mouse_old(2.0f), mouse_new(0.0f);
  while (continue_loop) {
    // Process events
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      switch (event.type) {
        case SDL_KEYDOWN:
          if (event.key.keysym.scancode == SDL_SCANCODE_Q) {
            continue_loop = false;
          }
          if (event.key.keysym.scancode == SDL_SCANCODE_W) {
            wire_frame = !wire_frame;
            if (wire_frame) glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            else glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
          }
          if (event.key.keysym.scancode == SDL_SCANCODE_T) {
            use_texture = !use_texture;
          }
          break;
        case SDL_MOUSEBUTTONUP:
          if (event.button.button == SDL_BUTTON_LEFT) {
            mouse_old = glm::vec2(2.0f);
          }
          break;
      }
    }

    int mouse_x, mouse_y;
    if (SDL_GetMouseState(&mouse_x, &mouse_y) & SDL_BUTTON(SDL_BUTTON_LEFT)) {
      mouse_new.x = -1.0f + 2.0f * mouse_x / WINDOW_WIDTH;
      mouse_new.y = 1.0f - 2.0f * mouse_y / WINDOW_HEIGHT;
      glm::vec3 position_old(
          mouse_old,
          glm::sqrt(1 - (mouse_old.x * mouse_old.x + mouse_new.y * mouse_new.y))
          );
      glm::vec3 position_new(
          mouse_new,
          glm::sqrt(1 - (mouse_new.x * mouse_new.x + mouse_new.y * mouse_new.y))
          );
      if (mouse_old.x < 2.0f
          && glm::length(mouse_new - mouse_old) > 0.01f
          && glm::length(position_new) > 0.00001f
          && glm::length(position_old) > 0.00001f) // prevent division by zero
      {
        mat_model = calc_rot_mat(position_old, position_new) * mat_model;
      }
      mouse_old = mouse_new;
    }

    // Draw stuff
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    sphere_shader.use();
    glUniform1i(u_tex_sampler_loc, 0);
    glUniform1i(u_use_texture_loc, use_texture);
    glUniformMatrix4fv(u_model_loc, 1, GL_FALSE, glm::value_ptr(mat_model));
    glUniformMatrix4fv(u_proj_loc, 1, GL_FALSE, glm::value_ptr(mat_proj));
    glUniformMatrix4fv(u_view_loc, 1, GL_FALSE, glm::value_ptr(mat_view));
    glBindVertexArray(vao);
      glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
    sphere_shader.detach();

    SDL_GL_SwapWindow(window);
  }

  sphere_shader.clean();
}

void clean_up() {
  IMG_Quit();
  SDL_Quit();
}

int main() {
  if (SDL_Init(SDL_INIT_VIDEO)) {
    std::cerr << SDL_GetError() << std::endl;
    return 1;
  }

  if (IMG_Init(IMG_INIT_PNG) != IMG_INIT_PNG) {
    std::cerr << IMG_GetError() << std::endl;
    return 1;
  }

  // Initialize OpenGL window
  SDL_Window *window = SDL_CreateWindow(
      "Trackball",
      SDL_WINDOWPOS_CENTERED,
      SDL_WINDOWPOS_CENTERED,
      WINDOW_WIDTH,
      WINDOW_HEIGHT,
      SDL_WINDOW_OPENGL
      );
  SDL_GLContext gl_context = SDL_GL_CreateContext(window);
  SDL_GL_MakeCurrent(window, gl_context);
  if (SDL_GL_SetSwapInterval(-1) == -1) { // Try enabling adaptive VSync
    SDL_GL_SetSwapInterval(1); // Fallback to normal VSync
  }
  if (!gladLoadGL()) {
    std::cerr << "Could not load OpenGL" << std::endl;
    return 1;
  }

  std::cout << "OpenGL version: " << GLVersion.major << '.' << GLVersion.minor << std::endl;

  // Enter main loop
  main_loop(window);

  std::atexit(clean_up);
  return 0;
}
