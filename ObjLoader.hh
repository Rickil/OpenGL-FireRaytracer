#ifndef TP1_OBJLOADER_HH
#define TP1_OBJLOADER_HH

#include "Vector3.hh"
#include "Vector2.hh"
#include "Fire.hh"
#include <vector>
#include <string>
#include <stdexcept>
#include <cstring>

class ObjLoader {
public:
    std::vector<float> vertex_buffer;
    std::vector<float> vertex_reflected_buffer;
    std::vector<float> normal_buffer;
    std::vector<float> uv_buffer;

    GLuint vao;
    GLuint vertex_vbo;
    GLuint vertex_reflected_vbo;
    GLuint normal_vbo;
    GLuint uv_vbo;

    program* program;
    Camera* camera;
    Fire* fire;

    //make isInsideTriangle function
    bool isInsideTriangle(Vector3 point, Vector3 v1, Vector3 v2, Vector3 v3){
        Vector3 v1v2 = v2 - v1;
        Vector3 v2v3 = v3 - v2;
        Vector3 v3v1 = v1 - v3;
        Vector3 pointv1 = point - v1;
        Vector3 pointv2 = point - v2;
        Vector3 pointv3 = point - v3;
        if (v1v2.cross(pointv1).dot(v1v2.cross(v2v3)) > 0 &&
            v2v3.cross(pointv2).dot(v2v3.cross(v3v1)) > 0 &&
            v3v1.cross(pointv3).dot(v3v1.cross(v1v2)) > 0){
            return true;
        }
        return false;
    }

    //make the checkCollisionTriangle function
    Vector3 checkCollisionTriangle(Vector3 v1, Vector3 v2, Vector3 v3, Vector3 reflectedDirection){
        Vector3 collisionPoint;
        Vector3 normal = (v2 - v1).cross(v3 - v1).normalize();
        float t = (v1 - camera->pos).dot(normal) / reflectedDirection.dot(normal);
        if (t > 0){
            collisionPoint = camera->pos + reflectedDirection * t;
            if (isInsideTriangle(collisionPoint, v1, v2, v3)){
                return collisionPoint;
            }
        }
        return {0,0,0};
    }


    //for all the vertex in vertex_buffer, calculate reflection and check a collision
    // considering that vertex_buffer make triangles
    Vector3 checkCollision(Vector3 vertex, Vector3 normal){
        Vector3 cameraToVertex = vertex - camera->pos;
        Vector3 reflectedDirection = cameraToVertex - normal * 2 * cameraToVertex.dot(normal);
        reflectedDirection = reflectedDirection.normalize();
        Vector3 reflected_position;

        for (int i = 0; i < vertex_buffer.size(); i+=9){
            Vector3 v1 = {vertex_buffer[i], vertex_buffer[i+1], vertex_buffer[i+2]};
            Vector3 v2 = {vertex_buffer[i+3], vertex_buffer[i+4], vertex_buffer[i+5]};
            Vector3 v3 = {vertex_buffer[i+6], vertex_buffer[i+7], vertex_buffer[i+8]};

            //check collision with the triangle made by v1, v2, v3 and the reflected direction vector
            //and get collision point
            Vector3 collisionPoint = checkCollisionTriangle(v1, v2, v3, reflectedDirection);
            if (collisionPoint.magnitude() > 0 && (reflected_position.magnitude() == 0 || (collisionPoint - vertex).magnitude() < (reflected_position - vertex).magnitude())){
                reflected_position = collisionPoint;
            }
        }
        return reflected_position;
    }

    void updateReflectionBuffer(){
        vertex_reflected_buffer.clear();
        for (int i = 0; i < vertex_buffer.size(); i+=3){
            Vector3 vertex = {vertex_buffer[i], vertex_buffer[i+1], vertex_buffer[i+2]};
            Vector3 normal = {normal_buffer[i], normal_buffer[i+1], normal_buffer[i+2]};
            Vector3 reflected_position = checkCollision(vertex, normal);
            if (reflected_position.magnitude() > 0){
                vertex_reflected_buffer.push_back(reflected_position.x);
                vertex_reflected_buffer.push_back(reflected_position.y);
                vertex_reflected_buffer.push_back(reflected_position.z);
            }else{
                vertex_reflected_buffer.push_back(0);
                vertex_reflected_buffer.push_back(0);
                vertex_reflected_buffer.push_back(0);
            }
        }

        glBindBuffer(GL_ARRAY_BUFFER, vertex_reflected_vbo);TEST_OPENGL_ERROR();
        glBufferData(GL_ARRAY_BUFFER, vertex_reflected_buffer.size() * sizeof(float), vertex_reflected_buffer.data(), GL_DYNAMIC_DRAW);TEST_OPENGL_ERROR();
    }

    void updateLightUniform(){
        GLuint lightPositions_location = glGetUniformLocation(program->program_id, "lightPositions");TEST_OPENGL_ERROR();
        GLuint numLights_location = glGetUniformLocation(program->program_id, "numLights");TEST_OPENGL_ERROR();

        std::vector<float> lightPositions;
        for (auto particle : fire->particles){
            lightPositions.push_back(particle->position.x);
            lightPositions.push_back(particle->position.y);
            lightPositions.push_back(particle->position.z);
        }

        glUniform1i(numLights_location, fire->particles.size());
        glUniform3fv(lightPositions_location, fire->particles.size(), lightPositions.data());
    }

    ObjLoader(class program* program, Camera* camera, Fire* fire){
        this->program = program;
        this->camera = camera;
        this->fire = fire;
        program->use();
        //init vao id, vbo id, and initial buffers
        glGenVertexArrays(1, &vao);TEST_OPENGL_ERROR();
        glBindVertexArray(vao);TEST_OPENGL_ERROR();

        glGenBuffers(1, &vertex_vbo);TEST_OPENGL_ERROR();
        glGenBuffers(1, &vertex_reflected_vbo);TEST_OPENGL_ERROR();
        glGenBuffers(1, &normal_vbo);TEST_OPENGL_ERROR();
        glGenBuffers(1, &uv_vbo);TEST_OPENGL_ERROR();

        //init uniforms
        GLuint color_location = glGetUniformLocation(program->program_id, "vColor");TEST_OPENGL_ERROR();
        GLuint lightColor_location = glGetUniformLocation(program->program_id, "lightColor");TEST_OPENGL_ERROR();
        GLuint lightIntensity_location = glGetUniformLocation(program->program_id, "lightIntensity");TEST_OPENGL_ERROR();


        glUniform3f(color_location, 1.0,1.0,0.0);
        glUniform3f(lightColor_location, 1.0,0.63,0.0);
        glUniform1f(lightIntensity_location, 0.0005);

        updateLightUniform();
    }

    int nbObjects = 0;

    void load(const char* path){
        nbObjects++;

        //init buffers
        std::vector<unsigned int> vertexIndices;
        std::vector<unsigned int> normalIndices;
        std::vector<unsigned int> uvIndices;
        std::vector<Vector3> temp_vertex_buffer;
        std::vector<Vector3> temp_normal_buffer;
        std::vector<Vector2> temp_uv_buffer;

        //open the file
        FILE * file = std::fopen(path, "r");
        if( file == NULL ){
            throw std::invalid_argument("Impossible to open the file !\n");
        }

        //parse the file
        while( 1 ) {
            char lineHeader[128];
            // read the first word of the line
            int res = fscanf(file, "%s", lineHeader);
            if (res == EOF)
                break;
            if ( strcmp( lineHeader, "v" ) == 0 ){
                Vector3 vertex;
                fscanf(file, " %f %f %f\n", &vertex.x, &vertex.y, &vertex.z );
                temp_vertex_buffer.push_back(vertex);
            }
            else if ( strcmp( lineHeader, "vt" ) == 0 ) {
                Vector2 uv;
                fscanf(file, "%f %f\n", &uv.x, &uv.y);
                temp_uv_buffer.push_back(uv);
            }else if ( strcmp( lineHeader, "vn" ) == 0 ) {
                Vector3 normal;
                fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z);
                temp_normal_buffer.push_back(normal);
            }else if ( strcmp( lineHeader, "f" ) == 0 ) {
                unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
                int matches = fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &vertexIndex[0], &uvIndex[0],
                                     &normalIndex[0], &vertexIndex[1], &uvIndex[1], &normalIndex[1], &vertexIndex[2],
                                     &uvIndex[2], &normalIndex[2]);
                if (matches != 9) {
                    throw std::invalid_argument(
                            "File can't be read by our simple parser : ( Try exporting with other options\n");
                }

                //fill indices buffer
                vertexIndices.push_back(vertexIndex[0]);
                vertexIndices.push_back(vertexIndex[1]);
                vertexIndices.push_back(vertexIndex[2]);
                uvIndices.push_back(uvIndex[0]);
                uvIndices.push_back(uvIndex[1]);
                uvIndices.push_back(uvIndex[2]);
                normalIndices.push_back(normalIndex[0]);
                normalIndices.push_back(normalIndex[1]);
                normalIndices.push_back(normalIndex[2]);
            }
        }

        //fill buffers in the correct order
        for( unsigned int i=0; i<vertexIndices.size(); i++ ){
            //fill vertex_buffer
            unsigned int vertexIndex = vertexIndices[i];
            Vector3 vertex = temp_vertex_buffer[ vertexIndex-1 ];
            vertex_buffer.push_back(vertex.x);
            vertex_buffer.push_back(vertex.y);
            vertex_buffer.push_back(vertex.z);

            //fill uv buffer
            unsigned int uvIndex = uvIndices[i];
            Vector2 uv = temp_uv_buffer[uvIndex-1];
            uv_buffer.push_back(uv.x);
            uv_buffer.push_back(uv.y);

            //fill normal buffer
            unsigned int normalIndex = normalIndices[i];
            Vector3 normal = temp_normal_buffer[normalIndex-1];
            normal_buffer.push_back(normal.x);
            normal_buffer.push_back(normal.y);
            normal_buffer.push_back(normal.z);
        }

        program->use();
        //fill buffers
        glBindVertexArray(vao);TEST_OPENGL_ERROR();
        GLint vertex_location = glGetAttribLocation(program->program_id,"vPosition");TEST_OPENGL_ERROR();
        GLint normal_location = glGetAttribLocation(program->program_id,"vNormal");TEST_OPENGL_ERROR();
        if (!vertex_buffer.empty()) {
            glBindBuffer(GL_ARRAY_BUFFER, vertex_vbo);
            TEST_OPENGL_ERROR();
            glBufferData(GL_ARRAY_BUFFER, vertex_buffer.size() * sizeof(float), vertex_buffer.data(), GL_STATIC_DRAW);
            TEST_OPENGL_ERROR();
            glVertexAttribPointer(vertex_location, 3, GL_FLOAT, GL_FALSE, 0, 0 );TEST_OPENGL_ERROR();
            glEnableVertexAttribArray(vertex_location);TEST_OPENGL_ERROR();

            //enable vertex_reflected
            GLint vertex_reflected_location = glGetAttribLocation(program->program_id,"vReflected");TEST_OPENGL_ERROR();
            glBindBuffer(GL_ARRAY_BUFFER, vertex_reflected_vbo);TEST_OPENGL_ERROR();
            glVertexAttribPointer(vertex_reflected_location, 3, GL_FLOAT, GL_FALSE, 0, 0 );TEST_OPENGL_ERROR();
            glEnableVertexAttribArray(vertex_reflected_location);TEST_OPENGL_ERROR();
        }if (!normal_buffer.empty()) {
            glBindBuffer(GL_ARRAY_BUFFER, normal_vbo);
            TEST_OPENGL_ERROR();
            glBufferData(GL_ARRAY_BUFFER, normal_buffer.size() * sizeof(float), normal_buffer.data(), GL_STATIC_DRAW);
            TEST_OPENGL_ERROR();
            glVertexAttribPointer(normal_location, 3, GL_FLOAT, GL_FALSE, 0, 0 );TEST_OPENGL_ERROR();
            glEnableVertexAttribArray(normal_location);TEST_OPENGL_ERROR();
        }
        glBindVertexArray(0);TEST_OPENGL_ERROR();
    }

    void draw(){
        program->use();
        updateReflectionBuffer();
        updateLightUniform();
        if (!vertex_buffer.empty()){
            //bind the VAO and draw it
            glBindVertexArray(vao);TEST_OPENGL_ERROR();
            glEnableVertexAttribArray(0);TEST_OPENGL_ERROR();
            glDrawArrays(GL_TRIANGLES, 0, vertex_buffer.size());TEST_OPENGL_ERROR();
            //unbind appropriate data
            glDisableVertexAttribArray(0);TEST_OPENGL_ERROR();
            glBindVertexArray(0);TEST_OPENGL_ERROR();
        }
    }
};


#endif //TP1_OBJLOADER_HH
