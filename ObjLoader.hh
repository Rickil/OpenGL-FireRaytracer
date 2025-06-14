#ifndef TP1_OBJLOADER_HH
#define TP1_OBJLOADER_HH

#include "Vector3.hh"
#include "Vector2.hh"
#include "Fire.hh"
#include "Material.hh"
#include "Octree.hh"
#include <vector>
#include <string>
#include <stdexcept>
#include <cstring>

class ObjLoader {
public:
    std::vector<float> vertex_buffer;
    std::vector<float> smart_vertex_buffer;
    std::vector<float> surfaceInfo;
    std::vector<float> normal_buffer;
    std::vector<float> uv_buffer;

    GLuint vao;
    GLuint vertex_vbo;
    GLuint vertex_ssbo_vbo;
    GLuint normal_vbo;
    GLuint uv_vbo;
    GLuint texture_vbo;
    GLuint light_ssbo_vbo;

    program* program;
    Camera* camera;
    Fire* fire;

    std::vector<Material*> materials;

    void updateLightUniform(){
        GLuint numLights_location = glGetUniformLocation(program->program_id, "numLights");TEST_OPENGL_ERROR();
        glUniform1i(numLights_location, fire->particles.size());

        //fill lights ssbo
        std::vector<float> lightPositions;
        for (auto particle : fire->particles){
            lightPositions.push_back(particle->position.x);
            lightPositions.push_back(particle->position.y);
            lightPositions.push_back(particle->position.z);
        }

        glBindBuffer(GL_SHADER_STORAGE_BUFFER, light_ssbo_vbo);
        glBufferData(GL_SHADER_STORAGE_BUFFER, lightPositions.size() * sizeof(float), lightPositions.data(), GL_DYNAMIC_DRAW);
        GLuint bindingPoint = 1;  // Choose a binding point (0 or higher)
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, bindingPoint, light_ssbo_vbo);
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
        glGenBuffers(1, &normal_vbo);TEST_OPENGL_ERROR();
        glGenBuffers(1, &uv_vbo);TEST_OPENGL_ERROR();
        glGenBuffers(1, &vertex_ssbo_vbo);TEST_OPENGL_ERROR();
        glGenBuffers(1, &light_ssbo_vbo);TEST_OPENGL_ERROR();

        //init uniforms
        GLuint color_location = glGetUniformLocation(program->program_id, "vColor");TEST_OPENGL_ERROR();
        GLuint lightColor_location = glGetUniformLocation(program->program_id, "lightColor");TEST_OPENGL_ERROR();
        GLuint lightIntensity_location = glGetUniformLocation(program->program_id, "lightIntensity");TEST_OPENGL_ERROR();

        glUniform3f(color_location, 1.0,1.0,0.0);
        glUniform3f(lightColor_location, 1.0,0.63,0.0);
        glUniform1f(lightIntensity_location, 0.001);

        updateLightUniform();
    }


    void load(const char* path){
        //load materials
        // Generate the texture array.
        materials = Material::loadMaterials(path);
        Material* currentMaterial;

        //init buffers
        std::vector<unsigned int> vertexIndices;
        std::vector<unsigned int> normalIndices;
        std::vector<unsigned int> uvIndices;
        std::vector<Vector3> temp_vertex_buffer;
        std::vector<Vector3> temp_normal_buffer;
        std::vector<Vector3> temp_uv_buffer;

        //open the file
        FILE * file = std::fopen(path, "r");
        if( file == NULL ){
            throw std::invalid_argument("Impossible to open the file !\n");
        }

        int nbVertices = 0;

        //parse the file
        while( 1 ) {
            char lineHeader[128];
            // read the first word of the line
            int res = fscanf(file, "%s", lineHeader);
            if (res == EOF)
                break;
            if (strcmp( lineHeader, "o" ) == 0){
                /*char objectName[128];
                fscanf(file, "%s", objectName);
                if ( strcmp( objectName, "rug_low" ) == 0 ){
                    int a = 5;
                }*/
                nbVertices = 0;
            }
            else if ( strcmp( lineHeader, "v" ) == 0 ){
                Vector3 vertex;
                fscanf(file, " %f %f %f\n", &vertex.x, &vertex.y, &vertex.z );
                temp_vertex_buffer.push_back(vertex);
            }
            else if ( strcmp( lineHeader, "vt" ) == 0 ) {
                Vector3 uv;
                fscanf(file, "%f %f\n", &uv.x, &uv.y);
                temp_uv_buffer.push_back(uv);
                nbVertices++;
            }else if ( strcmp( lineHeader, "vn" ) == 0 ) {
                Vector3 normal;
                fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z);
                temp_normal_buffer.push_back(normal);
            }else if ( strcmp( lineHeader, "usemtl" ) == 0 ){
                // Read the material name
                char materialName[128];
                fscanf(file, "%s", materialName);

                // Find the corresponding Material in the vector
                for (int i = 0; i < materials.size(); i++) {
                    if (materials[i]->name.compare(materialName) == 0) {
                        currentMaterial = materials[i];
                        for (int j = temp_uv_buffer.size()-nbVertices; j < temp_uv_buffer.size(); j++){
                            if (materials[i]->kdMap)
                                temp_uv_buffer[j].z = i;
                            else
                                temp_uv_buffer[j].z = -1;
                        }
                        break;
                    }
                }
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
                /*surfaceInfo.push_back(currentMaterial->kd);
                surfaceInfo.push_back(currentMaterial->ks);
                surfaceInfo.push_back(currentMaterial->Ns);*/
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
            Vector3 uv = temp_uv_buffer[uvIndex-1];
            uv_buffer.push_back(uv.x);
            uv_buffer.push_back(uv.y);
            uv_buffer.push_back(uv.z);

            //fill normal buffer
            unsigned int normalIndex = normalIndices[i];
            Vector3 normal = temp_normal_buffer[normalIndex-1];
            normal_buffer.push_back(normal.x);
            normal_buffer.push_back(normal.y);
            normal_buffer.push_back(normal.z);
        }

        //program->use();
        //fill buffers
        program->use();
        glBindVertexArray(vao);TEST_OPENGL_ERROR();
        GLint vertex_location = glGetAttribLocation(program->program_id,"vPosition");TEST_OPENGL_ERROR();
        GLint normal_location = glGetAttribLocation(program->program_id,"vNormal");TEST_OPENGL_ERROR();
        GLint uv_location = glGetAttribLocation(program->program_id,"uv");TEST_OPENGL_ERROR();
        if (!vertex_buffer.empty()) {
            glBindBuffer(GL_ARRAY_BUFFER, vertex_vbo);
            TEST_OPENGL_ERROR();
            glBufferData(GL_ARRAY_BUFFER, vertex_buffer.size() * sizeof(float), vertex_buffer.data(), GL_STATIC_DRAW);
            TEST_OPENGL_ERROR();
            glVertexAttribPointer(vertex_location, 3, GL_FLOAT, GL_FALSE, 0, 0 );TEST_OPENGL_ERROR();
            glEnableVertexAttribArray(vertex_location);TEST_OPENGL_ERROR();

            //init smart vertex buffer
            smart_vertex_buffer.reserve(vertex_buffer.size()*3);
            Octree octree = Octree(300);
            octree.build(vertex_buffer, uv_buffer);
            octree.fillSmart(octree.root, smart_vertex_buffer);
            /*KDTree kdTree = KDTree(300, vertex_buffer, uv_buffer);
            kdTree.fillSmart(kdTree.root, smart_vertex_buffer);*/

            //init and enable vertex ssbo
            glBindBuffer(GL_SHADER_STORAGE_BUFFER, vertex_ssbo_vbo);
            glBufferData(GL_SHADER_STORAGE_BUFFER, smart_vertex_buffer.size() * sizeof(float), smart_vertex_buffer.data(), GL_STATIC_DRAW);
            GLuint bindingPoint = 0;  // Choose a binding point (0 or higher)
            glBindBufferBase(GL_SHADER_STORAGE_BUFFER, bindingPoint, vertex_ssbo_vbo);

            GLuint nbVertices_location = glGetUniformLocation(program->program_id, "dataSize");TEST_OPENGL_ERROR();
            glUniform1ui(nbVertices_location, smart_vertex_buffer.size());TEST_OPENGL_ERROR();

            //octree.destroyTree(octree.root);
            //kdTree.DestroyTree(kdTree.root);

        }if (!normal_buffer.empty()) {
            glBindBuffer(GL_ARRAY_BUFFER, normal_vbo);
            TEST_OPENGL_ERROR();
            glBufferData(GL_ARRAY_BUFFER, normal_buffer.size() * sizeof(float), normal_buffer.data(), GL_STATIC_DRAW);
            TEST_OPENGL_ERROR();
            glVertexAttribPointer(normal_location, 3, GL_FLOAT, GL_FALSE, 0, 0 );TEST_OPENGL_ERROR();
            glEnableVertexAttribArray(normal_location);TEST_OPENGL_ERROR();
        }if (!uv_buffer.empty()){
            glBindBuffer(GL_ARRAY_BUFFER, uv_vbo);TEST_OPENGL_ERROR();
            glBufferData(GL_ARRAY_BUFFER, uv_buffer.size() * sizeof(float), uv_buffer.data(), GL_STATIC_DRAW);
            TEST_OPENGL_ERROR();
            glVertexAttribPointer(uv_location, 3, GL_FLOAT, GL_FALSE, 0, 0 );TEST_OPENGL_ERROR();
            glEnableVertexAttribArray(uv_location);TEST_OPENGL_ERROR();

            // Load and store textures
            GLsizei width = 0;
            GLsizei height = 0;
            size_t layerCount = 10;

            GLint textureArrayLoc = glGetUniformLocation(program->program_id, "textureArray");
            glUniform1i(textureArrayLoc, 0); // 0 refers to texture unit 0
            //glActiveTexture(GL_TEXTURE0);TEST_OPENGL_ERROR();
            glGenTextures(1,&texture_vbo);TEST_OPENGL_ERROR();
            glBindTexture(GL_TEXTURE_2D_ARRAY,texture_vbo);TEST_OPENGL_ERROR();

            // Always set reasonable texture parameters
            glTexParameteri(GL_TEXTURE_2D_ARRAY,GL_TEXTURE_MIN_FILTER,GL_LINEAR);TEST_OPENGL_ERROR();
            glTexParameteri(GL_TEXTURE_2D_ARRAY,GL_TEXTURE_MAG_FILTER,GL_LINEAR);TEST_OPENGL_ERROR();
            glTexParameteri(GL_TEXTURE_2D_ARRAY,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);TEST_OPENGL_ERROR();
            glTexParameteri(GL_TEXTURE_2D_ARRAY,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);TEST_OPENGL_ERROR();

            for (size_t i = 0; i<materials.size(); i++)
            {
                Tga* tgaImage = materials[i]->kdMap;
                if (tgaImage)
                {
                    const std::vector<std::uint8_t>& pixels = tgaImage->GetRGBA();

                    if (width == 0 && height == 0)
                    {
                        width = tgaImage->GetWidth();
                        height = tgaImage->GetHeight();
                        // Allocate the storage.
                        glTexStorage3D(GL_TEXTURE_2D_ARRAY, 1, GL_RGBA8, width, height, layerCount);TEST_OPENGL_ERROR();
                    }

                    glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, i, width, height, 1, GL_RGBA, GL_UNSIGNED_BYTE, pixels.data());TEST_OPENGL_ERROR();
                    std::cout << "loaded texture " << i << "\n";
                }
            }

            std::cout << "textures loaded!\n";

            glGenerateMipmap(GL_TEXTURE_2D_ARRAY);TEST_OPENGL_ERROR();

            glBindTexture(GL_TEXTURE_2D_ARRAY, 0);TEST_OPENGL_ERROR();

        }
        glBindVertexArray(0);TEST_OPENGL_ERROR();
    }

    void draw(){
        program->use();
        /*if (vertex_reflected_buffer.empty())
            updateReflectionBuffer();*/
        updateLightUniform();
        if (!vertex_buffer.empty()){
            //bind the VAO and draw it
            glBindVertexArray(vao);TEST_OPENGL_ERROR();
            glEnableVertexAttribArray(0);TEST_OPENGL_ERROR();

            // Activate the texture unit and bind the texture array.
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D_ARRAY, texture_vbo);TEST_OPENGL_ERROR();

            glDrawArrays(GL_TRIANGLES, 0, vertex_buffer.size());TEST_OPENGL_ERROR();
            //unbind appropriate data
            glDisableVertexAttribArray(0);TEST_OPENGL_ERROR();
            glBindTexture(GL_TEXTURE_2D_ARRAY, 0);TEST_OPENGL_ERROR();
            glBindVertexArray(0);TEST_OPENGL_ERROR();
        }
    }
};


#endif //TP1_OBJLOADER_HH
