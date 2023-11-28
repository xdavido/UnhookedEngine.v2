#pragma once
#include "Module.h"
#include "Globals.h"
#include <vector>
#include "Glew/include/glew.h"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <memory>
#include <deque>

class ModuleEditor : public Module
{
public:
    ModuleEditor(Application* app, bool start_enabled = true);
    ~ModuleEditor();

    bool Init();
    

    update_status DrawEditor();

    void ViewCollapsingHeader();

    void HardwareCollapsingHeader();

    bool CleanUp();
    void AddFPS(const float aFPS);
    void AddMs(const float aFPS);
    float AverageValueFloatVector(const std::vector<float>& fps);
    

    std::string ReadFileIO(const char* file);
    void LOGToConsole(const char* text);

    void PushBackLog(std::vector<float>* log, float current);
    
private:

    
    void CreateAboutWindow(bool& showAboutWindow);
    void URLButton(const char* url);
    void CreateConsoleWindow(bool& showConsoleWindow);
    void ModuleEditor::WindowCollapsingHeader();
    void ModuleEditor::RenderCollapsingHeader();
    void UpdatePlots();
    
public:
    std::deque<std::string>* logs = nullptr;


private:
    std::vector<float> mFPSLog;
    std::vector<float> mMsLog;
    
    //Window info
    bool showAboutWindow;
    bool fullscreen;
    bool resizable;
    bool borderless;
    bool fulldesktop;
    float brightness;

    //Renderer info
    bool vsync;
    bool depthTest;
    bool cullFace;
    bool lighting;
    bool coloMaterial;
    bool texture2D;
    bool blend;
    bool alphaTest;
    bool lineSmooth;
    bool pointSmooth;
    bool polygonSmooth;

    bool wireframeMode;
    bool showConsoleWindow;
    int width, height;

    //Libraries Info
    std::string sdlVersion;
    std::string assimpVersion;

    //Hardware info
    float systemRAM;
    int cpuCount;
    int cpuCacheSize;
    bool Now3D;
    bool AltiVec;
    bool AVX;
    bool AVX2;
    bool MMX;
    bool RDTSC;
    bool SSE;
    bool SSE2;
    bool SSE3;
    bool SSE41;
    bool SSE42;

    std::vector<std::string> screens;


    bool isActiveInspector;
    bool isActiveConsole;
    bool isActiveHierarchy;
    bool isActiveViewport;

    //About Window
    std::string license;

};

