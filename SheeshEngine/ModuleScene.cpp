#include "ModuleScene.h"
#include "GameObject.h"
#include "Application.h"
#include "ComponentTransform.h"
#include "ComponentMaterial.h"


ModuleScene::ModuleScene(Application* app, bool start_enabled) : Module(app, start_enabled)
{
    name = "Scene";
}

ModuleScene::~ModuleScene()
{
}

bool ModuleScene::Init()
{
    root = new GameObject(nullptr);
    root->name = ("Scene");
    jsonFile.FileToValue("scene.json");
    return false;
}

bool ModuleScene::Start() {





    //Load Baker House
    App->assimpMeshes->LoadMeshFromFile("Assets/Models/BakerHouse.fbx");


    return true;
}


update_status ModuleScene::PreUpdate(float dt) {

    return UPDATE_CONTINUE;
}

update_status ModuleScene::Update(float dt) {

    root->Update();
    UpdateGameObjects();
    return UPDATE_CONTINUE;
}

update_status ModuleScene::PostUpdate(float dt) {

    return UPDATE_CONTINUE;
}

bool ModuleScene::CleanUp() {

    LOG("Scene Cleanup")
        delete root;
    return true;
}

GameObject* ModuleScene::CreateGameObject(GameObject* parent)
{
    GameObject* newGameObject = new GameObject(parent);



    return newGameObject;

}

bool ModuleScene::SaveScene()
{
    LOG("Saving scene");

    rootFile = jsonFile.GetRootValue();

    JsonParser scene = jsonFile.SetChild(rootFile, "GameObjects");

    SaveGameObjects(root, scene.SetChild(scene.GetRootValue(), root->name.c_str()));

    jsonFile.SerializeFile(rootFile, "scene.json");
    saveSceneRequest = false;
    return true;
}


void ModuleScene::SaveGameObjects(GameObject* parentGO, JsonParser& node) {
    std::string num;
    JsonParser& child = node;
    ComponentTransform* transform;
    float4x4 localTransform;

    node.SetNewJsonString(node.ValueToObject(node.GetRootValue()), "name", parentGO->name.c_str());
    node.SetNewJsonBool(node.ValueToObject(node.GetRootValue()), "active", parentGO->isActive);
    node.SetNewJsonBool(node.ValueToObject(node.GetRootValue()), "isTimeToDelete", parentGO->isTimetoDelete);


    JsonParser components = node.SetChild(node.GetRootValue(), "components");
    JsonParser tmp = node;
    for (size_t i = 0; i < parentGO->mComponents.size(); i++)
    {
        // Create Child of component
        num = "Component " + std::to_string(i);
        //num += FormatComponentType(parentGO, i);

        

        tmp = components.SetChild(components.GetRootValue(), num.c_str());

        tmp.SetNewJsonNumber(tmp.ValueToObject(tmp.GetRootValue()), "Type", (int)parentGO->mComponents.at(i)->type);
        tmp.SetNewJsonBool(tmp.ValueToObject(tmp.GetRootValue()), "active", parentGO->mComponents.at(i)->isActive);

        switch ((ComponentType)parentGO->mComponents.at(i)->type)
        {
        case ComponentType::TRANSFORM:
            num = "";
            transform = (ComponentTransform*)(parentGO->mComponents.at(i));
            localTransform = transform->getLocalMatrix();
            for (int i = 0; i < 4; i++)
            {
                for (int j = 0; j < 4; j++)
                {
                   /* if (i == 0 && j == 0)num += std::to_string(localTransform.At(i, j));
                    else num += "," + std::to_string(localTransform.At(i, j));*/

                    num += std::to_string(localTransform.At(i, j));
                    num += ",";
                }
            }
                
            tmp.SetNewJsonString(tmp.ValueToObject(tmp.GetRootValue()), "LocalTransform", num.c_str());

            break;

        case ComponentType::MESH:

            tmp.SetNewJsonString(tmp.ValueToObject(tmp.GetRootValue()), "Mesh", parentGO->name.c_str());

            break;

        case ComponentType::MATERIAL:
            
            ComponentMaterial* componentMaterial; 
            componentMaterial = (ComponentMaterial*)(parentGO->mComponents.at(i));
            
            tmp.SetNewJsonString(tmp.ValueToObject(tmp.GetRootValue()), "Material", componentMaterial->pathTexture.c_str());

            
            break;

        default:
            break;

        }
        parentGO->mComponents.at(i)->type;
    }

    for (size_t i = 0; i <= parentGO->mChildren.size(); i++)
    {

        num = "Child " + std::to_string(i);

        if (parentGO->mChildren.size() > i) {
            SaveGameObjects(parentGO->mChildren[i], child.SetChild(child.GetRootValue(), num.c_str()));
        }
    }
}



bool ModuleScene::LoadScene()
{
    LOG("Loading configurations");

   
    rootFile = jsonFile.GetRootValue();

    rootGO = jsonFile.GetChild(rootFile, "GameObjects");
    
    LoadGameObject(rootGO.GetChild(rootGO.GetRootValue(), "Scene"));

    loadSceneRequest = false;




    return false;
}




void ModuleScene::LoadComponents(JsonParser& parent, std::string num, GameObject* gamObj)
{

    ComponentTransform* transform;
    ComponentMesh* meshRender;
    Mesh* mesh;
    ComponentMaterial* material;
    float3 size = float3::one;
    LOG("Loading Components \n");
    std::string debugPath;
    std::string debugUID;

    JsonParser components = parent.GetChild(parent.GetRootValue(), "components");
    JsonParser tmp = components;

    std::string pos;

    for (int i = 0; i < 4; i++)
    {
        num = "Component " + std::to_string(i);
        LOG((std::string("Loading ") + num).c_str());

        if (components.ExistChild(components.GetRootValue(), num.c_str()))
        {
            tmp = components.GetChild(components.GetRootValue(), num.c_str());
            switch ((ComponentType)(int)tmp.JsonValToNumber("Type"))
            {
            case ComponentType::TRANSFORM:
                gamObj->transform->SetLocalMatrix(strMatrixToF4x4(tmp.ValueToString("LocalTransform")));

                LOG(gamObj->transform->getLocalMatrix().ToString().c_str());

                //gamObj->transform = transform;

                break;
            case ComponentType::MESH:
                /*gamObj->AddComponent(ComponentType::MESHRENDERER);
                meshRender = static_cast<MeshRenderer*>(gamObj->GetComponent(ComponentType::MESHRENDERER));

                if (meshRender != NULL && tmp.JsonValToNumber("PrimType") == -1)
                {
                    mesh = new Mesh();
                    debugPath = tmp.JsonValToString("LibraryPath");
                    mesh->SetAssetsPath(tmp.JsonValToString("Mesh"));
                    mesh->SetLibraryPath(debugPath.c_str());
                    mesh->LoadFromFME(debugPath.c_str());

                    meshRender->SetMesh(mesh);
                    meshRender->SetBoundingBoxes(mesh);
                }
                else
                {
                    size = float3(tmp.JsonValToNumber("SizeX"), tmp.JsonValToNumber("SizeY"), tmp.JsonValToNumber("SizeZ"));
                    mesh = app->editor->LoadPrimitive(tmp.JsonValToNumber("PrimType"), size, tmp.JsonValToNumber("Radius"), size.y);
                    meshRender->SetMesh(mesh);
                    meshRender->SetBoundingBoxes(mesh);
                }

                meshRender->SetOwner(gamObj);*/

                break;
            case ComponentType::MATERIAL:
                //gamObj->AddComponent(ComponentType::MATERIAL);
                //material = static_cast<Material*>(gamObj->GetComponent(ComponentType::MATERIAL));
                //material->active = tmp.JsonValToBool("active");
                //material->texture = new Texture(tmp.JsonValToString("Material"), gamObj->name);
                //material->texture->SetLibraryPath(tmp.JsonValToString("LibraryPath"));
                //material->texture->path = material->texture->GetLibraryPath();
                ////material->texture->SetAssetsPath();
                //material->SetOwner(gamObj);
                //material->texture->LoadToMemory();

                break;

            }

        }
        else
        {
            break;
        }
            
    }

}


GameObject* ModuleScene::LoadGameObject(JsonParser parent, GameObject* father)
{
    std::string num;
    std::string convert;

    std::string name = parent.ValueToString("name");
    GameObject* gamObj = new GameObject();
    gamObj->name = name;
    gamObj->isActive = parent.JsonValToBool("active");
    gamObj->SetPendingToDelete(parent.JsonValToBool("isTimeToDelete"));

    LoadComponents(parent, num, gamObj);
    int count = 0;
    num = "Child " + std::to_string(count);
    while (parent.ExistChild(parent.GetRootValue(), num.c_str()))
    {
        

        gamObj->AttachChild(LoadGameObject(parent.GetChild(parent.GetRootValue(), num.c_str()), gamObj));
        ++count;
        num = "Child " + std::to_string(count);
    }


    return gamObj;
}

void ModuleScene::UpdateGameObjects()
{
    if (saveSceneRequest)SaveScene();
    if (loadSceneRequest)LoadScene();
}

const char* ModuleScene::FormatComponentType(GameObject* parentGO, const size_t& i)
{
    switch ((ComponentType)parentGO->mComponents.at(i)->type)
    {
    case ComponentType::TRANSFORM:

        return " Transform";
        break;

    case ComponentType::MESH:

        return " Mesh";
        break;

    case ComponentType::MATERIAL:

        return " Material";
        break;

    default:
        break;

    }
}

float4x4 ModuleScene::strMatrixToF4x4(const char* convert)
{
    std::string text = convert;
    std::string delimiter = ",";
    std::vector<float> floatArray{};

    size_t pos = 0;
    while ((pos = text.find(delimiter)) != std::string::npos) {
        floatArray.push_back(stof(text.substr(0, pos)));
        text.erase(0, pos + delimiter.length());
    }


    float4x4 matrix;
    int count = 0;

    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
        {

            matrix.At(i, j) = floatArray.at(count);
            ++count;
        }

    return matrix;
}