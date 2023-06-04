#ifndef POGL_FIRE_HH
#define POGL_FIRE_HH

#include "Particle.hh"
#include <vector>

class Fire {
public:
    std::vector<Particle> particles;

    Fire(unsigned int nbParticles, Vector3 color){
        for (unsigned int i = 0; i < nbParticles; i++){
            particles.emplace_back(color);
        }
    }
};


#endif //POGL_FIRE_HH
