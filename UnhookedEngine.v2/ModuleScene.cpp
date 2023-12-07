#include "ModuleScene.h"
#include "GameObject.h"
#include "Application.h"
#include "ComponentTransform.h"
#include "ComponentMesh.h"
#include "ComponentCamera.h"


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

    ImVec2 WindowSize = ImGui::GetContentRegionAvail();

    //Prevent image stretching by setting new aspect ratio
    float aspectRatio = WindowSize.x / WindowSize.y;
    App->camera->sceneCam->FrustumCam.verticalFov = App->camera->sceneCam->FOV * DEGTORAD;
    App->camera->sceneCam->FrustumCam.horizontalFov = 2.0f * atanf(tanf(App->camera->sceneCam->FrustumCam.verticalFov / 2.0f) * aspectRatio);

    ImGui::Image((ImTextureID)App->camera->sceneCam->cameraBuffer, WindowSize, ImVec2(0, 1), ImVec2(1, 0));
	


	ImGui::End();
}

void ModuleScene::GameWindow()
{
	
	ImGui::Begin("Game");

    ImVec2 WindowSize = ImGui::GetContentRegionAvail();

    float aspectRatio = WindowSize.x / WindowSize.y;
    App->renderer3D->mainCam->FrustumCam.verticalFov = App->renderer3D->mainCam->FOV * DEGTORAD;
    App->renderer3D->mainCam->FrustumCam.horizontalFov = 2.0f * atanf(tanf(App->renderer3D->mainCam->FrustumCam.verticalFov / 2.0f) * aspectRatio);

    ImGui::Image((ImTextureID)App->renderer3D->mainCam->cameraBuffer, WindowSize, ImVec2(0, 1), ImVec2(1, 0));

	ImGui::End();
}

GameObject* ModuleScene::CreateGameObject(GameObject* parent)
{
    GameObject* newGameObject = new GameObject(parent);



    return newGameObject;

}

