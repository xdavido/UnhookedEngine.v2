#include "ModuleScene.h"
#include "GameObject.h"
#include "Application.h"
#include "ComponentTransform.h"

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

    root_Assets = new GameObject(nullptr);
    root_Assets->name = ("Assets");



    //Load Street House
    objdebug = App->assimpMeshes->LoadMeshFromFile("Assets/Scenes/scene.DAE");
    //objdebug->transform->setRotation({ -90, 0, 0 });
    root->transform->setRotation({ 0, 0, -90 });


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
    delete objdebug;
    return true;
}


void ModuleScene::SceneWindow()
{
	ImGui::Begin("Scene");
	

	ImGui::End();
}

void ModuleScene::GameWindow()
{
	
	ImGui::Begin("Game");
	
	ImGui::End();
}

GameObject* ModuleScene::CreateGameObject(GameObject* parent)
{
    GameObject* newGameObject = new GameObject(parent);



    return newGameObject;

}

