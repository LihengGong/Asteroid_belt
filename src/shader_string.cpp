// Shader code
// I use version 330 because version 150 does not support layout

// Note: for the instanced shading, model_matrix will be used as a per-instance transformation
// matrix. Note that a mat4 consumes **4** consecutive locations, so
// this will actually sit in locations, 3, 4, 5, and 6.
// model_matrix will be used as a per-instance transformation
// matrix. Note that a mat4 consumes 4 consecutive locations, so
// this will actually sit in locations, 3, 4, 5, and 6.

const char * asteroid_vertexcode = 
        "#version 330 core                                                   "
        "layout (location = 0) in vec3 aPos;                                 "
        "layout (location = 2) in vec2 aTexCoords;                           "
        "layout (location = 3) in mat4 aInstanceMatrix;                      "
        "out vec2 TexCoords;                                                 "
        "uniform mat4 projection;                                            "
        "uniform mat4 view;                                                  "
        "void main()                                                         "
        "{                                                                   "
        "  TexCoords = aTexCoords;                                           "
        "  gl_Position = projection * view * aInstanceMatrix * vec4(aPos, 1.0f);     "
        "}                                                                   ";

const char* asteroid_fragmentcode = 
        "#version 330 core                                       "
        "out vec4 FragColor;                                     "
        "in vec2 TexCoords;                                      "
        "uniform sampler2D texture_diffuse1;                    "
        "void main()                                             "
        "{                                                       "
        "    FragColor = texture(texture_diffuse1, TexCoords);   "
        "}                                                       ";


const char* planet_vertexcode = 
        "#version 330 core                                                   "
        "layout (location = 0) in vec3 aPos;                                 "
        "layout (location = 2) in vec2 aTexCoords;                           "
        "out vec2 TexCoords;                                                 "
        "uniform mat4 projection;                                            "
        "uniform mat4 view;                                                  "
        "uniform mat4 model;                                                 "
        "void main()                                                         "
        "{                                                                   "
        "  TexCoords = aTexCoords;                                           "
        "  gl_Position = projection * view * model * vec4(aPos, 1.0f);       "
        "}                                                                   ";


const char * planet_fragmentcode = 
        "#version 330 core                                       "
        "out vec4 FragColor;                                     "
        "in vec2 TexCoords;                                      "
        "uniform sampler2D texture_diffuse1;                     "
        "void main()                                             "
        "{                                                       "
        "    FragColor = texture(texture_diffuse1, TexCoords);   "
        "}                                                       ";
