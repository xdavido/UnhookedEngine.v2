#include "ModuleAssimpMeshes.h"
#include "Application.h"
#include "ModuleTexture.h"
#include"GameObject.h"
#include"ComponentMesh.h"
#include"ComponentMaterial.h"

ModuleAssimpMeshes::ModuleAssimpMeshes(Application* app, bool start_enabled) : Module(app, start_enabled)
{
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

    // Log the mesh name
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

    // Load faces
    if (aiMesh->HasFaces())     
    {
        ourMesh->indexCount = aiMesh->mNumFaces * 3;
        // assume each face is a triangle
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
        
        ourMesh->id_texture = App->textures->textureID;
        ourMesh->texture_height = App->textures->textureWidth;
        ourMesh->texture_width = App->textures->textureWidth;

        BufferMesh(ourMesh);

        ComponentMesh* meshComp = new ComponentMesh();
        ourMesh->owner = CgameObject;
        meshComp->mesh = ourMesh;
        CgameObject->AddComponent(meshComp);

        //Add mesh to meshes vector
        meshes.push_back(ourMesh);

        //Has a texture
        if (scene->HasMaterials()) {
            if (scene->mMaterials[scene->mMeshes[index]->mMaterialIndex]->GetTextureCount(aiTextureType_DIFFUSE) > 0) {
                //Get texture path
                aiString texture_path;
                scene->mMaterials[scene->mMeshes[index]->mMaterialIndex]->GetTexture(aiTextureType_DIFFUSE, 0, &texture_path);
                aiString new_path;
                new_path.Set("Assets/Textures/");
                new_path.Append(texture_path.C_Str());

                //Build component
                ComponentMaterial* matComp = new ComponentMaterial();
                matComp->mOwner = CgameObject;
                matComp->SetTexture(new_path.C_Str());
                CgameObject->AddComponent(matComp);
            }
        }

    }
    else
    {
        delete ourMesh;

    }
}

void Mesh::Render()
{
    glEnable(GL_TEXTURE_COORD_ARRAY);
    glEnable(GL_TEXTURE_2D);
    glEnableClientState(GL_VERTEX_ARRAY);
    glBindTexture(GL_TEXTURE_2D, id_texture);
        
    // Binding buffers
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO); 

    // Draw
    glVertexPointer(3, GL_FLOAT, sizeof(float) * VERTEX, NULL);
    glTexCoordPointer(2, GL_FLOAT, sizeof(float) * VERTEX, (void*)(sizeof(float) * 3));
    glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, NULL);

    // Unbind buffers
    glDisableClientState(GL_VERTEX_ARRAY);

    //cleaning texture
    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_TEXTURE_2D);
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
        double xMedio = (vertexNormalFaces[index1 * 3] + vertexNormalFaces[index2 * 3] + vertexNormalFaces[index3 * 3]) / 3.0;
        double yMedio = (vertexNormalFaces[index1 * 3 + 1] + vertexNormalFaces[index2 * 3 + 1] + vertexNormalFaces[index3 * 3 + 1]) / 3.0;
        double zMedio = (vertexNormalFaces[index1 * 3 + 2] + vertexNormalFaces[index2 * 3 + 2] + vertexNormalFaces[index3 * 3 + 2]) / 3.0;

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
        double lineaLongitud = 0.3;

        // Calcula el punto final de la línea
        double xFinal = xMedio + normalx * lineaLongitud;
        double yFinal = yMedio + normaly * lineaLongitud;
        double zFinal = zMedio + normalz * lineaLongitud;


        // Dibujar la normal como una línea roja desde el punto medio de la cara
        glLineWidth(0.8f);
        glBegin(GL_LINES);
        glVertex3d(xMedio, yMedio, zMedio);
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

    // Dos buffers, vertex i index
    glGenBuffers(1, (GLuint*)&(mesh->VBO));
    glGenBuffers(1, (GLuint*)&(mesh->EBO));

    // Bind and fill buffers
    glBindBuffer(GL_ARRAY_BUFFER, mesh->VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * mesh->vertexCount * VERTEX, mesh->vertex, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    

    //Fill buffers with indices
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * mesh->indexCount, mesh->index, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    //glDisableClientState(GL_VERTEX_ARRAY);

}

void ModuleAssimpMeshes::RenderScene()
{
    //Render the scene
    for (int i = 0; i < meshes.size(); i++) {
        glColor3f(1.0f, 1.0f, 1.0f);
        meshes[i]->Render();
        glColor3f(0.0f, 0.6f, 0.7f);
        if (MeshDebug == true) { 
            meshes[i]->RenderFaceNormals(); 
        }
       /* glColor3f(1, 0, 0);
        meshes[i]->RenderVertexNormals();*/
    }
    glColor3f(1.0f, 1.0f, 1.0f);
}

void ModuleAssimpMeshes::DeleteMesh(Mesh* mesh) {
    
    //minimini: comprobar que esto funcione, sino hacerlo de la manera clásica

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

    // detach log stream
    aiDetachAllLogStreams();
    return true;
}