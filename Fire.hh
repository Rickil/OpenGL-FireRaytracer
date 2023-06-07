#ifndef POGL_FIRE_HH
#define POGL_FIRE_HH

#include "Particle.hh"
#include "ObjLoader.hh"
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


    Fire(){
        //init vao id, vbo id, and initial buffer
        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);

        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, NUMBEROFPARTICLES*3*sizeof(float), &positions[0], GL_DYNAMIC_DRAW);

        //init first particle
        particles.push_back(new Particle());
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
        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, NUMBEROFPARTICLES*3*sizeof(float), &positions[0], GL_DYNAMIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0 );
        glBindVertexArray(0);
    }

   void draw(){
       //disable the depth mask in order to stop depth writes and thus blending artifacts while keeping the flame from drawing over other objects
       glDepthMask(false);
       //bind the VAO and draw it
       glBindVertexArray(vao);
       glEnableVertexAttribArray(0);
       glDrawArrays(GL_POINTS, 0, NUMBEROFPARTICLES);
       //unbind appropriate data
       glDisableVertexAttribArray(0);
       glBindVertexArray(0);
       //re-enable the depth mask
       glDepthMask(true);
   }
};


#endif //POGL_FIRE_HH
