
#ifndef POGL_SCENE_HH
#define POGL_SCENE_HH

#include "ObjLoader.hh"
#include "Fire.hh"

class Scene {
public:
    //ObjLoader* objLoader;
    Fire* fire;
    GLuint program_id;

    Scene(GLuint program_id){
        this->program_id = program_id;
        fire = new Fire(program_id);
        //objLoader = new ObjLoader(program_id);
    }

    void loadScene(const char* path){
        //objLoader->load(path);
    }

    void update(int deltaTime){
        fire->deltaTime = deltaTime;
        fire->update();
    }

    void draw(){
        fire->draw();
        //objLoader->draw();
    }

};


#endif //POGL_SCENE_HH
