#include "ModuleScene.h"
#include "GameObject.h"
#include "Application.h"


ModuleScene::ModuleScene(Application* app, bool start_enabled) : Module(app, start_enabled) 
{
}

ModuleScene::~ModuleScene() 
{
}

bool ModuleScene::Init()
{
    return false;
}

bool ModuleScene::Start() {
    
    /*root = new GameObject(nullptr);
    root->name = ("Scene");*/

    //Load Baker House
    /*App->assimpMeshes->LoadMeshFromFile("Assets/Models/BakerHouse.fbx");
    App->textures->LoadTexture("Assets/Baker_house.png");*/

    return true;
}


update_status ModuleScene::PreUpdate(float dt) {
    
    return UPDATE_CONTINUE;
}

update_status ModuleScene::Update(float dt) {
    
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


    if (parent != nullptr)
        parent->AddChild(newGameObject);

    return newGameObject;

}

