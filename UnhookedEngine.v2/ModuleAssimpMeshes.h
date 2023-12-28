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
#include "MathGeoLib/include/Geometry/AABB.h"
#include "MathGeoLib/include/Geometry/OBB.h"
#pragma comment (lib, "Assimp/libx86/assimp.lib")

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

    GameObject* owner = nullptr;


    uint VAO;
    uint VBO;
    uint EBO;

    AABB _AABB;
    OBB _OBB;
    AABB GlobalAABB;

    GLuint id_texture;
    GLuint texture_width = 0;
    GLuint texture_height = 0;

    void Render();

    void InitAABB();

    void RenderAABB();

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
    void BufferMesh(Mesh* mesh);
    void RenderScene();
    void DrawBox(float3* corners, float3 color);
    void RenderGameWindow();
    void DeleteMesh(Mesh* mesh);
    
    GameObject* LoadFile(std::string file_path);
    Mesh* ImportMesh(aiMesh* aiMesh);
    std::string ImportTexture(const aiScene* scene, int index, std::string path);
    GameObject* ProcessNode(const aiScene* scene, aiNode* node, GameObject* parent, std::string Path);
    
public:
    std::vector<Mesh*> meshes;
    const char* file_path;
    bool MeshDebug;
    int renderedGameMeshes;
    int renderedSceneMeshes;
};

