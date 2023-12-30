#include "SceneWindow.h"
#include "Imgui/imgui.h"
#include "ComponentCamera.h"
#include "ComponentMesh.h"
#include "ModuleAssimpMeshes.h"
#include "ComponentTransform.h"
#include"ModuleHierarchy.h"

bool SceneWindows::isHovered = false;

void SceneWindows::PrintScene(Application* app)
{
    
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    ImGui::Begin("Scene");
   App->editor->sizeWindScn = ImGui::GetContentRegionAvail();

    App->editor->guizmoWindowPos = ImGui::GetWindowPos();
    App->editor->guizmoOffset = ImGui::GetFrameHeight() / 2;
    App->editor->guizmoSize = ImGui::GetContentRegionAvail();
    
    float aspectRatio = App->editor->sizeWindScn.x / App->editor->sizeWindScn.y;
    app->camera->camera->SetAspectRatio(aspectRatio);
    ImGui::Image((ImTextureID)app->camera->camera->cameraBuffer, App->editor->sizeWindScn, ImVec2(0, 1), ImVec2(1, 0));

    if (ImGui::IsWindowHovered())
    {
        isHovered = true;
    }
    else
    {
        isHovered = false;
    }


    if (ImGui::IsMouseClicked(0) && App->input->GetKey(SDL_SCANCODE_LALT) != KEY_REPEAT && ImGui::IsWindowHovered() && !ImGuizmo::IsOver())
    {
        std::vector<GameObject*> PickedGO;

        ImVec2 mousePos = ImGui::GetMousePos();


        ImVec2 norm = NormMousePos(ImGui::GetWindowPos().x,
            ImGui::GetWindowPos().y + ImGui::GetFrameHeight(),
            ImGui::GetWindowSize().x,
            ImGui::GetWindowSize().y - ImGui::GetFrameHeight(), mousePos);

        

        LineSegment picking = App->camera->camera->frustum.UnProjectLineSegment(norm.x, norm.y);
        App->renderer3D->linesegment = picking;

        for (size_t i = 0; i < App->assimpMeshes->meshes.size(); i++)
        {
            if (picking.Intersects(App->assimpMeshes->meshes[i]->_OBB))
            {
                LOG("%d", App->assimpMeshes->meshes[i]->vertexCount)

                    if (App->assimpMeshes->meshes[i]->owner != nullptr)
                        PickedGO.push_back(App->assimpMeshes->meshes[i]->owner);
            }
        }


        float currentDist;
        float minDist = 0;

        for (size_t j = 0; j < PickedGO.size(); j++) {

            ComponentMesh* gObjMesh = PickedGO[j]->GetMeshComponent();
            if (gObjMesh != nullptr) {

                for (size_t i = 0; i < gObjMesh->meshes.size(); i++) {

                    Mesh* mesh = gObjMesh->meshes[i];
                    float4x4 matTrans = PickedGO[j]->transform->getGlobalMatrix().Transposed();

                    if (mesh->indexCount > 9) {
                        for (size_t b = 0; b < mesh->indexCount; b += 3) {

                            float* t1 = &mesh->vertex[mesh->index[b] * VERTEX];
                            float* t2 = &mesh->vertex[mesh->index[b + 1] * VERTEX];
                            float* t3 = &mesh->vertex[mesh->index[b + 2] * VERTEX];

                            float4 tr1 = matTrans * float4(*t1, *(t1 + 1), *(t1 + 2), 1);
                            float4 tr2 = matTrans * float4(*t2, *(t2 + 1), *(t2 + 2), 1);
                            float4 tr3 = matTrans * float4(*t3, *(t3 + 1), *(t3 + 2), 1);

                            float3 tri1 = float3(tr1.x, tr1.y, tr1.z);
                            float3 tri2 = float3(tr2.x, tr2.y, tr2.z);
                            float3 tri3 = float3(tr3.x, tr3.y, tr3.z);

                            Triangle triangle(tri1, tri2, tri3);

                            if (picking.Intersects(triangle, &currentDist, nullptr))
                            {
                                if (minDist == 0) {
                                    minDist = currentDist;
                                    App->hierarchy->SetGameObject(PickedGO[j]);
                                    continue;
                                }
                                if (currentDist < minDist) {
                                    minDist = currentDist;
                                    App->hierarchy->SetGameObject(PickedGO[j]);
                                }

                            }
                        }
                    }
                }
            }

        }
        
        if (PickedGO.size() == 0) App->hierarchy->SetGameObject(nullptr);
        PickedGO.clear();
    }
     App->editor->DrawGuizmos();
        ImGui::End();
        ImGui::PopStyleVar();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
 }


ImVec2 SceneWindows::NormMousePos(float x, float y, float w, float h, ImVec2 p)
{
	ImVec2 normP;

	normP.x = -1.0 + 2.0 * ((p.x - x) / w);
	normP.y = 1.0 - 2.0 * ((p.y - y) / h);

	return normP;
}