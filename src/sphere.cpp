#include <cmath>

#include "sphere.hpp"

#define PI 3.14159265359

sphere::sphere(const double radius, const int n_stack, const int n_sector)
  : m_radius(radius), m_n_stack(n_stack), m_n_sector(n_sector)
{
  double phi = PI / -2; 
  double theta;
  const double phi_step = PI / n_stack;
  const double theta_step = 2 * PI / n_sector;
  double x, y, z;
  double radius_xz;

  for (int i = 0; i <= n_stack; ++i) {
    y = radius * sin(phi);
    theta = 0;
    radius_xz = radius * cos(phi);
    for (int j = 0; j <= n_sector; ++j) {
      // Vertices
      x = radius_xz * cos(theta);
      z = radius_xz * sin(theta);
      m_vertices.push_back(x);
      m_vertices.push_back(y);
      m_vertices.push_back(z);

      // Vertex normals
      m_normals.push_back(x / radius);
      m_normals.push_back(y / radius);
      m_normals.push_back(z / radius);

      // Texture coordinates
      m_tex_coords.push_back((float) j / n_sector);
      m_tex_coords.push_back((float) i / n_stack);

      theta += theta_step;
    }
    phi += phi_step;
  }

  // Indices
  int kh, kv;
  for (int i = 0; i < n_stack; ++i) {
    kh = i * (n_sector + 1);
    kv = kh + n_sector + 1;
    for (int j = 0; j < n_sector; ++j, ++kh, ++kv) {
      if (i != 0) {
        m_indices.push_back(kh);
        m_indices.push_back(kh + 1);
        m_indices.push_back(kv + 1);
      }
      if (i != (n_stack - 1)) {
        m_indices.push_back(kh);
        m_indices.push_back(kv + 1);
        m_indices.push_back(kv);
      }
    }
  }
}
