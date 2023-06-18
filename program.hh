#ifndef TP1_PROGRAM_HH
#define TP1_PROGRAM_HH

#include <string>
#include <GL/glew.h>
#include <GL/freeglut.h>

#include <fstream>
#include <iostream>

#define TEST_OPENGL_ERROR()\
  do {\
    GLenum err = glGetError();\
    if (err != GL_NO_ERROR) std::cerr << "OpenGL ERROR!, line : " << __LINE__ << std::endl;\
    switch (err)\
    {\
    case GL_INVALID_ENUM:\
        std::cerr << "GL_INVALID_ENUM" << std::endl; break;\
    case GL_INVALID_VALUE:\
        std::cerr << "GL_INVALID_VALUE" << std::endl; break; \
    case GL_INVALID_OPERATION:\
        std::cerr << "GL_INVALID_OPERATION" << std::endl; break;\
    case GL_INVALID_FRAMEBUFFER_OPERATION:\
        std::cerr << "GL_INVALID_FRAMEBUFFER_OPERATION" << std::endl; break;\
    case GL_OUT_OF_MEMORY:\
        std::cerr << "GL_OUT_OF_MEMORY" << std::endl; break;\
    case GL_STACK_UNDERFLOW:\
        std::cerr << "GL_STACK_UNDERFLOW" << std::endl; break;\
    case GL_STACK_OVERFLOW:\
        std::cerr << "GL_STACK_OVERFLOW" << std::endl; break; \
    default:\
        break;\
    }\
  } while(0)

std::string load(const std::string &filename) {
    std::ifstream input_src_file(filename, std::ios::in);
    std::string ligne;
    std::string file_content="";
    if (input_src_file.fail()) {
        std::cerr << "FAIL\n";
        return "";
    }
    while(getline(input_src_file, ligne)) {
        file_content = file_content + ligne + "\n";
    }
    file_content += '\0';
    input_src_file.close();
    return file_content;
}

class program {
public:
    GLuint program_id;
    program(GLuint program_id){ this->program_id = program_id;}
    ~program(){}

    static program* make_program(std::string& vertex_shader_src, std::string& geometry_shader_src, std::string& fragment_shader_src) {
        //load shaders in strings
        std::string vertex_src = load(vertex_shader_src);
        std::string geometry_src = load(geometry_shader_src);
        std::string fragment_src = load(fragment_shader_src);
        char *vertex_shd_src = (char*)std::malloc(vertex_src.length()*sizeof(char));
        char *geometry_shd_src = (char*)std::malloc(geometry_src.length()*sizeof(char));
        char *fragment_shd_src = (char*)std::malloc(fragment_src.length()*sizeof(char));
        vertex_src.copy(vertex_shd_src,vertex_src.length());
        geometry_src.copy(geometry_shd_src,geometry_src.length());
        fragment_src.copy(fragment_shd_src,fragment_src.length());


        //compile fragment shader
        unsigned int fragment_shader_id = glCreateShader(GL_FRAGMENT_SHADER);TEST_OPENGL_ERROR();
        glShaderSource(fragment_shader_id, 1, (const GLchar**)&(fragment_shd_src), NULL);TEST_OPENGL_ERROR();
        glCompileShader(fragment_shader_id);TEST_OPENGL_ERROR();
        int success;
        char infoLog[512];
        glGetShaderiv(fragment_shader_id, GL_COMPILE_STATUS, &success);TEST_OPENGL_ERROR();
        if (!success) {
            glGetShaderInfoLog(fragment_shader_id, 512, NULL, infoLog);
            std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED " << infoLog << std::endl;
        } else {
            glGetShaderInfoLog(fragment_shader_id, 512, NULL, infoLog);
            std::cout << "SUCCESS::SHADER::FRAGMENT::COMPILATION_SUCCESS " << infoLog << std::endl;
        }

        //compile geometry shader
        unsigned int geometry_shader_id = glCreateShader(GL_GEOMETRY_SHADER);TEST_OPENGL_ERROR();
        glShaderSource(geometry_shader_id, 1, (const GLchar**)&(geometry_shd_src), NULL);TEST_OPENGL_ERROR();
        glCompileShader(geometry_shader_id);TEST_OPENGL_ERROR();
        glGetShaderiv(geometry_shader_id, GL_COMPILE_STATUS, &success);TEST_OPENGL_ERROR();
        if (!success) {
            glGetShaderInfoLog(geometry_shader_id, 512, NULL, infoLog);
            std::cout << "ERROR::SHADER::GEOMETRY::COMPILATION_FAILED " << infoLog << std::endl;
        } else {
            glGetShaderInfoLog(geometry_shader_id, 512, NULL, infoLog);
            std::cout << "SUCCESS::SHADER::GEOMETRY::COMPILATION_SUCCESS " << infoLog << std::endl;
        }

       //compile vertex shader
       unsigned int vertex_shader_id = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertex_shader_id, 1, (const GLchar**)&(vertex_shd_src), NULL);
        glCompileShader(vertex_shader_id);TEST_OPENGL_ERROR();
        glGetShaderiv(vertex_shader_id, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(vertex_shader_id, 512, NULL, infoLog);
            std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED " << infoLog << std::endl;
        } else {
            glGetShaderInfoLog(vertex_shader_id, 512, NULL, infoLog);
            std::cout << "SUCCESS::SHADER::VERTEX::COMPILATION_SUCCESS " << infoLog << std::endl;
        }

        //link shaders
        unsigned int program_id = glCreateProgram();
        glAttachShader(program_id, vertex_shader_id);TEST_OPENGL_ERROR();
        glAttachShader(program_id, geometry_shader_id);TEST_OPENGL_ERROR();
        glAttachShader(program_id, fragment_shader_id);TEST_OPENGL_ERROR();
        glLinkProgram(program_id);TEST_OPENGL_ERROR();
        glGetProgramiv(program_id, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(program_id, 512, NULL, infoLog);
            std::cout << "ERROR::PROGRAM::LINKING_FAILED " << infoLog << std::endl;
        } else {
            glGetProgramInfoLog(program_id, 512, NULL, infoLog);
            std::cout << "SUCCESS::PROGRAM::LINKING_SUCCESS " << infoLog << std::endl;
        }
        /*glDetachShader(program_id, vertex_shader_id);
        glDetachShader(program_id, fragment_shader_id);
        glDeleteShader(vertex_shader_id);
        glDeleteShader(fragment_shader_id);*/

        return new program(program_id);
    }

    bool is_ready(){
        return true;
    }

    void use(){
        glUseProgram(program_id);TEST_OPENGL_ERROR();
    }
};


#endif //TP1_PROGRAM_HH
