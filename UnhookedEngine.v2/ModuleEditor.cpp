#include "ModuleEditor.h"
#include "Application.h"
#include "ModuleWindow.h"
#include "ModuleRenderer3D.h"
#include <GL/gl.h>
#include "ImGui/imgui.h"
#include "ImGui/backends/imgui_impl_opengl3.h"
#include "ImGui/backends/imgui_impl_sdl2.h"
#include "ImGuismo/ImGuizmo.h"

#include "parson.h"
#include "ModuleWindow.h"
#include "ModuleRenderer3D.h"
#include "ModuleHierarchy.h"
#include "OurPrimitive.h"
#include "ModuleCamera3D.h"
#include "ComponentCamera.h"
#include"ComponentTransform.h"
#include "SceneWindow.h"
#include"GameWindow.h"

#include "Assimp/include/ai_assert.h"
#include "Assimp/include/version.h"

ModuleEditor::ModuleEditor(Application* app, bool start_enabled) : Module(app, start_enabled)
{
    logs = new std::deque<std::string>();

    name = "Editor";
}

ModuleEditor::~ModuleEditor()
{
}

bool ModuleEditor::Init()
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
    

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsClassic();

    // Setup Platform/Renderer backends
    ImGui_ImplSDL2_InitForOpenGL(App->window->window, App->renderer3D->context);
    ImGui_ImplOpenGL3_Init();

    mFPSLog.reserve(30);
    mMsLog.reserve(30);

    //Editor attributes Initialization
    showConsoleWindow = false;
    showAboutWindow = false;
    wireframeMode = false;

    //Window info
    //fullscreen = App->window->IsFullscreen();
    //resizable = App->window->IsResizable();
    //borderless = App->window->IsBorderless();
    //fulldesktop = App->window->IsFulldesktop();
    //brightness = SDL_GetWindowBrightness(App->window->window);
    
    //Renderer info
    //vsync = App->renderer3D->GetVsync();
    //depthTest = App->renderer3D->GetDepthTestAttribute();
    //cullFace = App->renderer3D->GetCullFaceAttribute();
    //lighting = App->renderer3D->GetLightingAttribute();
    //coloMaterial = App->renderer3D->GetColorMaterialAttribute();
    //texture2D = App->renderer3D->GetTexture2DAttribute();
    //blend = App->renderer3D->GetBlendAttribute();
    //alphaTest = App->renderer3D->GetAlphaTestAttribute();
    //lineSmooth = App->renderer3D->GetLineSmoothAttribute();
    //pointSmooth = App->renderer3D->GetPointSmoothAttribute();
    //polygonSmooth = App->renderer3D->GetPolygonSmoothAttribute();

    //Hardware Info
    SDL_version versionSDL;
    SDL_GetVersion(&versionSDL);
   
    sdlVersion = std::string("SDL Version: ") +
                 std::to_string(versionSDL.major) + "." +
                 std::to_string(versionSDL.minor) + "." +
                 std::to_string(versionSDL.patch);

    assimpVersion = std::to_string(aiGetVersionMajor()) + '.' + std::to_string(aiGetVersionMinor()) + '.' + std::to_string(aiGetVersionRevision());

    systemRAM = (float) SDL_GetSystemRAM() / (1024.f);
    cpuCount = SDL_GetCPUCount();
    cpuCacheSize = SDL_GetCPUCacheLineSize();

    int numScreens = SDL_GetNumVideoDisplays();
    if (numScreens > 0)
    {
        for (int i = 0; i < numScreens; ++i) {
            SDL_DisplayMode displayMode;
            if (SDL_GetCurrentDisplayMode(i, &displayMode) == 0) {
                screens.push_back(SDL_GetDisplayName(i));
            }
        }
    }

    Now3D = SDL_Has3DNow();
    AltiVec = SDL_HasAltiVec();
    AVX = SDL_HasAVX();
    AVX2 = SDL_HasAVX2();
    MMX = SDL_HasMMX();
    RDTSC = SDL_HasRDTSC();
    SSE = SDL_HasSSE();
    SSE2 = SDL_HasSSE2();
    SSE3 = SDL_HasSSE3();
    SSE41 = SDL_HasSSE41();
    SSE42 = SDL_HasSSE42();

    //About Window
    license = ReadFileIO("../../License.txt");

    isActiveHierarchy = true;
    isActiveConsole = true;
    isActiveInspector = true;

    return true;
}



bool ModuleEditor::CleanUp()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    RELEASE(logs);


    return true;
}


update_status ModuleEditor::DrawEditor()
{
    update_status ret = update_status::UPDATE_CONTINUE;
    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();
    ImGuizmo::BeginFrame();
    ImGuizmo::Enable(true);
    EditorShortcuts();

    
    ImGuiWindowFlags flags = ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoNavFocus;

    const ImGuiViewport* main_viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(main_viewport->WorkPos);
    ImGui::SetNextWindowSize(main_viewport->Size);
    ImGui::SetNextWindowViewport(main_viewport->ID);

    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

    ImGui::Begin("DockingInv", nullptr, flags);

    ImGui::PopStyleVar(3);

    ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
    ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_PassthruCentralNode);
    /*ImGui::DockSpaceOverViewport(NULL, ImGuiDockNodeFlags_PassthruCentralNode);*/
    ImGui::End();

    UpdatePlots();

    ImGuiIO& io = ImGui::GetIO();

    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("Quit Application", "ESC")) {
                ret = UPDATE_STOP;
            }

            if (ImGui::MenuItem("Save Scene", "Ctrl+S"))
            {
                App->scene->SaveSceneRequest();
            }

            if (ImGui::MenuItem("Load Scene"))
            {
                App->scene->LoadSceneRequest();
            }

            ImGui::EndMenu();
            
        }
        if (ImGui::BeginMenu("Game Objects"))
        {

            if (ImGui::MenuItem(" EMPTY  ")) {

                PrimitivesGeomtriesLibrary::InstanciatePrimitiveGeometry(GeometryType::EMPTY);
            }
            if (ImGui::MenuItem(" CAMERA  ")) {

                PrimitivesGeomtriesLibrary::InstanciatePrimitiveGeometry(GeometryType::CAMERA);
            }
            if (ImGui::MenuItem(" CUBE  ")) {
                
                PrimitivesGeomtriesLibrary::InstanciatePrimitiveGeometry(GeometryType::P_CUBE);
            }

            if (ImGui::MenuItem(" PLANE "))
            {
                PrimitivesGeomtriesLibrary::InstanciatePrimitiveGeometry(GeometryType::P_PLANE);
            }

            if (ImGui::MenuItem(" SPHERE  "))
            {
                PrimitivesGeomtriesLibrary::InstanciatePrimitiveGeometry(GeometryType::P_SPHERE);
            }

            if (ImGui::MenuItem(" ICOSPHERE  "))
            {
                PrimitivesGeomtriesLibrary::InstanciatePrimitiveGeometry(GeometryType::P_ICOSHPERE);
            }

            if (ImGui::MenuItem(" CONE  "))
            {
                PrimitivesGeomtriesLibrary::InstanciatePrimitiveGeometry(GeometryType::P_CONE);
            }

            if (ImGui::MenuItem(" CYLINDER  "))
            {
                PrimitivesGeomtriesLibrary::InstanciatePrimitiveGeometry(GeometryType::P_CYLINDER);
            }

            if (ImGui::MenuItem(" TORUS  "))
            {
                PrimitivesGeomtriesLibrary::InstanciatePrimitiveGeometry(GeometryType::P_TORUS);
            }

            if (ImGui::MenuItem(" SUZANNE  "))
            {
                PrimitivesGeomtriesLibrary::InstanciatePrimitiveGeometry(GeometryType::P_SUZANNE);
            }
            if (ImGui::MenuItem(" PATRICLE EMMITER  "))
            {
                GameObject* b = new GameObject(App->scene->root);
                b->name = "Someke 1";
                b->transform->position = float3(0.0f, 5.0f, 0.0f);
                b->transform->calculateMatrix();
                CEmitter* emitter = new CEmitter(b);
                emitter->textureID = App->textures->LoadParticleTexture("Assets/VFX/smokeTexture.png");
                emitter->RefreshParticleText();
                b->mComponents.push_back(emitter);
            }

            ImGui::EndMenu();
        }
     

        if (ImGui::BeginMenu("Configuration"))
        {
            if (ImGui::Button("Save"))App->SaveConfigRequest();
            ImGui::SameLine();
            if (ImGui::Button("Load"))App->LoadConfigRequest();

            if (ImGui::CollapsingHeader(" Camera Options "))
            {
                const char* listType[]{ "Perspective", "Orthographic" };

                ImGui::LabelText("##Scene mesh", "Rendering Meshes:");
                ImGui::SameLine();
                ImGui::Text("%d", App->assimpMeshes->renderedSceneMeshes);

                ImGui::Text(" Camera type: ");
                ImGui::SameLine();
                if (ImGui::Combo("##CameraType", &App->camera->camera->typeCameraSelected, listType, IM_ARRAYSIZE(listType)))
                {
                    if (App->camera->camera->typeCameraSelected == 0)
                        App->camera->camera->frustum.type = PerspectiveFrustum;

                    if (App->camera->camera->typeCameraSelected == 1)
                        App->camera->camera->frustum.type = OrthographicFrustum;

                }

                ImGui::Text("");

                //Fov camera
                ImGui::Text(" FOV\t\t  ");
                ImGui::SameLine();
                if (ImGui::SliderInt("##FOVert", &App->camera->camera->cameraFOV, 10, 200))
                {
                    App->camera->camera->frustum.verticalFov = App->camera->camera->cameraFOV * DEGTORAD;
                    App->camera->camera->frustum.horizontalFov = 2.0f * atanf(tanf(App->camera->camera->frustum.verticalFov / 2.0f) * 1.7f);
                    App->renderer3D->OnResize(SCREEN_WIDTH, SCREEN_HEIGHT);
                }

                ImGui::Text("");

                //Slider Set Near Distane
                ImGui::Text(" Near Distance\t");
                ImGui::SameLine();
                if (ImGui::InputFloat("##nearDistance", &App->camera->camera->nearDistance))
                {
                    if (App->camera->camera->nearDistance >= App->camera->camera->farDistance)
                    {
                        App->camera->camera->farDistance = App->camera->camera->nearDistance + 1;
                        App->camera->camera->frustum.farPlaneDistance = App->camera->camera->farDistance;
                    }

                    App->camera->camera->frustum.nearPlaneDistance = App->camera->camera->nearDistance;
                    App->renderer3D->OnResize(SCREEN_WIDTH, SCREEN_HEIGHT);
                }

                ImGui::Text("");

                //Slider Set Far Distane
                ImGui::Text(" Far Distance\t ");
                ImGui::SameLine();
                if (ImGui::InputFloat("##farDistance", &App->camera->camera->farDistance))
                {
                    if (App->camera->camera->farDistance <= App->camera->camera->nearDistance)
                    {
                        App->camera->camera->nearDistance = App->camera->camera->farDistance - 1;
                        App->camera->camera->frustum.nearPlaneDistance = App->camera->camera->nearDistance;
                    }

                    App->camera->camera->frustum.farPlaneDistance = App->camera->camera->farDistance;
                    App->renderer3D->OnResize(SCREEN_WIDTH, SCREEN_HEIGHT);
                }
                
            }

            if (ImGui::Checkbox("Vsync", &vsync))
            {
                App->renderer3D->SetVsync(vsync);
            }

            std::string fps = std::to_string(AverageValueFloatVector(mFPSLog));
            std::string fpsText = "Average FPS: " + fps;
            ImGui::Text(fpsText.c_str());
            ImGui::PlotHistogram("FPS", &mFPSLog[0], mFPSLog.size(), 0, "FPS", 0.0f, 100.0f, ImVec2(450, 100));


            std::string ms = std::to_string(AverageValueFloatVector(mMsLog));
            std::string msText = "Average Miliseconds: " + ms;
            ImGui::Text(msText.c_str());
            ImGui::PlotHistogram("Ms.", &mMsLog[0], mMsLog.size(), 0, "Miliseconds", 0.0f, 100.0f, ImVec2(450, 100));


            WindowCollapsingHeader();

            RenderCollapsingHeader();
            
            if (ImGui::CollapsingHeader("Inputs"))
            {
                if (ImGui::IsMousePosValid())
                    ImGui::Text("Mouse pos: (%g, %g)", io.MousePos.x, io.MousePos.y);
                else
                    ImGui::Text("Mouse pos: <INVALID>");

                ImGui::Text("Mouse down:");
                for (int i = 0; i < IM_ARRAYSIZE(io.MouseDown); i++) if (ImGui::IsMouseDown(i)) { ImGui::SameLine(); ImGui::Text("b%d (%.02f secs)", i, io.MouseDownDuration[i]); }
                ImGui::Text("Mouse wheel: %.1f", io.MouseWheel);
                
                ImGuiKey start_key = (ImGuiKey)0;
                struct funcs { static bool IsLegacyNativeDupe(ImGuiKey key) { return key < 512 && ImGui::GetIO().KeyMap[key] != -1; } }; 
                ImGui::Text("Keys down:");         for (ImGuiKey key = start_key; key < ImGuiKey_NamedKey_END; key = (ImGuiKey)(key + 1)) { if (funcs::IsLegacyNativeDupe(key) || !ImGui::IsKeyDown(key)) continue; ImGui::SameLine(); ImGui::Text((key < ImGuiKey_NamedKey_BEGIN) ? "\"%s\"" : "\"%s\" %d", ImGui::GetKeyName(key), key); }

                
            }

            HardwareCollapsingHeader();

            

            if (ImGui::Button("Close", ImVec2(60, 0)))
            {
                ImGui::CloseCurrentPopup();
            }
            ImGui::End();
        }

        if (ImGui::BeginMenu("Help"))
        {
            if (ImGui::MenuItem("About..."))
            {
                showAboutWindow = true;
            }

            ImGui::EndMenu();
        }

        //CreateAboutModalPopup(showModalAbout);
        CreateAboutWindow(showAboutWindow);
        CreateConsoleWindow(isActiveConsole);

        ViewCollapsingHeader();

        ImGui::SameLine(ImGui::GetWindowWidth() / 2 - 37);
        {
            if (ImGui::Button(">", ImVec2(20, 20)))
            {
                if (App->IsRunning()) {
                    App->SetState(GameState::STOP);
                    ImGui::SetWindowFocus("Scene");
                }
                else {
                    App->SetState(GameState::PLAY);
                    ImGui::SetWindowFocus("Game");
                }
                LOG("Play");
                //App->SetGameDT();
            }
            ImGui::SameLine();
            if (ImGui::Button("[]", ImVec2(20, 20)))
            {
                if (App->IsRunning()) {
                    App->SetState(GameState::STOP);
                    ImGui::SetWindowFocus("Scene");
                }
                LOG("Stop");
                //App->StopGameDT();
            }
            ImGui::SameLine();
            if (ImGui::Button("||", ImVec2(20, 20)))
            {
                if (App->IsRunning()) {
                    App->SetState(GameState::PAUSE);
                }
                LOG("Pause");
                //App->PauseGameDT();
            }
        }

        ImGui::EndMainMenuBar();
    }



    if (App->hierarchy->objSelected && isActiveInspector) {

        if (ImGui::Begin("Inspector")) {
            
            App->hierarchy->objSelected->PrintInspector();
        }
        ImGui::End();
    }

  

    if (isActiveHierarchy) {
        if (ImGui::Begin("GameObjects Hierarchy")) {

            App->hierarchy->GameObjectTree(App->scene->root, 0);


        }
        ImGui::End();
    }

    GameWindows::PrintCamera(App);
    SceneWindows::PrintScene(App);
    UpdateAtributes();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    return ret;

    
}

void ModuleEditor::EditorShortcuts()
{
    if (App->input->GetKey(SDL_SCANCODE_LCTRL) == KEY_REPEAT && App->input->GetKey(SDL_SCANCODE_S) == KEY_UP)
    {
        App->scene->SaveSceneRequest();
    }
}

void ModuleEditor::ViewCollapsingHeader() {

    if (ImGui::BeginMenu("View")) {

        if (ImGui::Checkbox("Hierarchy", &isActiveHierarchy))
        {
            isActiveHierarchy != isActiveHierarchy;
        }
        if (ImGui::Checkbox("Inspector", &isActiveInspector))
        {
            isActiveInspector != isActiveInspector;
        }
        if (ImGui::Checkbox("Console", &isActiveConsole))
        {
            isActiveConsole != isActiveConsole;
        }

        ImGui::EndMenu();
    }
}

void ModuleEditor::HardwareCollapsingHeader()
{
    if (ImGui::CollapsingHeader("Hardware"))
    {
        ImGui::Text(sdlVersion.c_str());
        ImGui::Separator();
        
        ImGui::Text("CPUs ");
        ImGui::SameLine();
        ImGui::Text("%u", cpuCount);
        ImGui::SameLine();
        ImGui::Text(" (Cache: %u", cpuCacheSize);
        ImGui::SameLine();
        ImGui::Text("kb)\n");


        ImGui::Text("System RAM: ");
        ImGui::SameLine();
        ImGui::TextColored(IMGUICOL_SKY_BLUE, "%.2f", systemRAM);
        ImGui::SameLine();
        ImGui::TextColored(IMGUICOL_SKY_BLUE, "Gb");

        ImGui::Separator();
        ImGui::Text("Sceen Devices:");

        for (const std::string& screen : screens) {
            ImGui::Text("%s", screen.c_str());
        }

        ImGui::Text("Caps: ");

        if (Now3D) { ImGui::SameLine(); ImGui::TextColored(IMGUICOL_SKY_BLUE, "3DNOW,"); }
        if (AltiVec) { ImGui::SameLine(); ImGui::TextColored(IMGUICOL_SKY_BLUE, "AltiVec,"); }
        if (AVX) { ImGui::SameLine(); ImGui::TextColored(IMGUICOL_SKY_BLUE, "AVX,"); }
        if (AVX2) { ImGui::SameLine(); ImGui::TextColored(IMGUICOL_SKY_BLUE, "AVX2,"); }
        if (MMX) { ImGui::SameLine(); ImGui::TextColored(IMGUICOL_SKY_BLUE, "MMX,"); }
        if (RDTSC) { ImGui::SameLine(); ImGui::TextColored(IMGUICOL_SKY_BLUE, "RDTSC,"); }
        if (SSE) { ImGui::SameLine(); ImGui::TextColored(IMGUICOL_SKY_BLUE, "SSE,"); }
        if (SSE2) { ImGui::SameLine(); ImGui::TextColored(IMGUICOL_SKY_BLUE, "SSE2,"); }
        if (SSE3) { ImGui::SameLine(); ImGui::TextColored(IMGUICOL_SKY_BLUE, "SSE3,"); }
        if (SSE41) { ImGui::SameLine(); ImGui::TextColored(IMGUICOL_SKY_BLUE, "SSE41,"); }
        if (SSE42) { ImGui::SameLine(); ImGui::TextColored(IMGUICOL_SKY_BLUE, "SSE42"); }

    }
}

void ModuleEditor::WindowCollapsingHeader()
{
    if (ImGui::CollapsingHeader("Window"))
    {
        if (ImGui::BeginTable("split", 3))
        {
            int oldWidth = App->window->width;
            int oldHeight = App->window->height;

            ImGui::TableNextColumn();
            if (ImGui::Checkbox("Fullscreen", &fullscreen))
            {
                App->window->SetFullscreen(fullscreen);
            }

            ImGui::TableNextColumn();
            if (ImGui::Checkbox("Resizable", &resizable))
            {
                App->window->SetResizable(resizable);
            }

            ImGui::TableNextColumn();
            ImGui::SliderInt("Window Width", &App->window->width, 320, 2560);
            
            ImGui::TableNextColumn();
            if (ImGui::Checkbox("Borderless", &borderless))
            {
                App->window->SetBorderless(borderless);
            }
            
            ImGui::TableNextColumn();
            if (ImGui::Checkbox("Full Desktop", &fulldesktop))
            {
                App->window->SetFulldesktop(fulldesktop);
            }

   
            ImGui::TableNextColumn();
            ImGui::SliderInt("Window Height", &App->window->height, 256, 2048);

            if (oldWidth != App->window->width) { App->window->OnWidthChanged(); }
            if (oldHeight != App->window->height) { App->window->OnHeightChanged(); }

            ImGui::Spacing();

            if (ImGui::SliderFloat("Brightness", &brightness, 0.100f, 1.000f))
            {
                SDL_SetWindowBrightness(App->window->window, brightness);
                
            }
            ImGui::EndTable();
        }
    }
}

void ModuleEditor::RenderCollapsingHeader()
{
    if (ImGui::CollapsingHeader("Renderer"))
    {
        if (ImGui::BeginTable("split", 3))
        {
            ImGui::TableNextColumn();
            if (ImGui::Checkbox("GL_DEPTH_TEST", &depthTest))
            {
                App->renderer3D->SetDepthTestAttribute(depthTest);
            }

            ImGui::TableNextColumn();
            if (ImGui::Checkbox("GL_CULL_FACE", &cullFace))
            {
                App->renderer3D->SetCullFaceAttribute(cullFace);
            }

            ImGui::TableNextColumn();
            if (ImGui::Checkbox("GL_LIGHTING", &lighting))
            {
                App->renderer3D->SetLightingAttribute(lighting);
            }

            ImGui::Spacing();

            ImGui::TableNextColumn();
            if (ImGui::Checkbox("GL_COLOR_MATERIAL", &coloMaterial))
            {
                App->renderer3D->SetColorMaterialAttribute(coloMaterial);
            }

            ImGui::TableNextColumn();
            if (ImGui::Checkbox("GL_TEXTURE_2D", &texture2D))
            {
                App->renderer3D->SetTexture2DAttribute(texture2D);
            }

            ImGui::TableNextColumn();
            if (ImGui::Checkbox("GL_BLEND", &blend))
            {
                App->renderer3D->SetBlendAttribute(blend);
            }

            ImGui::Spacing();

            ImGui::TableNextColumn();
            if (ImGui::Checkbox("GL_ALPHA_TEST", &alphaTest))
            {
                App->renderer3D->SetAlphaTestAttribute(alphaTest);
            }

            ImGui::TableNextColumn();
            if (ImGui::Checkbox("GL_LINE_SMOOTH", &lineSmooth))
            {
                App->renderer3D->SetLineSmoothAttribute(lineSmooth);
            }

            ImGui::TableNextColumn();
            if (ImGui::Checkbox("GL_POINT_SMOOTH", &pointSmooth))
            {
                App->renderer3D->SetPointSmoothAttribute(pointSmooth);
            }

            ImGui::TableNextColumn();
            if (ImGui::Checkbox("GL_POLYGON_SMOOTH", &polygonSmooth))
            {
                App->renderer3D->SetPolygonSmoothAttribute(polygonSmooth);
            }

            ImGui::TableNextColumn();
            if (ImGui::Checkbox("WIREFRAME_MODE", &wireframeMode))
            {
                App->renderer3D->SetWireframeMode(wireframeMode);
            }

            ImGui::Spacing();

   
            ImGui::EndTable();
        }
    }
}

void ModuleEditor::CreateAboutWindow(bool& showAboutWindow)
{
    if (!showAboutWindow)
        return;

    ImGui::SetNextWindowSize(ImVec2(920, 300), ImGuiCond_FirstUseEver);
    if (!ImGui::Begin("About", &showAboutWindow))
    {
        showAboutWindow = true;
        ImGui::End();
        return;
    }

    // Basic Info
    ImGui::Text("Unhooked Engine");
    ImGui::Separator();
    ImGui::Spacing();
    ImGui::Text("Venture into the realm of game development with the legendary Unhooked Engine, \na creation born from the collaborative efforts of two visionary minds at CITM.");
    ImGui::Text("Whether you seek to forge epic tales of heroism or weave enchanting mysteries,\nthis engine is your magical wand.");
    ImGui::Spacing();
    if (ImGui::Button("Autors: David Ruiz Luengo & Pau Garriga Brotons"))
    {
        URLButton("https://github.com/xdavido/UnhookedEngine.v2");
    }

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    // Libraries
    ImGui::Text("Libraries and links to their websites:");

    if (ImGui::Button("SDL"))
    {
        URLButton("https://libsdl.org/index.php");
    }
    ImGui::SameLine();
    ImGui::Text(sdlVersion.c_str());

    if (ImGui::Button("OpenGL"))
    {
        URLButton("https://www.opengl.org/");
    }
    ImGui::SameLine();
    ImGui::Text("OpenGL: %s", glewGetString(GLEW_VERSION));

    if (ImGui::Button("ImGui"))
    {
        URLButton("https://github.com/ocornut/imgui/");
    }
    ImGui::SameLine();
    ImGui::Text(ImGui::GetVersion());

    if (ImGui::Button("Glew"))
    {
        URLButton("http://glew.sourceforge.net/");
    }
    ImGui::SameLine();
    ImGui::Text((const char*)glewGetString(GLEW_VERSION));

    // Phys
    // Assimp
    if (ImGui::Button("Assimp"))
    {
        URLButton("https://www.assimp.org/");
    }
    ImGui::SameLine();
    ImGui::Text(assimpVersion.c_str());

    
    //Devil
    if (ImGui::Button("DevIL"))
    {
        URLButton("http://openil.sourceforge.net/");
    }
    ImGui::SameLine();
    ImGui::Text("v1.8.0");

    ImGui::Separator();
    ImGui::Spacing();

    // Licence
    ImGui::Text(license.c_str());

    // Close Button
    if (ImGui::Button("Close", ImVec2(60, 0)))
    {
        showAboutWindow = false;
    }

    ImGui::End();



}

void ModuleEditor::CreateConsoleWindow(bool& showConsoleWindow)
{
    if (!showConsoleWindow)
        return;

    bool AutoScroll = false;
    ImGuiTextFilter Filter;

    ImGui::SetNextWindowSize(ImVec2(920, 300), ImGuiCond_FirstUseEver);
    if (!ImGui::Begin("Console", &showConsoleWindow))
    {
        showConsoleWindow = true;
        ImGui::End();
        return;
    }

    if (ImGui::SmallButton("Clear")) { logs->clear(); }

    ImGui::Separator();
    ImGui::Spacing();

    // Options menu
    if (ImGui::BeginPopup("Options"))
    {
        ImGui::Checkbox("Auto-scroll", &AutoScroll);
        ImGui::EndPopup();
    }

    //Texto    
    for (std::string& str : *(logs))
    {
        ImGui::Text(str.c_str());
    }    

    ImGui::End();
}


void ModuleEditor::URLButton(const char* url)
{
    
#ifdef _WIN32
    ShellExecuteA(NULL, "open", url, NULL, NULL, SW_SHOWNORMAL);
#elif defined(__APPLE__)
    std::string command = "open " + url;
    system(command.c_str());
#elif defined(__linux__)
    std::string command = "xdg-open " + url;
    system(command.c_str());
#else

    ImGui::Text("Sistema operativo no compatible");
#endif
}

void ModuleEditor::UpdatePlots()
{
    AddMs(1000.0 * App->GetDt());
    AddFPS(App->GetFrameRate());
    
}


void ModuleEditor::AddFPS(const float aFPS)
{

    if (mFPSLog.size() == mFPSLog.capacity())
    {

        for (unsigned int i = 0; i < mFPSLog.size(); i++)
        {
            if (i + 1 < mFPSLog.size())
            {
                float iCopy = mFPSLog[i + 1];
                mFPSLog[i] = iCopy;
            }
        }
        mFPSLog[mFPSLog.capacity() - 1] = aFPS;
        
    }
    else
    {
        mFPSLog.push_back(aFPS);
    }
}

void ModuleEditor::AddMs(const float aFPS)
{

    if (mMsLog.size() == mMsLog.capacity())
    {

        for (unsigned int i = 0; i < mMsLog.size(); i++)
        {
            if (i + 1 < mMsLog.size())
            {
                float iCopy = mMsLog[i + 1];
                mMsLog[i] = iCopy;
            }
        }
        mMsLog[mMsLog.capacity() - 1] = aFPS;

    }
    else
    {
        mMsLog.push_back(aFPS);
    }
}


std::string ModuleEditor::ReadFileIO(const char* filePath)
{
    std::ifstream file(filePath); 
    std::string contenido; 

    if (file.is_open()) {
        std::string linea;
        while (std::getline(file, linea)) {
            contenido += linea; 
            contenido += '\n'; 
        }
        file.close(); 
    }
    else {
        return std::string();
    }

    return contenido;
}

float ModuleEditor::AverageValueFloatVector(const std::vector<float>& fps)
{
    if (fps.empty()) {
        return std::numeric_limits<float>::quiet_NaN();
    }

    float total = 0.0f;
    for (float value : fps) {
        total += value;
    }

    float average = total / fps.size();
    return std::round(average * 10) / 10.0f;
}

void ModuleEditor::UpdateAtributes()
{
    App->renderer3D->SetVsync(vsync);
  

    App->renderer3D->SetDepthTestAttribute(depthTest);
    App->renderer3D->SetCullFaceAttribute(cullFace);
    App->renderer3D->SetLightingAttribute(lighting);
    App->renderer3D->SetColorMaterialAttribute(coloMaterial);
    App->renderer3D->SetTexture2DAttribute(texture2D);
    App->renderer3D->SetBlendAttribute(blend);
    App->renderer3D->SetAlphaTestAttribute(alphaTest);
    App->renderer3D->SetLineSmoothAttribute(lineSmooth);
    App->renderer3D->SetPointSmoothAttribute(pointSmooth);
    App->renderer3D->SetPolygonSmoothAttribute(polygonSmooth);

    if (wireframeMode)
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
       
    }
    else
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
       
    }

}



void ModuleEditor::LOGToConsole(const char* text) {

    if (logs == nullptr) return;

    
    std::time_t tiempoActual = std::time(nullptr);

    
    struct std::tm* tiempoDescompuesto = std::localtime(&tiempoActual);

    
    int horas = tiempoDescompuesto->tm_hour;
    int minutos = tiempoDescompuesto->tm_min;
    int segundos = tiempoDescompuesto->tm_sec;

    char horaFormateada[100];
    std::strftime(horaFormateada, sizeof(horaFormateada), "%H:%M:%S ", tiempoDescompuesto);

    std::string aux = horaFormateada;
    aux += text;

    logs->push_front(aux);

}

void ModuleEditor::DrawGuizmos()
{
    if (App->hierarchy->objSelected == nullptr) return;
    ComponentTransform* transform = App->hierarchy->objSelected->GetTransformComponent();
    if (transform == nullptr) return;


    ImGuizmo::SetDrawlist();

    float x = ImGui::GetWindowPos().x;
    float y = ImGui::GetWindowPos().y;
    float w = sizeWindScn.x;
    float h = sizeWindScn.y;
    //Guizmo

    float4x4 aux = transform->getGlobalMatrix();

    ImGuizmo::MODE finalMode = (App->camera->operation == ImGuizmo::OPERATION::SCALE ? ImGuizmo::MODE::LOCAL : App->camera->mode);

    ImGuizmo::SetRect(x, y, w, h);
    if (ImGuizmo::Manipulate(App->camera->camera->GetViewMatrix(), App->camera->camera->GetProjetionMatrix(), App->camera->operation, finalMode, &aux.v[0][0]))
    {
        aux.Transpose();
        transform->SetTransformMatrixW(aux);
    }


    if (ImGuizmo::IsOver())
    {

    }
}




