/**************************
 * Author: Liheng Gong(lg2848@nyu.edu)
 * Helper functions are implemented in this file
 * to load *.obj file and texture image.
***************************/

#include <iostream>
#include <fstream>
#include <cstdio>
#include <cstring>
#include <vector>
#include <string>
// Linear Algebra Library
#include <Eigen/Core>
// #include <Eigen/Dense>

#include "Helpers.h"
#include "stb_image.h"
#include "load_obj.h"

bool IS_DEBUG_LOAD_OBJ = false;

bool load_obj_mesh_data_new(
    const std::string filename,
    Eigen::MatrixXf& out_vertices,
    Eigen::MatrixXi& out_indices,
    Eigen::MatrixXf& out_normals
) {

    ObjMesh* p_mesh = new ObjMesh();
    load_obj_new(filename, &p_mesh);
    int col_num = 0;

    std::cout << "()()()()()()(\n";
    if (p_mesh == nullptr) {
        std::cout << "something is wrong here\n";
        return false;
    } else {
        col_num = p_mesh->mNumVertices;
        printf("pointer is %p\n", p_mesh);
        std::cout << "&&&&&&&" << col_num << "\n";
    }

    // int col_num111 = p_mesh->mNumVertices;

    std::cout << "&&&&&&&&&&&&&&&&&";

    std::cout << std::endl << std::endl;

    std::cout << "33333333333333 col_num: " << col_num;

    std::cout << std::endl << std::endl;

    out_vertices.resize(5, col_num);

    std::cout << "555555555555 col_num: " << col_num;

    std::cout << std::endl << std::endl;
    std::cout << p_mesh->mVertices.size() << " is vert size\n";

    // Vertices from mesh data
    for (int vi = 0; vi < col_num; ++vi) {
        out_vertices(0, vi) = p_mesh->mVertices[vi].x;
        out_vertices(1, vi) = p_mesh->mVertices[vi].y;
        out_vertices(2, vi) = p_mesh->mVertices[vi].z;

        out_vertices(3, vi) = p_mesh->mTextureCoords[vi].x;
        out_vertices(4, vi) = p_mesh->mTextureCoords[vi].y;
    }

    std::cout << "666666666 col_num: " << col_num;

    std::cout << std::endl << std::endl;

    out_indices.resize(1, col_num);
    // std::vector<int> vec_indices;
    for (int i = 0; i < col_num; ++i) {
        out_indices(0, i) = i;
    }

    IS_DEBUG_LOAD_OBJ = false;
    if (IS_DEBUG_LOAD_OBJ) {
        std::cout << "indices is : \n";
        std::cout << out_indices.transpose() << "\n";
    }
    IS_DEBUG_LOAD_OBJ = false;

    std::cout << "doneeeeeeeeeeeeeeeeeeeeee\n";
    return true;
}


bool load_obj_new(const std::string filename, ObjMesh** p_mesh) {
    std::cout << "Loading OBJ file " << filename << "\n";

    FILE *file = fopen(filename.c_str(), "r");
    std::cout << "load_obj_new\n";
    if (file == nullptr) {
        std::cout << "Cannot load the file. Please make sure the file exists\n";
        return false;
    }

    const int MAX_CHARS_PER_LINE = 128;
    std::vector<Vertex> vertices;
    std::vector<Texture> uvs;
    std::vector<Vertex> normals;
    std::vector<unsigned int> vertexIndices, uvIndices, normalIndices;
    while (true) {
        char curline_chars[MAX_CHARS_PER_LINE];
        int res = fscanf(file, "%s", curline_chars);
        if (res == EOF) {
            // End of file. Quit the loop.
            break;
        }

        std::string cur_tag = std::string(curline_chars);
        if (IS_DEBUG_LOAD_OBJ)
            std::cout << "cur_line:" << cur_tag << std::endl;
        if (cur_tag == "v") {
            // vertices coordinates
            float fx, fy, fz;
            fscanf(file, "%f %f %f\n", &fx, &fy, &fz);
            Vertex vert;
            vert.x = fx;
            vert.y = fy;
            vert.z = fz;
            vertices.push_back(vert);
            if (IS_DEBUG_LOAD_OBJ)
                std::cout << "vertice: " << fx << ", " << fy << ", " << fz << "\n";
        } else if (cur_tag == "vt") {
            // Texture UV coordinates
            float fu, fv;
            fscanf(file, "%f %f\n", &fu, &fv);
            Texture tex;
            tex.x = fu;
            tex.y = fv;
            uvs.push_back(tex);
            if (IS_DEBUG_LOAD_OBJ)
                std::cout << "texture: " << fu << ", " << fv << "\n";
        } else if (cur_tag == "vn") {
            // Normals
            float fnx, fny, fnz;
            fscanf(file, "%f %f %f\n", &fnx, &fny, &fnz);
            Vertex norm;
            norm.x = fnx;
            norm.y = fny;
            norm.z = fnz;
            normals.push_back(norm);
            if (IS_DEBUG_LOAD_OBJ)
                std::cout << "normal: " << fnx << ", " << fny << ", " << fnz << "\n";
        } else if (cur_tag == "f") {
            // Indexes
            const int NUM_AXIS = 3;
            unsigned int vertex_ind[NUM_AXIS], 
                         uv_ind[NUM_AXIS],
                         norm_ind[NUM_AXIS];
            int num_match = fscanf(file,
                        "%d/%d/%d %d/%d/%d %d/%d/%d\n",
                        &vertex_ind[0], &uv_ind[0], &norm_ind[0],
                        &vertex_ind[1], &uv_ind[1], &norm_ind[1],
                        &vertex_ind[2], &uv_ind[2], &norm_ind[2]);
            if (num_match != 9) {
                std::cerr << "File format cannot be recognized\n";
                fclose(file);
                return false;
            }
            for (int i = 0; i < NUM_AXIS; ++i) {
                vertexIndices.push_back(vertex_ind[i]-1);
                uvIndices.push_back(uv_ind[i]-1);
                normalIndices.push_back(norm_ind[i]-1);
            }
            (*p_mesh)->mNumFaces++;
            if (IS_DEBUG_LOAD_OBJ)
                std::cout << "face: " << vertex_ind[0] << ", " << vertex_ind[1] << std::endl;
        } else {
            // prepare to parse other keywords or comments
            const int DUMMY_BUF_LEN = 1000;
            char dummy_buf[DUMMY_BUF_LEN];
            fgets(dummy_buf, DUMMY_BUF_LEN, file);
        }
    }

    // Now save vertices coordinates, vertices indexes, texture
    // uv coordinates and normals in Eigen matrices
    std::cout << "1111vertI: " << vertexIndices.size() << ", uvI: " << uvIndices.size()
              << ", normalI: " << normalIndices.size() << "\n";
    std::cout << "111vertN:" << vertices.size() << ", uvN: " << uvs.size()
              << ", normalN: " << normals.size() << "\n";
    std::cout << "1111num faces: " << (*p_mesh)->mNumFaces << "\n";

    for (int i = 0; i < vertexIndices.size(); ++i) {
        (*p_mesh)->mNumVertices++;

        (*p_mesh)->mVertices.push_back(vertices[vertexIndices[i]]);
        (*p_mesh)->mTextureCoords.push_back(uvs[uvIndices[i]]);
        (*p_mesh)->mNormals.push_back(normals[normalIndices[i]]);
    }

    std::cout << "load_obj_new 2222\n";
    // for (int i = 0; i < (*p_mesh)->mNumVertices; ++i) {
    //     std::cout << (*p_mesh)->mVertices[i].x << ", "
    //               <<  (*p_mesh)->mVertices[i].y << ","
    //               <<  (*p_mesh)->mVertices[i].z << "\n";
    // }

    std::cout << "end of load .........\n";

    fclose(file);

    std::cout << "end of load2 .........\n";
    return true;
}



bool load_obj(const std::string filename,
              Eigen::MatrixXf& out_vertices,
              Eigen::MatrixXi& out_indices,
              Eigen::MatrixXf& out_uvs,
              Eigen::MatrixXf& out_normals) {
    std::cout << "Loading OBJ file " << filename << "\n";

    FILE *file = fopen(filename.c_str(), "r");
    if (file == nullptr) {
        std::cout << "Cannot load the file. Please make sure the file exists\n";
        return false;
    }

    const int MAX_CHARS_PER_LINE = 128;
    std::vector<float> vertices;
    std::vector<float> uvs;
    std::vector<float> normals;
    std::vector<unsigned int> vertexIndices, uvIndices, normalIndices;
    while (true) {
        char curline_chars[MAX_CHARS_PER_LINE];
        int res = fscanf(file, "%s", curline_chars);
        if (res == EOF) {
            // End of file. Quit the loop.
            break;
        }

        std::string cur_tag = std::string(curline_chars);
        if (IS_DEBUG_LOAD_OBJ)
            std::cout << "cur_line:" << cur_tag << std::endl;
        if (cur_tag == "v") {
            // vertices coordinates
            float fx, fy, fz;
            // std::istringstream s(cur_line.substr(2));
            fscanf(file, "%f %f %f\n", &fx, &fy, &fz);
            vertices.push_back(fx);
            vertices.push_back(fy);
            vertices.push_back(fz);
            if (IS_DEBUG_LOAD_OBJ)
                std::cout << "vertice: " << fx << ", " << fy << ", " << fz << "\n";
        } else if (cur_tag == "vt") {
            // Texture UV coordinates
            float fu, fv;
            fscanf(file, "%f %f\n", &fu, &fv);
            uvs.push_back(fu);
            uvs.push_back(fv);
            if (IS_DEBUG_LOAD_OBJ)
                std::cout << "texture: " << fu << ", " << fv << "\n";
        } else if (cur_tag == "vn") {
            // Normals
            float fnx, fny, fnz;
            fscanf(file, "%f %f %f\n", &fnx, &fny, &fnz);
            normals.push_back(fnx);
            normals.push_back(fny);
            normals.push_back(fnz);
            if (IS_DEBUG_LOAD_OBJ)
                std::cout << "normal: " << fnx << ", " << fny << ", " << fnz << "\n";
        } else if (cur_tag == "f") {
            // Indexes
            const int NUM_AXIS = 3;
            unsigned int vertex_ind[NUM_AXIS], 
                         uv_ind[NUM_AXIS],
                         norm_ind[NUM_AXIS];
            int num_match = fscanf(file,
                        "%d/%d/%d %d/%d/%d %d/%d/%d\n",
                        &vertex_ind[0], &uv_ind[0], &norm_ind[0],
                        &vertex_ind[1], &uv_ind[1], &norm_ind[1],
                        &vertex_ind[2], &uv_ind[2], &norm_ind[2]);
            if (num_match != 9) {
                std::cerr << "File format cannot be recognized\n";
                fclose(file);
                return false;
            }
            for (int i = 0; i < NUM_AXIS; ++i) {
                vertexIndices.push_back(vertex_ind[i]-1);
                uvIndices.push_back(uv_ind[i]-1);
                normalIndices.push_back(norm_ind[i]-1);
            }
            if (IS_DEBUG_LOAD_OBJ)
                std::cout << "face: " << vertex_ind[0] << ", " << vertex_ind[1] << std::endl;
        } else {
            // prepare to parse other keywords or comments
            const int DUMMY_BUF_LEN = 1000;
            char dummy_buf[DUMMY_BUF_LEN];
            fgets(dummy_buf, DUMMY_BUF_LEN, file);
        }
    }

    // Now save vertices coordinates, vertices indexes, texture
    // uv coordinates and normals in Eigen matrices
    std::cout << "vertI: " << vertexIndices.size() << ", uvI: " << uvIndices.size()
              << ", normalI: " << normalIndices.size() << "\n";
    std::cout << "vertN:" << vertices.size() << ", uvN: " << uvs.size()
              << ", normalN: " << normals.size() << "\n";

    unsigned int cols = vertexIndices.size();
    out_vertices.resize(3, cols);
    out_uvs.resize(2, cols);
    out_normals.resize(3, cols);
    out_indices.resize(1, cols);
    out_vertices = Eigen::MatrixXf::Zero(3, cols);
    out_uvs = Eigen::MatrixXf::Zero(2, cols);
    out_normals = Eigen::MatrixXf::Zero(3, cols);
    out_indices = Eigen::MatrixXi::Zero(1, cols);

    for (int i = 0; i < cols; ++i) {
        int v_i = vertexIndices[i];
        int uv_i = uvIndices[i];
        int nml_i = normalIndices[i];
        for (int k = 0; k < 2; ++k) {
            out_vertices(k, i) = vertices[v_i + k];
            out_normals(k, i) = normals[nml_i + k];
            out_uvs(k, i) = uvs[uv_i + k];
        }
        out_vertices(2, i) = vertices[v_i + 2];
        out_normals(2, i) = normals[nml_i + 2];

        out_indices(0, i) = i;
    }

    if (IS_DEBUG_LOAD_OBJ) {
        std::cout << "out_vertices:\n" << out_vertices.transpose() << "\n\n";
        std::cout << "out_uvs:\n" << out_uvs.transpose() << "\n\n";
        std::cout << "out_normals:\n" << out_normals.transpose() << "\n";
    }

    fclose(file);
    return true;
}

// The texture file is usually  .png or .bmp format,
// so we use STB library to load the picture(s).
bool load_texture(const std::string filename,
                  GLuint& texture_id) {
    unsigned int t_id;
    glGenTextures(1, &t_id);
    int width, height, nr_components;
    unsigned char* data = stbi_load(filename.c_str(),
                                    &width, &height, 
                                    &nr_components, 0);
    if (data == nullptr) {
        std::cerr << "Error loading texture file" << filename << "\n";
        texture_id = -1;
        stbi_image_free(data);
        return false;
    }
    texture_id = t_id;

    // OpenGL texture related stuff
    GLenum color_format;
    switch (nr_components) {
        case 1:
            color_format = GL_RED;
        break;
        case 3:
            color_format = GL_RGB;
        break;
        case 4:
            color_format = GL_RGBA;
        break;
        default:
        break;
    }

    glBindTexture(GL_TEXTURE_2D, texture_id);
    glTexImage2D(GL_TEXTURE_2D, 0, color_format, width, height, 0, color_format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    std::cout << color_format << " = color format\n";
    std::cout << nr_components << " = nr components\n";
    check_gl_error();

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    check_gl_error();

    stbi_image_free(data);
    return true;
}
