#ifndef _WAVEFRONT_OBJ_H_
#define _WAVEFRONT_OBJ_H_

#include <vector>
#include <string>
#include <utility>
#include <GL/glut.h>
#include <glm/glm.hpp>

class wavefront_obj_t  {
  public:
    static constexpr GLuint gl_primitive_mode = GL_POLYGON;

    struct face_t {
        std::size_t idx_begin;
        std::size_t count;
        glm::dvec3 normal;
    };
    std::vector<glm::dvec3> vertices;	// x, y, z
    std::vector<glm::dvec3> normals;	// x, y, z: unit vector or {0, 0, 0}
    std::vector<glm::dvec2> texcoords;	// u, v
    std::vector<face_t> faces;
    std::vector<int> vertex_indices;
    std::vector<int> normal_indices;
    std::vector<int> texcoord_indices;

    bool is_flat;
    std::pair<glm::dvec3, glm::dvec3> aabb; // bounding box

    wavefront_obj_t( const std::string &path ); // constructor: load from the file
    void draw();
};


#endif // _WAVEFRONT_OBJ_H_