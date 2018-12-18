/********
 * Author: Liheng Gong(lg2848@nyu.edu)
 * This is the main program of this project.
 * Shader programs, VAOs, VBOs and VEOs are created and
 * data is binded to buffer.
 **********/
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <Eigen/Core>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "camera.h"
#include "load_obj.h"
#include "Helpers.h"

#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);

void generate_random_model_matrices(glm::mat4* model_matrices, unsigned int amount);
void config_instanced_array_buffer(VertexArrayObject& VAO,
                                   glm::mat4* model_matrices,
                                   unsigned int amount);
void config_instanced_array_buffer2(VertexArrayObject& VAO,
                                   Eigen::MatrixXf model_matrices,
                                   unsigned int amount);
void generate_random_model_matrices2(Eigen::MatrixXf& model_matrices, unsigned int amount);
void draw_planet(Program& shader, unsigned int planet_texture_id,
                 VertexArrayObject& VAO, unsigned int indices_size);
void draw_asteroids(Program& shader, VertexArrayObject& VAO, 
                    unsigned int rock_texture_id, unsigned int indices_size,
                    int amount);
void enable_bind_vertext_attrib_pointers2(
    VertexArrayObject& VAO,
    VertexBufferObject& VBO,
    ElementArrayBufferObject& EBO,
    const Eigen::MatrixXf& vertices,
    const Eigen::MatrixXi& indices
);

const unsigned int SCR_WIDTH = 1280;
const unsigned int SCR_HEIGHT = 720;
extern const char* asteroid_vertexcode;
extern const char* asteroid_fragmentcode;
extern const char* planet_vertexcode;
extern const char* planet_fragmentcode;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 155.0f));
float lastX = (float)SCR_WIDTH / 2.0;
float lastY = (float)SCR_HEIGHT / 2.0;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // GLFW captures the mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glEnable(GL_DEPTH_TEST);

    Program asteroid_shader;
    asteroid_shader.init(asteroid_vertexcode,
                         asteroid_fragmentcode,
                         "FragColor");

    Program planet_shader;
    planet_shader.init(planet_vertexcode,
                       planet_fragmentcode,
                       "FragColor");

    std::string planet_filename = "../data/objects/planet/planet.obj";
    std::string rock_filename = "../data/objects/rock/rock.obj";
    std::string rock_texturename = "../data/objects/rock/rock.png";
    std::string planet_texturename = "../data/objects/planet/planet_Quom1200.png";

    Eigen::MatrixXf rock_vertices = Eigen::MatrixXf::Identity(5, 20);
    Eigen::MatrixXf rock_uvs1 = Eigen::MatrixXf::Identity(2, 20);
    Eigen::MatrixXi rock_indices = Eigen::MatrixXi::Identity(3, 20);
    Eigen::MatrixXf rock_normals = Eigen::MatrixXf::Identity(3, 20);
    load_obj_mesh_data_new(
        rock_filename,
        rock_vertices,
        rock_indices,
        rock_normals
    );

    unsigned int rock_texture_id;
    load_texture(rock_texturename, rock_texture_id);
    std::cout << "rock texture id is : " << rock_texture_id << "\n";

    VertexArrayObject r_VAO;
    VertexBufferObject r_VBO;
    ElementArrayBufferObject r_EBO;
    enable_bind_vertext_attrib_pointers2(
        r_VAO,
        r_VBO,
        r_EBO,
        rock_vertices,
        rock_indices
    );

    Eigen::MatrixXf planet_vertices = Eigen::MatrixXf::Identity(5, 20);
    Eigen::MatrixXf planet_uvs = Eigen::MatrixXf::Identity(2, 20);
    Eigen::MatrixXi planet_indices = Eigen::MatrixXi::Identity(3, 20);
    Eigen::MatrixXf planet_normals = Eigen::MatrixXf::Identity(3, 20);
    
    load_obj_mesh_data_new(
        planet_filename,
        planet_vertices,
        planet_indices,
        planet_normals
    );

    unsigned int planet_texture_id;
    load_texture(planet_texturename, planet_texture_id);
    std::cout << "planet texture id is : " << planet_texture_id << "\n";

    VertexArrayObject p_VAO;
    VertexBufferObject p_vVBO;
    VertexBufferObject p_tVBO;
    ElementArrayBufferObject p_EBO;
    enable_bind_vertext_attrib_pointers2(
        p_VAO,
        p_vVBO,
        p_EBO,
        planet_vertices,
        planet_indices
    );

    unsigned int amount = 10;
    glm::mat4* model_matrices;
    model_matrices = new glm::mat4[amount];
    srand(glfwGetTime());
    // generate_random_model_matrices(model_matrices, amount);

    Eigen::MatrixXf eigen_model_matrices = Eigen::MatrixXf(4, 4 * amount);
    generate_random_model_matrices2(eigen_model_matrices, amount);

    // Eigen::MatrixXf eigen_model_matrices = Eigen::MatrixXf(4, 4 * amount);
    // for (int i = 0; i < amount; ++i) {
    //     for (int row = 0; row < 4; ++row) {
    //         for (int col = 0; col < 4; ++col) {
    //             eigen_model_matrices(row, col + i * 4) = model_matrices[i][col][row];
    //         }
    //     }
    // }

    std::cout << "eigen::::::::\n";
    // std::cout << eigen_model_matrics.transpose() << std::endl;

    config_instanced_array_buffer2(
        r_VAO, eigen_model_matrices, amount
        );

    while (!glfwWindowShouldClose(window))
    {

        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 1000.0f);
        glm::mat4 view = camera.GetViewMatrix();

        asteroid_shader.bind();
        glUniformMatrix4fv(
            asteroid_shader.uniform("projection"),
            1, GL_FALSE, &projection[0][0]
            );

        glUniformMatrix4fv(
            asteroid_shader.uniform("view"),
            1, GL_FALSE, &view[0][0]
            );

        planet_shader.bind();

        glUniformMatrix4fv(
            planet_shader.uniform("projection"),
            1, GL_FALSE, &projection[0][0]
            );

        glUniformMatrix4fv(
            planet_shader.uniform("view"),
            1, GL_FALSE, &view[0][0]
            );

        draw_planet(
            planet_shader,
            planet_texture_id,
            p_VAO,
            planet_vertices.cols()
            );
        check_gl_error();

        draw_asteroids(
            asteroid_shader,
            r_VAO,
            rock_texture_id,
            rock_vertices.cols(),
            amount);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

void enable_bind_vertext_attrib_pointers2(
    VertexArrayObject& VAO,
    VertexBufferObject& VBO,
    ElementArrayBufferObject& EBO,
    const Eigen::MatrixXf& vertices,
    const Eigen::MatrixXi& indices
) {
    // init buffers
    VAO.init();
    VBO.init();
    EBO.init();

    // bind buffers and data
    VAO.bind();

    VBO.update(vertices);

    // Textures
    EBO.bind();
    EBO.update(indices);

    // Describe buffer data format
    // vertices coordinates
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);

    // Texture coordinates
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

    glBindVertexArray(0);
}

void generate_random_model_matrices(glm::mat4* model_matrices, unsigned int amount) {
    float radius = 150.0f;
    float offset = 25.0f;

    for (unsigned int i = 0; i < amount; i++)
    {
        glm::mat4 model;
        // 1. translation: displace along circle with 'radius' in range [-offset, offset]
        float angle = (float)i / (float)amount * 360.0f;
        float displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
        float x = sin(angle) * radius + displacement;
        displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
        float y = displacement * 0.4f; // keep height of asteroid field smaller compared to width of x and z
        displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
        float z = cos(angle) * radius + displacement;
        model = glm::translate(model, glm::vec3(x, y, z));

        // 2. scale: Scale between 0.05 and 0.25f
        float scale = (rand() % 20) / 100.0f + 0.05;
        model = glm::scale(model, glm::vec3(scale));

        // 3. rotation: add random rotation around a (semi)randomly picked rotation axis vector
        float rotAngle = (rand() % 360);
        model = glm::rotate(model, rotAngle, glm::vec3(0.4f, 0.6f, 0.8f));

        // 4. now add to list of matrices
        model_matrices[i] = model;
    }
}

void translate(Eigen::Matrix4f& model, float dis_x, float dis_y, float dis_z) {
  Eigen::Matrix4f trans = Eigen::Matrix4f::Identity();
  trans.col(3) << dis_x, dis_y, dis_z, 1.0;
  model = trans * model;
}

void scale_model(Eigen::Matrix4f& model, float scale_x, float scale_y, float scale_z) {
    Eigen::Matrix4f scale_mat = Eigen::Matrix4f::Identity();
    scale_mat(0, 0) = scale_x;
    scale_mat(1, 1) = scale_y;
    scale_mat(2, 2) = scale_z;
    model = scale_mat * model;
}


void generate_random_model_matrices2(Eigen::MatrixXf& model_matrices, unsigned int amount) {
    float radius = 150.0f;
    float offset = 25.0f;

    for (unsigned int i = 0; i < amount; i++)
    {
        Eigen::Matrix4f model;
        // 1. translation: displace along circle with 'radius' in range [-offset, offset]
        float angle = (float)i / (float)amount * 360.0f;
        float displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
        float x = sin(angle) * radius + displacement;
        displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
        float y = displacement * 0.4f; // keep height of asteroid field smaller compared to width of x and z
        displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
        float z = cos(angle) * radius + displacement;
        // model = glm::translate(model, glm::vec3(x, y, z));
        translate(model, x, y, z);

        // 2. scale: Scale between 0.05 and 0.25f
        float scale = (rand() % 20) / 100.0f + 0.05;
        // model = glm::scale(model, glm::vec3(scale));
        scale_model(model, scale, scale, scale);

        // 3. rotation: add random rotation around a (semi)randomly picked rotation axis vector
        // float rotAngle = (rand() % 360);
        // model = glm::rotate(model, rotAngle, glm::vec3(0.4f, 0.6f, 0.8f));

        // 4. now add to list of matrices
        // model_matrices[i] = model;
        for (int row_ind = 0; row_ind < 4; ++row_ind) {
            for (int col_ind = 0; col_ind < 4; ++col_ind) {
                model_matrices(row_ind, col_ind + i * 4) = model(row_ind, col_ind);
            }
        }
    }
}

void config_instanced_array_buffer(VertexArrayObject& VAO,
                                   glm::mat4* model_matrices,
                                   unsigned int amount) {
    unsigned int buffer;
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(
        GL_ARRAY_BUFFER,
        amount * sizeof(glm::mat4),
        &model_matrices[0], GL_STATIC_DRAW);

    glBindVertexArray(VAO.id);
    unsigned int vec_size = sizeof(glm::vec4);
    unsigned int mat_size = sizeof(glm::mat4);
    for (int i = 3; i < 7; ++i) {
        glEnableVertexAttribArray(i);
        glVertexAttribPointer(
            i, 4, GL_FLOAT, GL_FALSE, mat_size,
            (void*)((int(i - 3)) * vec_size));
        glVertexAttribDivisor(i, 1);
    }

    glBindVertexArray(0);

    check_gl_error();

}

void config_instanced_array_buffer2(VertexArrayObject& VAO,
                                   Eigen::MatrixXf model_matrices,
                                   unsigned int amount) {
    unsigned int buffer;
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    // glBufferData(
    //     GL_ARRAY_BUFFER,
    //     amount * sizeof(glm::mat4),
    //     &model_matrices[0], GL_STATIC_DRAW);
    glBufferData(
        GL_ARRAY_BUFFER,
        model_matrices.size() * sizeof(float),
        model_matrices.data(),
        GL_STATIC_DRAW
    );

    glBindVertexArray(VAO.id);
    // unsigned int vec_size = sizeof(glm::vec4);
    // unsigned int mat_size = sizeof(glm::mat4);
    // for (int i = 3; i < 7; ++i) {
    //     glEnableVertexAttribArray(i);
    //     glVertexAttribPointer(
    //         i, 4, GL_FLOAT, GL_FALSE, mat_size,
    //         (void*)((int(i - 3)) * vec_size));
    //     glVertexAttribDivisor(i, 1);
    // }
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(
        3, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)0
        );
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(
        4, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), 
        (void*)(sizeof(glm::vec4))
        );
    glEnableVertexAttribArray(5);
    glVertexAttribPointer(
        5, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), 
        (void*)(2 * sizeof(glm::vec4))
        );
    glEnableVertexAttribArray(6);
    glVertexAttribPointer(
        6, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), 
        (void*)(3 * sizeof(glm::vec4))
        );

    glVertexAttribDivisor(3, 1);
    glVertexAttribDivisor(4, 1);
    glVertexAttribDivisor(5, 1);
    glVertexAttribDivisor(6, 1);

    glBindVertexArray(0);

    check_gl_error();

}

void draw_planet(Program& shader, unsigned int planet_texture_id,
                 VertexArrayObject& VAO, unsigned int indices_size) {
    glm::mat4 model;
    model = glm::translate(model, glm::vec3(0.0f, -3.0f, 0.0f));
    model = glm::scale(model, glm::vec3(4.0f, 4.0f, 4.0f));
    glUniformMatrix4fv(shader.uniform("model"), 1, GL_FALSE, &model[0][0]);
    glActiveTexture(GL_TEXTURE0);
    glUniform1i(shader.uniform("texture_diffuse1"), 0);
    glBindTexture(GL_TEXTURE_2D, planet_texture_id);
    glBindVertexArray(VAO.id);
    glDrawElements(GL_TRIANGLES, indices_size, GL_UNSIGNED_INT, 0);

    glBindVertexArray(0);

    glActiveTexture(GL_TEXTURE0);
    check_gl_error();
}

void draw_asteroids(Program& shader, VertexArrayObject& VAO, 
                    unsigned int rock_texture_id, unsigned int indices_size,
                    int amount) {
    shader.bind();

    glUniform1i(shader.uniform("texture_diffuse1"), 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, rock_texture_id);

    glBindVertexArray(VAO.id);
    glDrawElementsInstanced(GL_TRIANGLES, indices_size, GL_UNSIGNED_INT, 0, amount);

    glBindVertexArray(0);
    check_gl_error();
}


void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(yoffset);
}
