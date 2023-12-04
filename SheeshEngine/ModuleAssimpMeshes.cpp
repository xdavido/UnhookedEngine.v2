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



GameObject* ModuleAssimpMeshes::LoadMeshFromFile(const char* file_path)
{
    const aiScene* scene = aiImportFile(file_path, aiProcess_Triangulate|aiProcess_FlipUVs);
    

    if (scene != nullptr&& scene->HasMeshes())
    {
        GameObject* OBJ = new GameObject(App->scene->root);
        OBJ->name = std::string(file_path).substr(std::string(file_path).find_last_of(char(92)) + 1);
        OBJ->name = OBJ->name.substr(std::string(file_path).find_last_of("/") + 1);
        for (int i = 0; i < scene->mNumMeshes; i++)
        {
            GameObject* obj = new GameObject();
            OBJ->SetAsChildOf(obj);
            obj->name = "Mesh_" + std::to_string(i);
            ImportAssimpMesh(scene->mMeshes[i],OBJ, obj, scene,i);
           
        }
       

           
        
        
        aiReleaseImport(scene);

        return OBJ;
    }
    else
    {
        LOG("Error loading scene: %s", file_path);
    }
    
}

void ModuleAssimpMeshes::ImportAssimpMesh(aiMesh* aiMesh, GameObject* PgameObject, GameObject* CgameObject, const aiScene* scene, int index)
{
    Mesh* ourMesh = new Mesh();


    std::string assimpMeshName = "Importing Assimp Mesh: " + std::string(aiMesh->mName.C_Str());
    LOG(assimpMeshName.c_str());


    ourMesh->vertexCount = aiMesh->mNumVertices;
    ourMesh->vertex = new float[ourMesh->vertexCount * VERTEX];

    ourMesh->vertexNormalFaces = new float[ourMesh->vertexCount * 3];
    memcpy(ourMesh->vertexNormalFaces, aiMesh->mVertices, sizeof(float) * ourMesh->vertexCount * 3);

    for (int v = 0; v < ourMesh->vertexCount; v++) {    
        ourMesh->vertex[v * VERTEX] = aiMesh->mVertices[v].x;
        ourMesh->vertex[v * VERTEX + 1] = aiMesh->mVertices[v].y;
        ourMesh->vertex[v * VERTEX + 2] = aiMesh->mVertices[v].z;

        ourMesh->vertex[v * VERTEX + 3] = aiMesh->mTextureCoords[0][v].x;
        ourMesh->vertex[v * VERTEX + 4] = aiMesh->mTextureCoords[0][v].y;
    }


    if (aiMesh->HasFaces())     
    {
        ourMesh->indexCount = aiMesh->mNumFaces * 3;
       
        ourMesh->index = new uint[ourMesh->indexCount]; 

        for (uint i = 0; i < aiMesh->mNumFaces; ++i)
        {
            if (aiMesh->mFaces[i].mNumIndices != 3)
            {
                LOG("WARNING, geometry face with != 3 indices!");
            }
            else
            {
                memcpy(&ourMesh->index[i * 3], aiMesh->mFaces[i].mIndices, 3 * sizeof(uint));
            }
        }

        ourMesh->VBO = 0;
        ourMesh->EBO = 0;
        


        BufferMesh(ourMesh);

        
        meshes.push_back(ourMesh);

        ComponentMesh* meshComp = new ComponentMesh(CgameObject);
        ourMesh->owner = CgameObject;
        ourMesh->InitAABB();
        meshComp->mesh = ourMesh;
        CgameObject->AddComponent(meshComp);

        ourMesh->id_texture = App->textures->checkersID;

        /*meshComp->type = ComponentType::MESH;*/

        
        if (scene->HasMaterials()) {
            if (scene->mMaterials[scene->mMeshes[index]->mMaterialIndex]->GetTextureCount(aiTextureType_DIFFUSE) > 0) {
                
                aiString texture_path;
                scene->mMaterials[scene->mMeshes[index]->mMaterialIndex]->GetTexture(aiTextureType_DIFFUSE, 0, &texture_path);
                aiString new_path;
                new_path.Set("Assets/Textures/");
                new_path.Append(texture_path.C_Str());
               
               
                ComponentMaterial* matComp = new ComponentMaterial(CgameObject);
                matComp->mOwner = CgameObject;
                matComp->SetTexture(new_path.C_Str());
                CgameObject->AddComponent(matComp);
            }
            else
            {
                ComponentMaterial* mat = new ComponentMaterial();
                mat->mOwner = CgameObject;
                CgameObject->AddComponent(mat);
            }
        }

        ourMesh->texture_height = App->textures->textureWidth;
        ourMesh->texture_width = App->textures->textureWidth;

    


    }
    else
    {
        delete ourMesh;

    }
}

mat4x4 ConvertFloat4x4ToMat4(const float4x4& floatMatrix) {

    mat4x4 mat;
    floatMatrix[1];
    mat[0] = floatMatrix.At(0, 0);
    mat[1] = floatMatrix.At(0, 1);
    mat[2] = floatMatrix.At(0, 2);
    mat[3] = floatMatrix.At(0, 3);

    mat[4] = floatMatrix.At(1, 0);
    mat[5] = floatMatrix.At(1, 1);
    mat[6] = floatMatrix.At(1, 2);
    mat[7] = floatMatrix.At(1, 3);

    mat[8] = floatMatrix.At(2, 0);
    mat[9] = floatMatrix.At(2, 1);
    mat[10] = floatMatrix.At(2, 2);
    mat[11] = floatMatrix.At(2, 3);

    mat[12] = floatMatrix.At(3, 0);
    mat[13] = floatMatrix.At(3, 1);
    mat[14] = floatMatrix.At(3, 2);
    mat[15] = floatMatrix.At(3, 3);

    return mat;
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
        
        glMultMatrixf((&ConvertFloat4x4ToMat4(owner->GetTransformComponent()->getGlobalMatrix())));
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

    DrawBox(corners1, float3(182, 149, 192));

    float3 corners2[8];
    GlobalAABB.GetCornerPoints(corners2);

    DrawBox(corners2, float3(182, 149, 192));
}
void Mesh::DrawBox(float3* corners, float3 color)
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
    for (unsigned int i = 0; i < /*indexCount*/vertexCount * 3; i += 3)
    {
        glVertex3f(vertex[i], vertex[i + 1], vertex[i + 2]);
        glVertex3f(vertex[i] + vertexNormals[i].x * normalLenght, vertex[i + 1] + vertexNormals[i].y * normalLenght, vertex[i + 2] + vertexNormals[i].z * normalLenght);
    }
    glEnd();
    

}

void Mesh::RenderFaceNormals()
{
    //---num_index
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
    

    ////---num_vertex
    //for (size_t i = 0; i < num_vertex; i += 9)
    //{

    //    // Calcular el punto medio de la cara del triángulo
    //    double xMedio = (vertex[i] + vertex[i + 3] + vertex[i + 6]) / 3.0;
    //    double yMedio = (vertex[i + 1] + vertex[i + 4] + vertex[i + 7]) / 3.0;
    //    double zMedio = (vertex[i + 2] + vertex[i + 5] + vertex[i + 8]) / 3.0;

    //    // Calcular la normal de la cara del triángulo usando el producto cruz
    //    double edge1x = vertex[i + 3] - vertex[i];
    //    double edge1y = vertex[i + 4] - vertex[i + 1];
    //    double edge1z = vertex[i + 5] - vertex[i + 2];

    //    double edge2x = vertex[i + 6] - vertex[i];
    //    double edge2y = vertex[i + 7] - vertex[i + 1];
    //    double edge2z = vertex[i + 8] - vertex[i + 2];

    //    double normalx = edge1y * edge2z - edge1z * edge2y;
    //    double normaly = edge1z * edge2x - edge1x * edge2z;
    //    double normalz = edge1x * edge2y - edge1y * edge2x;

    //    // Normaliza la normal
    //    double length = sqrt(normalx * normalx + normaly * normaly + normalz * normalz);
    //    normalx /= length;
    //    normaly /= length;
    //    normalz /= length;

    //    // Define la longitud de la línea en la dirección de la normal
    //    double lineaLongitud = 3.0;

    //    // Calcula el punto final de la línea
    //    double xFinal = xMedio + normalx * lineaLongitud;
    //    double yFinal = yMedio + normaly * lineaLongitud;
    //    double zFinal = zMedio + normalz * lineaLongitud;

    //    // Establecer el color a rojo
    //    glColor3f(1.0f, 0.0f, 0.0f);

    //    // Dibujar la normal como una línea roja desde el punto medio de la cara
    //    glLineWidth(2.0f);
    //    glBegin(GL_LINES);
    //    glVertex3d(xMedio, yMedio, zMedio);
    //    glVertex3d(xFinal, yFinal, zFinal);
    //    glEnd();
    //    glLineWidth(1.0f);


    //    glEnd();
    //}
    
}



void ModuleAssimpMeshes::BufferMesh(Mesh* mesh)
{
    //Fill buffers with vertex
    // glEnableClientState(GL_VERTEX_ARRAY);


    glGenBuffers(1, (GLuint*)&(mesh->VBO));
    glGenBuffers(1, (GLuint*)&(mesh->EBO));


    glBindBuffer(GL_ARRAY_BUFFER, mesh->VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * mesh->vertexCount * VERTEX, mesh->vertex, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    


    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * mesh->indexCount, mesh->index, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    //glDisableClientState(GL_VERTEX_ARRAY);

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
            if (meshes[i]->owner->GetMeshComponent()->faceNormals) {
                meshes[i]->RenderFaceNormals();
            }
            /* glColor3f(1, 0, 0);
                meshes[i]->RenderVertexNormals();*/
        }
    }
    glColor3f(1.0f, 1.0f, 1.0f);

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