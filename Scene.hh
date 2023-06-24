
#ifndef POGL_SCENE_HH
#define POGL_SCENE_HH

#include "ObjLoader.hh"
#include "Fire.hh"

class Scene {
public:
    ObjLoader* objLoader = nullptr;
    Fire* fire = nullptr;
    program* fire_program;
    program* objects_program;

    Scene(program* fire_program, program* objects_program){
        this->fire_program = fire_program;
        this->objects_program = objects_program;

        if (fire_program)
            fire = new Fire(fire_program);
        if (objects_program)
            objLoader = new ObjLoader(objects_program);
    }

    void loadScene(const char* path){
        objLoader->load(path);
    }

    void update(int deltaTime){
        if (fire) {
            fire->deltaTime = deltaTime;
            fire->update();
        }
    }

    void draw(){
        if (objLoader)
            objLoader->draw();
        if (fire)
            fire->draw();
    }

};


#endif //POGL_SCENE_HH
