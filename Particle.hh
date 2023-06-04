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
};


#endif //TP1_PARTICULE_HH
