#include "ModuleScene.h"
#include "GameObject.h"
#include "Application.h"
#include "ComponentTransform.h"


ModuleScene::ModuleScene(Application* app, bool start_enabled) : Module(app, start_enabled) 
{
    name = "Scene";
}

ModuleScene::~ModuleScene() 
{
}

bool ModuleScene::Init()
{

    jsonFile.FileToValue("scene.json");
    return false;
}

bool ModuleScene::Start() {
    
    root = new GameObject(nullptr);
    root->name = ("Scene");



    //Load Baker House
    App->assimpMeshes->LoadMeshFromFile("Assets/Models/BakerHouse.fbx");
   

    return true;
}


update_status ModuleScene::PreUpdate(float dt) {
    
    return UPDATE_CONTINUE;
}

update_status ModuleScene::Update(float dt) {
    
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
    node.SetNewJsonBool(node.ValueToObject(node.GetRootValue()), "pendingToDelete", parentGO->isTimetoDelete);

    JsonParser components = node.SetChild(node.GetRootValue(), "components");
    JsonParser tmp = node;
    for (size_t i = 0; i < parentGO->mComponents.size(); i++)
    {
        // Create Child of component
        num = "Component " + std::to_string(i);
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
                for (int j = 0; j < 4; j++)
                {
                    if (i == 0 && j == 0)num += std::to_string(localTransform.At(i, j));
                    else num += "," + std::to_string(localTransform.At(i, j));
                }
            tmp.SetNewJsonString(tmp.ValueToObject(tmp.GetRootValue()), "LocalTransform", num.c_str());

            break;

        case ComponentType::MESH:
            tmp.SetNewJsonString(tmp.ValueToObject(tmp.GetRootValue()), "Mesh", parentGO->name.c_str());

            break;

        case ComponentType::MATERIAL:
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
	return false;
}

void ModuleScene::UpdateGameObjects()
{
    

    if (saveSceneRequest)SaveScene();
}

