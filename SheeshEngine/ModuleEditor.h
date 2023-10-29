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
    

    void DrawEditor();

    void HardwareCollapsingHeader();

    bool CleanUp();
    void AddFPS(const float aFPS);
    void AddMs(const float aFPS);
    float AverageFPS(const std::vector<float>& fps);

    std::string ReadFileIO(const char* file);
    void LOGToConsole(const char* text);
    
private:

    void CreateAboutModalPopup(bool& showModalAbout);
    void URLButton(const char* url);
    void CreateConsoleWindow(bool& showConsoleWindow);
    void ModuleEditor::WindowCollapsingHeader();
    void ModuleEditor::RenderCollapsingHeader();
    
public:
    std::deque<std::string>* logs = nullptr;


private:
    std::vector<float> mFPSLog;
    std::vector<float> mMsLog;
    
    //Window info
    bool showAboutModal;
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

    //Hardware info
    std::string sdlVersion;
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

    //About Window
    std::string license;

};

