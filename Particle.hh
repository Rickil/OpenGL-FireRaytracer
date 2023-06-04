#ifndef TP1_PARTICULE_HH
#define TP1_PARTICULE_HH

#include "Vector3.hh"

class Particle {
public:
    Vector3 position;
    Vector3 velocity;
    Vector3 color;
    float life;

    explicit Particle(const Vector3 &color) : color(color) {}
    static std::vector<float> verticesFromParticle(Particle particle){

        std::vector<float> vertex_buffer = {
                -0.5f, -0.5f, 0.0f,
                0.5f, -0.5f, 0.0f,
                -0.5f, 0.5f, 0.0f,
                0.5f, 0.5f, 0.0f,
        };
        Vector3::translateVerticesOnVector(vertex_buffer, particle.position);
        return vertex_buffer;
    }
};


#endif //TP1_PARTICULE_HH
