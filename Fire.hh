#ifndef POGL_FIRE_HH
#define POGL_FIRE_HH

#include "Particle.hh"
#include "ObjLoader.hh"
#include "TGA.hh"
#include "KDTree.hh"
#include <GL/gl.h>
#include <vector>
#include <cstdlib>

#define NUMBEROFPARTICLES 1500

class Fire {
public:
    KDTree* m_root;
    Vector3 position;
    std::vector<Particle*> particles;
    std::vector<Particle*> m_particlesInRange;
    float positions[NUMBEROFPARTICLES*3];
    float colors[NUMBEROFPARTICLES*3];
    GLuint program_id;
    GLuint vao;
    GLuint vbo;
    GLuint color_vbo;
    unsigned int deltaTime;


    Fire(GLuint program_id){
        this->program_id = program_id;

        //init color array
        for (int i=0; i < NUMBEROFPARTICLES*3; i+=3){
            colors[i] = 1.0;
            colors[i+1] = 0;
            colors[i+2] = 0;
        }

        //init first particle
        particles.push_back(new Particle());

        //init vao id, vbo id, and initial buffers
        glGenVertexArrays(1, &vao);TEST_OPENGL_ERROR();
        glBindVertexArray(vao);TEST_OPENGL_ERROR();

        glGenBuffers(1, &vbo);TEST_OPENGL_ERROR();
        glBindBuffer(GL_ARRAY_BUFFER, vbo);TEST_OPENGL_ERROR();
        glBufferData(GL_ARRAY_BUFFER, NUMBEROFPARTICLES*3*sizeof(float), &positions[0], GL_DYNAMIC_DRAW);TEST_OPENGL_ERROR();

        /*glGenBuffers(1, &color_vbo);TEST_OPENGL_ERROR();
        glBindBuffer(GL_ARRAY_BUFFER, color_vbo);TEST_OPENGL_ERROR();
        glBufferData(GL_ARRAY_BUFFER, NUMBEROFPARTICLES*3*sizeof(float), &colors[0], GL_DYNAMIC_DRAW);TEST_OPENGL_ERROR();*/

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

        glBindVertexArray(0);TEST_OPENGL_ERROR();
    }

    void update() {
        //fill positions array with 10000 in order to not show any unupdated particles
        // (they are not in the camera field at this distance)
        std::fill_n(positions,NUMBEROFPARTICLES*3,10000);

        if (NUMBEROFPARTICLES >= 6 && particles.size() < NUMBEROFPARTICLES - 6)
        {
            for (int i = 0; i < 6; i++)
            {
                particles.push_back(new Particle);
            }
        }

        float maxDist = 100000.0f;
        //generate a new KDTree of all the particles
        m_root = new KDTree(2,particles);
        //for every particle
        for (unsigned int i = 0; i < particles.size(); i++)
        {
            //zero the vector of particles within range
            m_particlesInRange.clear();
            //reset the maximum distance used in the Findm_particlesInRange() function
            maxDist = 100000.0f;
            //find the particles within a range of 0.3 units
            m_root->FindParticlesInRange(0.3f,particles[i]->position,m_particlesInRange,&maxDist);
            //update the particle
            particles[i]->update(m_particlesInRange, deltaTime);
            //if the particle has reached the end of its lifetime delete it and step back a particle in order to not miss any out
            if (particles[i]->lifetime < 0)
            {
                delete particles[i];
                particles.erase(particles.begin()+i);
                i--;
            }
                //otherwise push it's position to the positions array
            else
            {
                positions[(i*3)] = particles[i]->position.x;
                positions[(i*3)+1] = particles[i]->position.y;
                positions[(i*3)+2] = particles[i]->position.z;
            }
        }
        //delete the tree
        m_root->DestroyTree();
        delete m_root;

        /*for (unsigned int i = 0; i < particles.size(); i++){
            particles[i]->update(deltaTime);

            if (particles[i]->lifetime <= 0){
                delete particles[i];
                particles.erase(particles.begin() + i);
                i--;
            }else{
                positions[i*3] = particles[i]->position.x;
                positions[(i*3)+1] = particles[i]->position.y;
                positions[(i*3)+2] = particles[i]->position.z;

                colors[i*3] = 1.0;
                colors[(i*3)+1] = (200-particles[i]->lifetime)/255.0f;
                colors[(i*3)+2] = 0;
            }
        }*/

        GLint color_location = glGetAttribLocation(program_id,"color");TEST_OPENGL_ERROR();

        //update the vertex buffer
        glBindVertexArray(vao);TEST_OPENGL_ERROR();
        glBindBuffer(GL_ARRAY_BUFFER, vbo);TEST_OPENGL_ERROR();
        glBufferData(GL_ARRAY_BUFFER, NUMBEROFPARTICLES*3*sizeof(float), &positions[0], GL_DYNAMIC_DRAW);TEST_OPENGL_ERROR();
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0 );TEST_OPENGL_ERROR();

        //update the color buffer
        /*glBindBuffer(GL_ARRAY_BUFFER, color_vbo);TEST_OPENGL_ERROR();
        glBufferData(GL_ARRAY_BUFFER, NUMBEROFPARTICLES*3*sizeof(float), &colors[0], GL_DYNAMIC_DRAW);TEST_OPENGL_ERROR();
        glVertexAttribPointer(color_location, 3, GL_FLOAT, GL_FALSE, 0, 0 );TEST_OPENGL_ERROR();*/

        glBindVertexArray(0);TEST_OPENGL_ERROR();
    }

   void draw(){
       GLuint color_location = glGetUniformLocation(program_id, "color");TEST_OPENGL_ERROR();
       glUniform3f(color_location, 1.0,0.0,0.0);
       //disable the depth mask in order to stop depth writes and thus blending artifacts while keeping the flame from drawing over other objects
       //glDepthMask(false);
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
