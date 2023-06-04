#ifndef POGL_FIRE_HH
#define POGL_FIRE_HH

#include "Particle.hh"
#include "ObjLoader.hh"
#include <vector>
#include <cstdlib>


class Fire {
public:
    std::vector<Particle> particles;

    void update(GLuint program_id){

        //update particles buffers
        for (Particle particle : particles){
            particle.updateVAO(program_id);
            particle.draw();
        }
    }


    Fire(unsigned int nbParticles, Vector3 color){
        for (unsigned int i = 0; i < nbParticles; i++){
            Particle particle(color);
            particles.push_back(particle);
        }
    }
};


#endif //POGL_FIRE_HH
