#ifndef POGL_FIRE_HH
#define POGL_FIRE_HH

#include "Particle.hh"
#include "ObjLoader.hh"
#include "TGA.hh"
#include <GL/gl.h>
#include <vector>
#include <cstdlib>

#define NUMBEROFPARTICLES 500

class Fire {
public:
    std::vector<Particle*> particles;
    float positions[NUMBEROFPARTICLES*3];
    GLuint vao;
    GLuint vbo;
    unsigned int deltaTime;


    Fire(GLuint program_id){
        //init first particle
        particles.push_back(new Particle());

        //init vao id, vbo id, and initial buffer
        glGenVertexArrays(1, &vao);TEST_OPENGL_ERROR();
        glBindVertexArray(vao);TEST_OPENGL_ERROR();

        glGenBuffers(1, &vbo);TEST_OPENGL_ERROR();
        glBindBuffer(GL_ARRAY_BUFFER, vbo);TEST_OPENGL_ERROR();
        glBufferData(GL_ARRAY_BUFFER, NUMBEROFPARTICLES*3*sizeof(float), &positions[0], GL_DYNAMIC_DRAW);TEST_OPENGL_ERROR();

        //init texture
        Tga texture("assets/Flame_Particle.tga");
        GLuint texture_id;
        GLint tex_location;

        GLint texture_units, combined_texture_units;
        glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &texture_units);TEST_OPENGL_ERROR();
        glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &combined_texture_units);TEST_OPENGL_ERROR();
        std::cout << "Limit 1 " <<  texture_units << " limit 2 " << combined_texture_units << std::endl;

        glGenTextures(1, &texture_id);TEST_OPENGL_ERROR();
        glActiveTexture(GL_TEXTURE0);TEST_OPENGL_ERROR();
        glBindTexture(GL_TEXTURE_2D,texture_id);TEST_OPENGL_ERROR();

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texture.GetWidth(), texture.GetHeight(), 0, GL_RGB, GL_UNSIGNED_BYTE, texture.GetPixels().data());TEST_OPENGL_ERROR();
        tex_location = glGetUniformLocation(program_id, "texture_sampler");TEST_OPENGL_ERROR();;
        glUniform1i(tex_location,0);TEST_OPENGL_ERROR();

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);TEST_OPENGL_ERROR();
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);TEST_OPENGL_ERROR();
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);TEST_OPENGL_ERROR();
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);TEST_OPENGL_ERROR();
    }

    void update() {
        //fill positions array with 10000 in order to not show any unupdated particles
        // (they are not in the camera field at this distance)
        std::fill_n(positions,NUMBEROFPARTICLES*3,10000);

        if (particles.size() < NUMBEROFPARTICLES - 6)
        {
            for (int i = 0; i < 6; i++)
            {
                particles.push_back(new Particle);
            }
        }

        for (unsigned int i = 0; i < particles.size(); i++){
            particles[i]->update(deltaTime);

            if (particles[i]->lifetime <= 0){
                delete particles[i];
                particles.erase(particles.begin() + i);
                i--;
            }else{
                positions[i*3] = particles[i]->position.x;
                positions[(i*3)+1] = particles[i]->position.y;
                positions[(i*3)+2] = particles[i]->position.z;
            }
        }


        //update the vertex buffer
        glBindVertexArray(vao);TEST_OPENGL_ERROR();
        glBindBuffer(GL_ARRAY_BUFFER, vbo);TEST_OPENGL_ERROR();
        glBufferData(GL_ARRAY_BUFFER, NUMBEROFPARTICLES*3*sizeof(float), &positions[0], GL_DYNAMIC_DRAW);TEST_OPENGL_ERROR();
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0 );TEST_OPENGL_ERROR();
        glBindVertexArray(0);TEST_OPENGL_ERROR();
    }

   void draw(){
       //disable the depth mask in order to stop depth writes and thus blending artifacts while keeping the flame from drawing over other objects
       glDepthMask(false);
       //bind the VAO and draw it
       glBindVertexArray(vao);TEST_OPENGL_ERROR();
       glEnableVertexAttribArray(0);TEST_OPENGL_ERROR();
       glDrawArrays(GL_POINTS, 0, NUMBEROFPARTICLES);TEST_OPENGL_ERROR();
       //unbind appropriate data
       glDisableVertexAttribArray(0);TEST_OPENGL_ERROR();
       glBindVertexArray(0);TEST_OPENGL_ERROR();
       //re-enable the depth mask
       glDepthMask(true);
   }
};


#endif //POGL_FIRE_HH
