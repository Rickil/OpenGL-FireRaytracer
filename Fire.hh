#ifndef POGL_FIRE_HH
#define POGL_FIRE_HH

#include "Particle.hh"
#include <vector>

class Fire {
public:
    std::vector<Particle> particles;
    std::vector<float> vertex_buffer;

    void updateVertexBuffer(){
        for (Particle particle : particles){
            std::vector<float> particle_vertex_buffer = Particle::verticesFromParticle(particle);
            vertex_buffer.insert(vertex_buffer.end(), particle_vertex_buffer.begin(), particle_vertex_buffer.end());
        }
    }

    Fire(unsigned int nbParticles, Vector3 color){
        for (unsigned int i = 0; i < nbParticles; i++){
            particles.emplace_back(color);
        }
    }

    void update(){
        updateVertexBuffer();
    }
};


#endif //POGL_FIRE_HH
