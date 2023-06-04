#include "program.hh"
#include "Transformations.hh"
#include "Camera.hh"
#include "ObjLoader.hh"
#include "Particle.hh"

GLuint vao_id;
GLuint program_id;
Camera camera;
ObjLoader object;
Vector2 mouse = {-1,-1};
Vector3 lightPosition = {2,2,2};
matrix4 MVP;
int resolution = 800;

matrix4 getWorldToProjMatrix(){

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

void window_resize(int width, int height) {
    //std::cout << "glViewport(0,0,"<< width << "," << height << ");TEST_OPENGL_ERROR();" << std::endl;
    glViewport(0,0,width,height);TEST_OPENGL_ERROR();

    //change camera settings to adapt new window size
    camera.setRatio(width, height);
    GLint worldToProj_matrix_location = glGetUniformLocation(program_id,"worldToProj_matrix");TEST_OPENGL_ERROR();
    matrix4 worldToProj_matrix = getWorldToProjMatrix();

    glUniformMatrix4fv(worldToProj_matrix_location, 1, GL_FALSE, &(worldToProj_matrix.m[0][0]));TEST_OPENGL_ERROR();
    glutPostRedisplay();
}

void idleFunc(){
    GLint lightPosition_location = glGetUniformLocation(program_id,"lightPosition");TEST_OPENGL_ERROR();
    GLint worldToProj_matrix_location = glGetUniformLocation(program_id,"worldToProj_matrix");TEST_OPENGL_ERROR();
    matrix4 lightRotation = matrix4::rotationY(0.01);
    matrix4 objectRotation = matrix4::rotationZ(0.0001);
    //MVP *= objectRotation;
    //MVP *= lightRotation;

    lightPosition = lightRotation * lightPosition;
    //std::cout << '(' << lightPosition[0] << "," << lightPosition[1] << "," << lightPosition[2] << ")" << std::endl;
    glUniform3f(lightPosition_location, lightPosition.x, lightPosition.y, lightPosition.z);
    glUniformMatrix4fv(worldToProj_matrix_location, 1, GL_FALSE, &(MVP.m[0][0]));TEST_OPENGL_ERROR();
    glutPostRedisplay();
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
    glutPostRedisplay();
}

/*void specialKeyboardFunc(int key, int x, int y){
    GLint worldToProj_matrix_location = glGetUniformLocation(program_id,"worldToProj_matrix");TEST_OPENGL_ERROR();
    float step = 1;
    if (key == GLUT_KEY_LEFT)
        camera.pos[0] -= step;
    if (key == GLUT_KEY_RIGHT)
        camera.pos[0] += step;
    if (key == GLUT_KEY_UP)
        camera.pos[1] += step;
    if (key == GLUT_KEY_DOWN)
        camera.pos[1] -= step;

    MVP = getWorldToProjMatrix();

    glUniformMatrix4fv(worldToProj_matrix_location, 1, GL_FALSE, &(MVP.m[0][0]));TEST_OPENGL_ERROR();
    glutPostRedisplay();
}*/

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
    glutPostRedisplay();

}

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);TEST_OPENGL_ERROR();
    glBindVertexArray(vao_id);
    TEST_OPENGL_ERROR();
    glDrawArrays(GL_TRIANGLES, 0, object.vertex_buffer.size());
    TEST_OPENGL_ERROR();
    glBindVertexArray(0);TEST_OPENGL_ERROR();
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
    //glutSpecialFunc(specialKeyboardFunc);
    glutIdleFunc(idleFunc);
    glutPassiveMotionFunc(mouseFunc);
    glutSetCursor(GLUT_CURSOR_NONE);
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
    GLint color_location = glGetUniformLocation(program_id,"vColor");TEST_OPENGL_ERROR();
    GLint worldToProj_matrix_location = glGetUniformLocation(program_id,"worldToProj_matrix");TEST_OPENGL_ERROR();
    GLint cameraPos_location = glGetUniformLocation(program_id,"cameraPos");TEST_OPENGL_ERROR();
    GLint lightPosition_location = glGetUniformLocation(program_id, "lightPosition");
    GLint lightColor_location = glGetUniformLocation(program_id, "lightColor");
    GLint lightIntensity_location = glGetUniformLocation(program_id, "lightIntensity");

    //fix the uniforms
    glUniform3f(color_location, 0.5, 0.5, 0.0);TEST_OPENGL_ERROR();
    glUniform3f(cameraPos_location, camera.pos.x, camera.pos.y, camera.pos.z);TEST_OPENGL_ERROR();
    glUniform3f(lightPosition_location, lightPosition.x, lightPosition.y, lightPosition.z);
    glUniform3f(lightColor_location, 2,2,2);
    glUniform1f(lightIntensity_location, 2);

    MVP = getWorldToProjMatrix();

    glUniformMatrix4fv(worldToProj_matrix_location, 1, GL_FALSE, &(MVP.m[0][0]));TEST_OPENGL_ERROR();
}

void init_VBO(){

    //get vertex location
    GLint vertex_location = glGetAttribLocation(program_id,"vPosition");TEST_OPENGL_ERROR();
    GLint normal_location = glGetAttribLocation(program_id, "vNormal");TEST_OPENGL_ERROR();

    object.load("assets/scene.obj");

    for (int i = 0; i < object.nbObjects; i++) {
        // Create and bind the Vertex Array Object (VAO)
        glGenVertexArrays(1, &vao_id);
        TEST_OPENGL_ERROR();
        glBindVertexArray(vao_id);
        TEST_OPENGL_ERROR();

        // Create the Vertex Buffer Object (VBO) and copy the vertex data to it
        GLuint vertex_VBO;
        glGenBuffers(1, &vertex_VBO);
        TEST_OPENGL_ERROR();
        glBindBuffer(GL_ARRAY_BUFFER, vertex_VBO);
        TEST_OPENGL_ERROR();
        glBufferData(GL_ARRAY_BUFFER, object.vertex_buffer.size() * sizeof(float), object.vertex_buffer.data(),
                     GL_STATIC_DRAW);
        TEST_OPENGL_ERROR();
        glVertexAttribPointer(vertex_location, 3, GL_FLOAT, GL_FALSE, 0, 0);
        TEST_OPENGL_ERROR();
        glEnableVertexAttribArray(vertex_location);
        TEST_OPENGL_ERROR();

        GLuint normal_VBO;
        glGenBuffers(1, &normal_VBO);
        TEST_OPENGL_ERROR();
        glBindBuffer(GL_ARRAY_BUFFER, normal_VBO);
        TEST_OPENGL_ERROR();
        glBufferData(GL_ARRAY_BUFFER, object.normal_buffer.size() * sizeof(float), object.normal_buffer.data(),
                     GL_STATIC_DRAW);
        TEST_OPENGL_ERROR();
        glVertexAttribPointer(normal_location, 3, GL_FLOAT, GL_FALSE, 0, 0);
        TEST_OPENGL_ERROR();
        glEnableVertexAttribArray(normal_location);
        TEST_OPENGL_ERROR();
    }


    /*glBindVertexArray(vao_id);TEST_OPENGL_ERROR();
    glBindBuffer(GL_ARRAY_BUFFER, normal_VBO);TEST_OPENGL_ERROR();*/


}

int main(int argc, char *argv[]){

    initGlut(argc, argv);
    initGlew();
    init_gl();

    std::string vertex_shader_src = "assets/vertex.shd";
    std::string fragment_shader_src = "assets/fragment.shd";
    auto my_program = program::make_program(vertex_shader_src, fragment_shader_src);
    my_program->use();
    program_id = my_program->program_id;

    fixUniforms();
    init_VBO();

    glutMainLoop();
    return 0;
}