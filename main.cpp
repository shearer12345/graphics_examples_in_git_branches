#include <iostream>

#include <SDL.h>

using namespace std;

int main( int argc, char* args[] )
{

	if (SDL_Init(SDL_INIT_EVERYTHING) != 0){
		cout << "SDL_Init Error: " << SDL_GetError() << std::endl;
		return 1;
	}
	cout << "SDL initialised OK!\n";
    
	SDL_Window *win = SDL_CreateWindow("Hello World!", 100, 100, 640, 480, SDL_WINDOW_SHOWN);
	if (win == nullptr)
	{
		std::cout << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
		SDL_Quit();
		return 1;
	}
	cout << "SDL CreatedWindow OK!\n";
    
	SDL_Delay(2000); //Wait for 2 seconds before exiting

	return 0;
}