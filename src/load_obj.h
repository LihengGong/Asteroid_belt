#ifndef OBJ_LOADER_H
#define OBJ_LOADER_H

#include <string>
#include <Eigen/Core>
#include <vector>

bool load_obj_mesh_data(
    const std::string filename,
    Eigen::MatrixXf& out_vertices,
    Eigen::MatrixXi& out_indices,
    Eigen::MatrixXf& out_normals
);

bool load_obj_mesh_data_new(
    const std::string filename,
    Eigen::MatrixXf& out_vertices,
    Eigen::MatrixXi& out_indices,
    Eigen::MatrixXf& out_normals
);

bool load_obj(const std::string filename,
              Eigen::MatrixXf& out_vertices,
              Eigen::MatrixXf& out_uvs,
              Eigen::MatrixXf& out_normals);

bool load_obj(const std::string filename,
              Eigen::MatrixXf& out_vertices,
              Eigen::MatrixXi& out_indices,
              Eigen::MatrixXf& out_uvs,
              Eigen::MatrixXf& out_normals);

bool load_texture(const std::string filename,
                  GLuint& texture_id);

struct Vertex {
    float x;
    float y;
    float z;
};

struct Texture {
    float x;
    float y;
};

struct Face {
    unsigned int mNumIndices;
    std::vector<unsigned int> mIndices;
};

class ObjMesh {
public:
    int mNumVertices;
    int mNumFaces;
    int mNumTexCoords;
    int mNumNormals;
    std::vector<Vertex> mVertices;
    std::vector<Vertex> mNormals;
    std::vector<Texture> mTextureCoords;
    std::vector<Face> mFaces;
};

bool load_obj_new(const std::string filename, ObjMesh** p_mesh);

#endif

