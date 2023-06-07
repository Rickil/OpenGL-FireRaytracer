#include "program.hh"
#include "Transformations.hh"
#include "Camera.hh"
#include "ObjLoader.hh"
#include "Fire.hh"
#include "image_io.hh"

GLuint program_id;
Camera camera;
Vector2 mouse = {-1,-1};
matrix4 MVP;
int resolution = 800;
float FPS = 1000/60;
//Fire fire(10, Vector3(1,0.5,0.5));

/*matrix4 getWorldToProjMatrix(){

    // Create the matrices
    matrix4 view_matrix;
    look_at(view_matrix, camera.pos.x, camera.pos.y, camera.pos.z,
            camera.pos.x+camera.target.x, camera.pos.y+camera.target.y, camera.pos.z+camera.target.z,
            camera.up.x, camera.up.y, camera.up.z);

    matrix4 proj_matrix;
    frustum(proj_matrix, camera.left, camera.right, camera.bottom, camera.top, camera.z_near, camera.z_far);

    matrix4 worldToProj_matrix = view_matrix;
    worldToProj_matrix *= proj_matrix;

    return worldToProj_matrix;
}

void update(int value){
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);TEST_OPENGL_ERROR();
    fire.update(program_id);
    glutPostRedisplay();
    glutTimerFunc(FPS, update, value+1);
}

void window_resize(int width, int height) {
    //std::cout << "glViewport(0,0,"<< width << "," << height << ");TEST_OPENGL_ERROR();" << std::endl;
    glViewport(0,0,width,height);TEST_OPENGL_ERROR();

    //change camera settings to adapt new window size
    camera.setRatio(width, height);
    GLint worldToProj_matrix_location = glGetUniformLocation(program_id,"worldToProj_matrix");TEST_OPENGL_ERROR();
    matrix4 worldToProj_matrix = getWorldToProjMatrix();

    glUniformMatrix4fv(worldToProj_matrix_location, 1, GL_FALSE, &(worldToProj_matrix.m[0][0]));TEST_OPENGL_ERROR();
    //glutPostRedisplay();
}

void mouseFunc(int x, int y){
    //std::cout << x << ", " << y << std::endl;

    GLint worldToProj_matrix_location = glGetUniformLocation(program_id,"worldToProj_matrix");TEST_OPENGL_ERROR();

    if (mouse.x == -1){
        mouse.x = x;
        mouse.y = y;
    }

    float xOffset = x - mouse.x;
    float yOffset = mouse.y-y;
    mouse.x = x;
    mouse.y = y;

    float sensitivity = 0.1;
    xOffset*=sensitivity;
    yOffset*=sensitivity;

    camera.yaw+=xOffset;
    camera.pitch+=yOffset;

    if (camera.pitch > 89)
        camera.pitch = 89;
    if (camera.pitch < -89)
        camera.pitch = -89;

    Vector3 newTarget;
    newTarget.x = cos(rad(camera.yaw))*cos(rad(camera.pitch));
    newTarget.y = sin(rad(camera.pitch));
    newTarget.z = sin(rad(camera.yaw))*cos(rad(camera.pitch));
    newTarget.normalize();
    camera.target = newTarget;

    //recenter the cursor to avoid it going out of window
    int center = resolution/2;
    mouse.x = center;
    mouse.y = center;
    glutWarpPointer(center, center);

    MVP = getWorldToProjMatrix();

    glUniformMatrix4fv(worldToProj_matrix_location, 1, GL_FALSE, &(MVP.m[0][0]));TEST_OPENGL_ERROR();
    //glutPostRedisplay();
}

void keyboardFunc(unsigned char key, int x, int y){
    GLint worldToProj_matrix_location = glGetUniformLocation(program_id,"worldToProj_matrix");TEST_OPENGL_ERROR();
    float step = 0.1;
    if (key == 'd'){
        camera.pos += vectorialProduct(camera.target, camera.up).normalize()*step;
    }else if(key == 'q'){
        camera.pos += vectorialProduct(camera.up, camera.target).normalize()*step;
    }else if(key == 's'){
        camera.pos -= camera.target*step;
    }else if(key == 'z'){
        camera.pos += camera.target*step;
    }
    else if(key == 'a'){
        camera.pos.y += step;
    }
    else if(key == 'e'){
        camera.pos.y -= step;
    }

    MVP = getWorldToProjMatrix();

    glUniformMatrix4fv(worldToProj_matrix_location, 1, GL_FALSE, &(MVP.m[0][0]));TEST_OPENGL_ERROR();
    //glutPostRedisplay();

}

void display() {
    glutSwapBuffers();
}

bool initGlut(int &argc, char *argv[]){
    glutInit(& argc , argv ) ;
    glutInitContextVersion ( 4 , 5 ) ;
    glutInitContextProfile(GLUT_CORE_PROFILE | GLUT_DEBUG);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowSize(resolution, resolution);
    glutInitWindowPosition(500,10);
    glutCreateWindow("Test OpenGL _ POGL");
    glutDisplayFunc(display);
    glutReshapeFunc(window_resize);
    glutKeyboardFunc(keyboardFunc);
    glutPassiveMotionFunc(mouseFunc);
    glutSetCursor(GLUT_CURSOR_NONE);
    glutTimerFunc(FPS, update, 0);
    return true;
}

bool initGlew(){
    if (glewInit()) {
        std::cerr << " Error while initializing glew";
        return false;
    }
    return true;
}

bool init_gl(){
    glEnable(GL_DEPTH_TEST);TEST_OPENGL_ERROR();
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);TEST_OPENGL_ERROR();
    //glEnable(GL_CULL_FACE);TEST_OPENGL_ERROR();
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.4,0.4,0.4,1.0);TEST_OPENGL_ERROR();
    glPixelStorei(GL_UNPACK_ALIGNMENT,1);
    glPixelStorei(GL_PACK_ALIGNMENT,1);
    return true;
}

void fixUniforms(){
    //fix mouse
    Vector3 newTarget;
    newTarget.x = cos(rad(camera.yaw))*cos(rad(camera.pitch));
    newTarget.y = sin(rad(camera.pitch));
    newTarget.z = sin(rad(camera.yaw))*cos(rad(camera.pitch));
    camera.target = newTarget;

    //get the locations
    GLint worldToProj_matrix_location = glGetUniformLocation(program_id,"worldToProj_matrix");TEST_OPENGL_ERROR();

    //fix the uniforms
    MVP = getWorldToProjMatrix();

    glUniformMatrix4fv(worldToProj_matrix_location, 1, GL_FALSE, &(MVP.m[0][0]));TEST_OPENGL_ERROR();
}*/

int main(int argc, char *argv[]){

    /*float a = 2;

    initGlut(argc, argv);
    initGlew();
    init_gl();

    std::string vertex_shader_src = "assets/vertex.shd";
    std::string fragment_shader_src = "assets/fragment.shd";
    auto my_program = program::make_program(vertex_shader_src, fragment_shader_src);
    my_program->use();
    program_id = my_program->program_id;

    fixUniforms();
    fire.init();

    glutMainLoop();*/

    tifo::rgb24_image* image = tifo::load_image("assets/Flame_Particle.tga");

    return 0;
}