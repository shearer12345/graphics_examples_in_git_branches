#include <iostream>

#include <SDL.h>
#include <SDL_opengl.h>

using namespace std;

//global variables

SDL_Window *win; //pointer to the SDL_Window
SDL_Renderer *ren; //pointer to the SDL_Renderer

void initialise()
{
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0){
		cout << "SDL_Init Error: " << SDL_GetError() << std::endl;
		exit(1);
	}
	cout << "SDL initialised OK!\n";
}

void createWindow()
{
	win = SDL_CreateWindow("Hello World!", 100, 100, 640, 480, SDL_WINDOW_SHOWN);
	if (win == nullptr)
	{
		std::cout << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
		SDL_Quit();
		exit(1);
	}
	cout << "SDL CreatedWindow OK!\n";
}

void setGLAttributes()
{
	// set the opengl context version
	int major = 3;
	int minor = 3;
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, major);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, minor);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE); //core profile
	cout << "Set OpenGL context to version " << major << "." << minor << " OK!\n";
}

void createContext()
{
	ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (ren == nullptr){
		SDL_DestroyWindow(win);
		std::cout << "SDL_CreateRenderer Error: " << SDL_GetError() << std::endl;
		SDL_Quit();
		exit(1);
	}
	cout << "Created OpenGL context OK!\n";
}

void loadAssets()
{
	cout << "Loaded Assets OK!\n";
}

void cleanUp()
{
	SDL_DestroyRenderer(ren);
	SDL_DestroyWindow(win);
	cout << "Cleaning up OK!\n";
}

int main( int argc, char* args[] )
{

	//setup
	//- do just once
	initialise();
	createWindow();
	setGLAttributes();
	createContext();

	//load stuff from files
	//- usually do just once
	loadAssets();
	
	
	//clear framebuffer, render, and present the framebuffer to the display
	//- usually loop forever, but not in this example

	//LOOP FROM HERE - PLACEHOLDER

		//GET INPUT HERE - PLACEHOLDER

		//UPDATE SIMULATION - PLACEHOLDER

		SDL_RenderClear(ren); //clear the screen. To what color?
		
		//RENDER HERE - PLACEHOLDER

		SDL_RenderPresent(ren); //present the frame buffer to the display (swapBuffers)

	//LOOP TO HERE - PLACEHOLDER


	//not looping, so delay so we can see what it does
	SDL_Delay(2000); //Wait for 2 seconds before exiting

	
	//cleanup and exit
	cleanUp();
	SDL_Quit();

	return 0;
}