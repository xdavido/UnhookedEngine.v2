#include "ModuleAssimpMeshes.h"
#include "Application.h"
#include "ModuleTexture.h"
#include"GameObject.h"
#include"ComponentMesh.h"
#include"ComponentMaterial.h"
#include"ComponentTransform.h"

ModuleAssimpMeshes::ModuleAssimpMeshes(Application* app, bool start_enabled) : Module(app, start_enabled)
{
    name = "AssimpMeshes";
}

ModuleAssimpMeshes::~ModuleAssimpMeshes()
{
}

bool ModuleAssimpMeshes::Init()
{
    return true;
}

bool ModuleAssimpMeshes::Start()
{
    bool ret = true;

    // Stream log messages to Debug window
    struct aiLogStream stream;
    stream = aiGetPredefinedLogStream(aiDefaultLogStream_DEBUGGER, nullptr);
    aiAttachLogStream(&stream);

    MeshDebug = true;
    
                
    return ret;
}



GameObject* ModuleAssimpMeshes::LoadFile(std::string file_path)
{
    const aiScene* scene = aiImportFile(file_path.c_str(), aiProcessPreset_TargetRealtime_MaxQuality);

    if (scene != nullptr&& scene->HasMeshes())
    {
        GameObject* OBJ = ProcessNode(scene, scene->mRootNode, App->scene->root, file_path);
        aiReleaseImport(scene);

        return OBJ;
    }
    else
    {
        LOG("Error loading scene: %s", file_path);
    }
    
}

Mesh* ModuleAssimpMeshes::ImportMesh(aiMesh* aiMesh)
{
    Mesh* mesh = new Mesh();
    // copy vertices
    mesh->vertexCount = aiMesh->mNumVertices;
    mesh->vertex = new float[mesh->vertexCount * VERTEX];

    for (int k = 0; k < mesh->vertexCount; k++) {

        mesh->vertex[k * VERTEX] = aiMesh->mVertices[k].x;
        mesh->vertex[k * VERTEX + 1] = aiMesh->mVertices[k].y;
        mesh->vertex[k * VERTEX + 2] = aiMesh->mVertices[k].z;

        if (aiMesh->mTextureCoords[0] == nullptr) continue;
        mesh->vertex[k * VERTEX + 3] = aiMesh->mTextureCoords[0][k].x;
        // -1 to invert uv's
        mesh->vertex[k * VERTEX + 4] = aiMesh->mTextureCoords[0][k].y;

    }

    LOG("New mesh with %d vertices", mesh->vertexCount);

    // copy faces
    if (aiMesh->HasFaces())
    {
        mesh->indexCount = aiMesh->mNumFaces * 3;
        mesh->index = new uint[mesh->indexCount]; // assume each face is a triangle

        for (uint j = 0; j < aiMesh->mNumFaces; j++)
        {
            if (aiMesh->mFaces[j].mNumIndices != 3) {
                LOG("WARNING, geometry face with != 3 indices!");
            }
            else
            {
                memcpy(&mesh->index[j * 3], aiMesh->mFaces[j].mIndices, 3 * sizeof(uint));
            }

        }

        mesh->InitAABB();

        
        BufferMesh(mesh);

        return mesh;
    }
    else {

        delete mesh;

        return nullptr;
    }
}

std::string ModuleAssimpMeshes::ImportTexture(const aiScene* scene, int index, std::string path)
{

    if (scene->HasMaterials())
    {
        aiMaterial* MaterialIndex = scene->mMaterials[scene->mMeshes[index]->mMaterialIndex];
        if (MaterialIndex->GetTextureCount(aiTextureType_DIFFUSE) > 0) {

            aiString TextPath;
            MaterialIndex->GetTexture(aiTextureType_DIFFUSE, 0, &TextPath);

            for (int i = 0; i < path.size(); i++)
            {
                if (path[i] == '\\')
                {
                    path[i] = '/';
                }
            }

            std::string NormTextPath = TextPath.C_Str();

            for (int i = 0; i < NormTextPath.size(); i++)
            {
                if (NormTextPath[i] == '\\') 
                {
                    NormTextPath[i] = '/';
                }
            }

            std::string AssetsPath = path;
            uint AssetsPos = AssetsPath.find("Assets/");

            AssetsPath = AssetsPath.substr(AssetsPos, AssetsPath.find_last_of("/") - AssetsPos);
            AssetsPath = AssetsPath.substr(AssetsPos, AssetsPath.find_last_of("/") - AssetsPos);
            AssetsPath.append("/Textures/").append(TextPath.C_Str());

            return AssetsPath;
        }
    }

    return "";
    
}

GameObject* ModuleAssimpMeshes::ProcessNode(const aiScene* scene, aiNode* node, GameObject* parent, std::string Path)
{
    if (node->mNumMeshes == 0 && node->mNumChildren == 0) return nullptr;

    GameObject* gObj = new GameObject(parent);

    gObj->name = node->mName.C_Str();

    aiVector3D scale, position, rotation;
    aiQuaternion QuatRotation;

    node->mTransformation.Decompose(scale, QuatRotation, position);
    rotation = QuatRotation.GetEuler();

    gObj->transform->getScale() = float3(scale.x, scale.y, scale.z);
    gObj->transform->position = float3(position.x, position.y, position.z);
    gObj->transform->calculateMatrix();

    if (node->mNumMeshes != 0) {

        ComponentMesh* component = new ComponentMesh(gObj);


        std::string texture_path = "";


        for (int i = 0; i < node->mNumMeshes; i++)
        {
            Mesh* mesh = ImportMesh(scene->mMeshes[node->mMeshes[i]]);

            if (mesh == nullptr) {
                LOG("Error loading scene %s", Path);
                continue;
            }

            mesh->owner = gObj;
            component->meshes.push_back(mesh);

            if (texture_path == "") texture_path = ImportTexture(scene, node->mMeshes[i], Path);

        }

        gObj->mComponents.push_back(component);

        if (texture_path != "") {
            ComponentMaterial* componentT = new ComponentMaterial(gObj);
            gObj->mComponents.push_back(componentT);
            componentT->SetTexture(texture_path.c_str());
        }
    }

    for (int i = 0; i < node->mNumChildren; i++) {
        ProcessNode(scene, node->mChildren[i], gObj, Path);
    }

    return gObj;
}
   


void Mesh::Render()
{
    glEnable(GL_TEXTURE_COORD_ARRAY);
    glEnable(GL_TEXTURE_2D);
    glEnableClientState(GL_VERTEX_ARRAY);
    glBindTexture(GL_TEXTURE_2D, id_texture);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO); 

    glVertexPointer(3, GL_FLOAT, sizeof(float) * VERTEX, NULL);
    glTexCoordPointer(2, GL_FLOAT, sizeof(float) * VERTEX, (void*)(sizeof(float) * 3));

    glPushMatrix(); // Bind transform matrix

    if (owner != nullptr) {
        
        glMultMatrixf((owner->GetTransformComponent()->getGlobalMatrix().ptr()));
    }

    glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, NULL);

    glPopMatrix();
    glDisableClientState(GL_VERTEX_ARRAY);

    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_TEXTURE_2D);
}

void Mesh::InitAABB()
{
    std::vector<float3> correctVertex;
    for (size_t i = 0; i < vertexCount * VERTEX; i += VERTEX)
    {
        correctVertex.emplace_back(vertex[i], vertex[i + 1], vertex[i + 2]);
    }
    _AABB.SetFrom(&correctVertex[0], correctVertex.size());
}

void Mesh::RenderAABB()
{
    float3 corners1[8];

    _OBB.GetCornerPoints(corners1);

    App->assimpMeshes->DrawBox(corners1, float3(182, 149, 192));

    float3 corners2[8];
    GlobalAABB.GetCornerPoints(corners2);

    App->assimpMeshes->DrawBox(corners2, float3(182, 149, 192));
}
void ModuleAssimpMeshes::DrawBox(float3* corners, float3 color)
{
    int indices[24] = { 0,2,2,6,6,4,4,0,0,1,1,3,3,2,4,5,6,7,5,7,3,7,1,5 };
    glBegin(GL_LINES);
    glColor3fv(color.ptr());

    for (size_t i = 0; i < 24; i++)
    {
        glVertex3fv(corners[indices[i]].ptr());
    }

    glColor3f(255.f, 255.f, 255.f);
    glEnd();
}
void Mesh::RenderVertexNormals()
{
    //Vertex normals
    float normalLenght = 0.05f;
    
    glBegin(GL_LINES);
    for (unsigned int i = 0; i < vertexCount * 3; i += 3)
    {
        glVertex3f(vertex[i], vertex[i + 1], vertex[i + 2]);
        glVertex3f(vertex[i] + vertexNormals[i].x * normalLenght, vertex[i + 1] + vertexNormals[i].y * normalLenght, vertex[i + 2] + vertexNormals[i].z * normalLenght);
    }
    glEnd();
    

}

void Mesh::RenderFaceNormals()
{
    for (size_t i = 0; i < indexCount; i += 3)
    {
        // Obtener los índices de los vértices para esta cara
        size_t index1 = index[i];
        size_t index2 = index[i + 1];
        size_t index3 = index[i + 2];

        // Calcular el punto medio de la cara del triángulo
        double xMidpoint = (vertexNormalFaces[index1 * 3] + vertexNormalFaces[index2 * 3] + vertexNormalFaces[index3 * 3]) / 3.0;
        double yMidPoint = (vertexNormalFaces[index1 * 3 + 1] + vertexNormalFaces[index2 * 3 + 1] + vertexNormalFaces[index3 * 3 + 1]) / 3.0;
        double zMidPoint = (vertexNormalFaces[index1 * 3 + 2] + vertexNormalFaces[index2 * 3 + 2] + vertexNormalFaces[index3 * 3 + 2]) / 3.0;

        // Calcular la normal de la cara del triángulo usando el producto cruz
        double edge1x = vertexNormalFaces[index2 * 3] - vertexNormalFaces[index1 * 3];
        double edge1y = vertexNormalFaces[index2 * 3 + 1] - vertexNormalFaces[index1 * 3 + 1];
        double edge1z = vertexNormalFaces[index2 * 3 + 2] - vertexNormalFaces[index1 * 3 + 2];

        double edge2x = vertexNormalFaces[index3 * 3] - vertexNormalFaces[index1 * 3];
        double edge2y = vertexNormalFaces[index3 * 3 + 1] - vertexNormalFaces[index1 * 3 + 1];
        double edge2z = vertexNormalFaces[index3 * 3 + 2] - vertexNormalFaces[index1 * 3 + 2];

        double normalx = edge1y * edge2z - edge1z * edge2y;
        double normaly = edge1z * edge2x - edge1x * edge2z;
        double normalz = edge1x * edge2y - edge1y * edge2x;

        // Normaliza la normal
        double length = sqrt(normalx * normalx + normaly * normaly + normalz * normalz);
        normalx /= length;
        normaly /= length;
        normalz /= length;

        // Define la longitud de la línea en la dirección de la normal
        double lineLength = 0.3;

        // Calcula el punto final de la línea
        double xFinal = xMidpoint + normalx * lineLength;
        double yFinal = yMidPoint + normaly * lineLength;
        double zFinal = zMidPoint + normalz * lineLength;


        // Dibujar la normal como una línea roja desde el punto medio de la cara
        glLineWidth(0.8f);
        glBegin(GL_LINES);
        glVertex3d(xMidpoint, yMidPoint, zMidPoint);
        glVertex3d(xFinal, yFinal, zFinal);
        glEnd();
        glLineWidth(1.0f);
    }
    
}



void ModuleAssimpMeshes::BufferMesh(Mesh* mesh)
{
    //Fill buffers with vertex


    glGenBuffers(1, (GLuint*)&(mesh->VBO));
    glGenBuffers(1, (GLuint*)&(mesh->EBO));


    glBindBuffer(GL_ARRAY_BUFFER, mesh->VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * mesh->vertexCount * VERTEX, mesh->vertex, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    


    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * mesh->indexCount, mesh->index, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    meshes.push_back(mesh);

}

void ModuleAssimpMeshes::RenderScene()
{
    renderedSceneMeshes = 0;
    for (int i = 0; i < meshes.size(); i++) {
        if (App->camera->camera->FrustumCulling(meshes[i])) {
            glColor3f(1.0f, 1.0f, 1.0f);
            meshes[i]->_OBB = meshes[i]->_AABB;
            meshes[i]->_OBB.Transform(meshes[i]->owner->transform->getGlobalMatrix().Transposed());
            meshes[i]->GlobalAABB.SetNegativeInfinity();
            meshes[i]->GlobalAABB.Enclose(meshes[i]->_OBB);
            meshes[i]->Render();
            meshes[i]->RenderAABB();
            renderedSceneMeshes++;
            glColor3f(0.0f, 0.6f, 0.7f);
            //Frustum debug
            if (meshes[i]->owner->GetMeshComponent()->faceNormals) {
                meshes[i]->RenderFaceNormals();
            }
        }
    }
    
    ComponentCamera* pilota = App->renderer3D->GetMainCamera();
    if (pilota != nullptr) {
        float3 corners[8];
        pilota->frustum.GetCornerPoints(corners);
        DrawBox(corners, float3(1, .5, .9));
    }
    glColor3f(1.0f, 1.0f, 1.0f);
    for (int i = 0; i < App->particle->emitterVector.size(); i++) {
        App->particle->emitterVector[i]->RenderParticles();
    }

}

void ModuleAssimpMeshes::RenderGameWindow()
{
    renderedGameMeshes = 0;

    //Render Game Window
    for (int i = 0; i < meshes.size(); i++) {
        if (!App->renderer3D->GetMainCamera()->FrustumCulling(meshes[i])) continue;

        meshes[i]->Render();
        renderedGameMeshes++;
    }
    for (int i = 0; i < App->particle->emitterVector.size(); i++) {
        App->particle->emitterVector[i]->RenderParticles();
    }
}

void ModuleAssimpMeshes::DeleteMesh(Mesh* mesh) {
    
    

    auto it = std::find(meshes.begin(), meshes.end(), mesh);

    if (it != meshes.end()) {
        meshes.erase(it);
        delete mesh;
        mesh = nullptr;
    }
    else
    {
        LOG("DELETE MESH NO HA ENCONTRADO LA MESH DESEADA DE ELIMINAR")
    }
}

bool ModuleAssimpMeshes::CleanUp()
{

   
    aiDetachAllLogStreams();
    return true;
}