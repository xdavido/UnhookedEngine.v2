#ifndef __ModuleWindow_H__
#define __ModuleWindow_H__

#include "Module.h"
#include "SDL/include/SDL.h"

class Application;

class ModuleWindow : public Module
{
public:

	ModuleWindow(Application* app, bool start_enabled = true);

	// Destructor
	virtual ~ModuleWindow();

	bool Init();
	bool CleanUp();

	// Setter functions for window attributes
	void SetTitle(const char* title);
	void SetFullscreen(bool newFullscreen);
	void SetResizable(bool newResizable);
	void SetBorderless(bool newBorderless);
	void SetFulldesktop(bool newFulldesktop);

	// Getter functions for window attributes
	bool IsFullscreen() const { return fullscreen; }
	bool IsResizable() const { return resizable; }
	bool IsBorderless() const { return borderless; }
	bool IsFulldesktop() const { return fulldesktop; }

	//"Listeners" to Window Width and Height changes
	void OnWidthChanged();
	void OnHeightChanged();

public:
	//The window we'll be rendering to
	SDL_Window* window;

	//The surface contained by the window
	SDL_Surface* screen_surface;

	int width;
	int height;

private:
	//Window attributes

	bool fullscreen;
	bool resizable;
	bool borderless;
	bool fulldesktop;
};

#endif // __ModuleWindow_H__