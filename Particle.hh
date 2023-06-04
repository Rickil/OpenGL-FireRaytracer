#ifndef TP1_PARTICULE_HH
#define TP1_PARTICULE_HH

#include "Vector3.hh"

class Particle {
public:
    Vector3 position;
    Vector3 velocity;
    Vector3 color;
    float life;

    GLuint vao;
    GLuint vertex_VBO;
    GLuint color_VBO;
    std::vector<float> vertex_buffer;
    std::vector<float> color_buffer;

    Particle(Vector3 &color) : color(color) {

        //init vertex_buffer
        vertex_buffer = {
                -0.5f, -0.5f, 0.0f,
                0.5f, -0.5f, 0.0f,
                -0.5f, 0.5f, 0.0f,
                0.5f, 0.5f, 0.0f,
        };
        Vector3::translateVerticesOnVector(vertex_buffer, position);

        //init color_buffer
        color_buffer = color.toArray();

        //init vao
        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);

        //init vbos
        glGenBuffers(1, &vertex_VBO);
        glGenBuffers(1, &color_VBO);
    }

    void updateVAO(GLuint program_id){
        //get vertex location
        GLint vertex_location = glGetAttribLocation(program_id,"vPosition");TEST_OPENGL_ERROR();
        GLint color_location = glGetAttribLocation(program_id,"vColor");TEST_OPENGL_ERROR();

        //bind current vao
        glBindVertexArray(vao);

        //update vertex vbo
        glBindBuffer(GL_ARRAY_BUFFER, vertex_VBO);
        glBufferData(GL_ARRAY_BUFFER, vertex_buffer.size()*sizeof(float), vertex_buffer.data(), GL_DYNAMIC_DRAW);
        glVertexAttribPointer(vertex_location, 3, GL_FLOAT, GL_FALSE, 0, 0);
        glEnable(vertex_location);

        //update color vbo
        glBindBuffer(GL_ARRAY_BUFFER, color_VBO);
        glBufferData(GL_ARRAY_BUFFER, color_buffer.size()*sizeof(float), color_buffer.data(), GL_DYNAMIC_DRAW);
        glVertexAttribPointer(color_location, 3, GL_FLOAT, GL_FALSE, 0, 0);
        glEnable(color_location);
    }

};


#endif //TP1_PARTICULE_HH
