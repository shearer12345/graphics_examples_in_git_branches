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
    return 0;
}