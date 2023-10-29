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
    
    root = new GameObject(nullptr);
    root->name = ("Scene");

    prova1 = new GameObject(root);
    prova2 = new GameObject(prova1);
    prova3 = new GameObject(prova1);
    prova4 = new GameObject(root);
    prova5 = new GameObject(prova4);
    prova6 = new GameObject(prova4);

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



    return newGameObject;

}

