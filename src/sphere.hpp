#ifndef _SPHERE_HPP
#define _SPHERE_HPP

#include <vector>
#include <cinttypes>

class sphere {
  private:
    std::vector<float> m_vertices;
    std::vector<float> m_normals;
    std::vector<float> m_tex_coords;
    std::vector<uint32_t> m_indices;
    double m_radius;
    int m_n_stack, m_n_sector;

  public:
    sphere(const double radius, const int n_stack, const int n_sector);
    sphere(const sphere& rhs) 
      : m_vertices(rhs.m_vertices), m_normals(rhs.m_normals), m_indices(rhs.m_indices),
        m_radius(rhs.m_radius), m_n_stack(rhs.m_n_stack), m_n_sector(rhs.m_n_sector) {};

    double radius() const { return m_radius; }
    double n_stack() const { return m_n_stack; }
    double n_sector() const { return m_n_sector; }
    const std::vector<float>& vertices() const { return m_vertices; }
    const std::vector<float>& normals() const { return m_normals; }
    const std::vector<uint32_t>& indices() const { return m_indices; }
    const std::vector<float>& tex_coords() const { return m_tex_coords; }
};

#endif /* _SPHERE_HPP */
