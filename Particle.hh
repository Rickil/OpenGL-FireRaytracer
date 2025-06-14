#ifndef TP1_PARTICULE_HH
#define TP1_PARTICULE_HH

#include "Vector3.hh"
//#include <glm/glm.hpp>

class Particle {
public:
    Vector3 position;
    Vector3 velocity;
    float lifetime;

    Particle(){
        //set initial lifetime of the particle to 200 frames
        float lifetimeOffset = -20 + rand()/RAND_MAX*40;
        lifetime = 200 + lifetimeOffset;

        //set random initial position for particle between -1 and 1
        position = Vector3((((float) rand() / (float) RAND_MAX) - 0.5)+0.5,0.5,(((float) rand() / (float) RAND_MAX) - 0.5));
    }

    void update(std::vector<Particle*> &particlesInRange, unsigned int deltaTime){


        Vector3 acceleration;
        //upwards acceleration due to convection added
        acceleration.y += 0.6f;
        //lifetime of the particle decreased
        lifetime--;
        //for each particle in range move towards or away from it depending on density
        for (unsigned int i = 0; i < particlesInRange.size(); i++)
        {
            if (particlesInRange[i] != this)
            {
                if (particlesInRange.size() > 20)
                {
                    acceleration += (position - particlesInRange[i]->position) * 0.02f;
                }
                else
                {
                    acceleration += (position - particlesInRange[i]->position) * -2.0f;
                }
            }
        }
        //accelerate the particle
        velocity += acceleration * ((float)deltaTime / 1000.0f);
        //move the particle
        position += velocity * ((float)deltaTime / 1000.0f);
    }

};


#endif //TP1_PARTICULE_HH
