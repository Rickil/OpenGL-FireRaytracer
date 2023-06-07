#ifndef TP1_CAMERA_HH
#define TP1_CAMERA_HH

#include "Vector3.hh"

class Camera {
public:
    Vector3 pos = {10,10,10};
    Vector3 target = {0,0,0};
    Vector3 up = {0,1,0};
    float left = -1;
    float right = 1;
    float bottom = -1;
    float top = 1;
    float z_near = 5;
    float z_far = 9999;
    float yaw = 225;
    float pitch = -35;

    void setRatio(int width, int height){
        float ratio = (float)width/height;
        if (width < height){
            top = ratio;
            bottom = -ratio;
            right = 1;
            left = -1;
        }else if (width > height){
            top = 1;
            bottom = -1;
            right = ratio;
            left = -ratio;
        }else{
            top = 1;
            bottom = -1;
            right = 1;
            left = -1;
        }
    }
};


#endif //TP1_CAMERA_HH
