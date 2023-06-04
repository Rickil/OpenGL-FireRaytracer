#ifndef POGL_FIRE_HH
#define POGL_FIRE_HH

#include "Particle.hh"
#include <vector>

class Fire {
public:
    std::vector<Particle> particles;
    std::vector<float> vertex_buffer;
    std::vector<float> color_buffer;

    void updateBuffers(){
        vertex_buffer.clear();
        color_buffer.clear();

        for (Particle particle : particles){
            std::vector<float> particle_vertex_buffer = Particle::verticesFromParticle(particle);
            std::vector<float> particle_color_buffer = {particle.color.x, particle.color.y, particle.color.z};

            vertex_buffer.insert(vertex_buffer.end(), particle_vertex_buffer.begin(), particle_vertex_buffer.end());
            color_buffer.insert(color_buffer.end(), particle_color_buffer.begin(), particle_color_buffer.end());
        }
    }

    Fire(unsigned int nbParticles, Vector3 color){
        for (unsigned int i = 0; i < nbParticles; i++){
            particles.emplace_back(color);
        }
    }

    void update(){
        updateBuffers();
    }
};


#endif //POGL_FIRE_HH
