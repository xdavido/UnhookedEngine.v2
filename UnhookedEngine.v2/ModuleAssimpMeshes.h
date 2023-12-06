#pragma once
#pragma once
#include "Module.h"
#include "Globals.h"
#include <vector>
#include "Assimp/include/cimport.h"
#include "Assimp/include/scene.h"
#include "Assimp/include/postprocess.h"
#include"Glew/include/glew.h"
#include "GameObject.h"
#include "MathGeoLib/include/MathGeoLib.h"
#pragma comment (lib, "Assimp/libx86/assimp.lib")
#include "MathGeoLib/include/MathGeoLib.h"

#include <string>


#define VERTEX 5

struct Mesh {
    uint indexID = 0;
    uint indexCount = 0;
    uint* index = nullptr;

    uint vertexID = 0;
    uint vertexCount = 0;
    float* vertex = nullptr;

    float* vertexNormalFaces = nullptr;

    aiVector3D* vertexNormals = nullptr; 

    GameObject* owner;

    AABB localAABB;
    AABB aabb;
    OBB obb;

    uint VAO;
    uint VBO;
    uint EBO;

    GLuint id_texture;
    GLuint texture_width = 0;
    GLuint texture_height = 0;

    void InnitAABB();

    void Render();

    void RenderFaceNormals();

    void RenderVertexNormals();
};

class ModuleAssimpMeshes : public Module
{
public:

    ModuleAssimpMeshes(Application* app, bool start_enabled = true);
    ~ModuleAssimpMeshes();

    bool Init() override;
    bool Start() override;
    bool CleanUp() override;
    GameObject* LoadMeshFromFile(const char* file_path);
    void ImportAssimpMesh(aiMesh* aiMesh, GameObject* PgameObject, GameObject* CgameObject, const aiScene* scene, int index);
    void BufferMesh(Mesh* mesh);
    void RenderScene();

    void DeleteMesh(Mesh* mesh);
    
    
public:
    std::vector<Mesh*> meshes;
    const char* file_path;
    bool MeshDebug;
};
