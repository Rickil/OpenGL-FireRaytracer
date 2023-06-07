#ifndef TP1_PARTICULE_HH
#define TP1_PARTICULE_HH

//#include "Vector3.hh"
#include <glm/glm.hpp>

class Particle {
public:
    glm::vec3 position;
    glm::vec3 velocity;
    float lifetime;

    Particle(){
        //set initial lifetime of the particle to 200 frames
        lifetime = 200;

        //set random initial position for particle between -1 and 1
        float x = -5 + rand()/RAND_MAX*10;
        float y = 0 + rand()/RAND_MAX*2;
        float z = -5 + rand()/RAND_MAX*10;
        position = glm::vec3(x,y,z);
    }

    void update(unsigned int deltaTime){

        //create acceleration vector
        glm::vec3 acceleration;
        //acceleration.y -= 0.6;
        acceleration.x += rand()/RAND_MAX;
        acceleration.y -= rand()/RAND_MAX;
        acceleration.z += rand()/RAND_MAX;
        acceleration = glm::normalize(acceleration);
        acceleration *= 0.1;

        //lifetime of the particle decreased
        lifetime--;

        //accelerate the particle
        //velocity += acceleration /** ((float)deltaTime / 1000.0f)*/;
        //move the particle
        position += acceleration /** ((float)deltaTime / 1000.0f)*/;
    }

};


#endif //TP1_PARTICULE_HH
