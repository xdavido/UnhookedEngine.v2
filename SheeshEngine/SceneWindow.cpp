#include "SceneWindow.h"
#include "Imgui/imgui.h"
#include "ComponentCamera.h"
#include "ComponentMesh.h"
#include "ModuleAssimpMeshes.h"
#include "ComponentTransform.h"

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

        for (size_t i = 0; i < PickedGO.size(); i++)
        {
            Mesh* m = PickedGO[i]->GetMeshComponent()->mesh;
            float4x4 mat = PickedGO[i]->transform->getGlobalMatrix().Transposed();

            for (size_t j = 0; j < m->indexCount; j += 3)
            {
                
                float* v1 = &m->vertex[m->index[j] * VERTEX];
                float* v2 = &m->vertex[m->index[j + 1] * VERTEX];
                float* v3 = &m->vertex[m->index[j + 2] * VERTEX];

                
                float4 pT1 = mat * float4(*v1, *(v1 + 1), *(v1 + 2), 1);
                float4 pT2 = mat * float4(*v2, *(v2 + 1), *(v2 + 2), 1);
                float4 pT3 = mat * float4(*v3, *(v3 + 1), *(v3 + 2), 1);

                
                float3 _pt1 = float3(pT1.x, pT1.y, pT1.z);
                float3 _pt2 = float3(pT2.x, pT2.y, pT2.z);
                float3 _pt3 = float3(pT3.x, pT3.y, pT3.z);

                
                Triangle triangle(_pt1, _pt2, _pt3);

                
                if (picking.Intersects(triangle, &currentDist, nullptr))
                {
                    
                    if (minDist == 0) {
                        minDist = currentDist;
                        App->hierarchy->SetGameObject(PickedGO[i]);
                        continue;
                    }

                    
                    if (minDist > currentDist) {
                        minDist = currentDist;
                        App->hierarchy->SetGameObject(PickedGO[i]);
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