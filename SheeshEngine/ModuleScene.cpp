#include "ModuleScene.h"
#include "GameObject.h"
#include "Application.h"


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
    for (auto child : parentGO->mChildren) {
        JsonParser& childNode = node.SetChild(node.SetChild(node.GetRootValue(), "Child").GetRootValue(), child->name.c_str());

        node.SetNewJsonBool(node.ValueToObject(childNode.GetRootValue()), "isActive", child->isActive);

        node.SetNewJsonString(node.ValueToObject(childNode.GetRootValue()), "name", child->name.c_str());

        node.SetNewJsonBool(node.ValueToObject(childNode.GetRootValue()), "pendingToDelete", child->isTimetoDelete);

        JsonParser& components = node.SetChild(childNode.GetRootValue(), "components");

        for (auto component : child->mComponents) {
            components.SetNewJsonBool(node.ValueToObject(components.GetRootValue()), "isActive", child->isActive); 
        }

        SaveGameObjects(child, node); 
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

