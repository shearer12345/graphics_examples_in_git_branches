#include <iostream>
#include <vector>
#include <algorithm>

#include <GL/glew.h>
#include <SDL.h>

using namespace std;

/////////////////////
// global variables

SDL_Window *win; //pointer to the SDL_Window
SDL_GLContext context; //the SDL_GLContext
GLuint theProgram; //GLuint that we'll fill in to refer to the GLSL program (only have 1 at this point)

//string holding the **source** of our vertex shader, to save loading from a file
const std::string strVertexShader(
	"#version 330\n"
	"layout(location = 0) in vec4 position;\n"
	"void main()\n"
	"{\n"
	"   gl_Position = position;\n"
	"}\n"
	);

//string holding the **source** of our fragment shader, to save loading from a file
const std::string strFragmentShader(
	"#version 330\n"
	"out vec4 outputColor;\n"
	"void main()\n"
	"{\n"
	"   outputColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);\n"
	"}\n"
	);

const float vertexPositions[] = {
	0.75f, 0.75f, 0.0f, 1.0f,
	0.75f, -0.75f, 0.0f, 1.0f,
	-0.75f, -0.75f, 0.0f, 1.0f,
};

GLuint positionBufferObject;
GLuint vao;

// end Global Variables
/////////////////////////


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
	win = SDL_CreateWindow("Hello World!", 100, 100, 640, 480, SDL_WINDOW_OPENGL);
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
	context = SDL_GL_CreateContext(win);
	if (context == nullptr){
		SDL_DestroyWindow(win);
		std::cout << "SDL_GL_CreateContext Error: " << SDL_GetError() << std::endl;
		SDL_Quit();
		exit(1);
	}
	cout << "Created OpenGL context OK!\n";
}

void initGlew()
{
	GLenum rev;
	glewExperimental = GL_TRUE; //GLEW isn't perfect - see https://www.opengl.org/wiki/OpenGL_Loading_Library#GLEW
	rev = glewInit();
	if (GLEW_OK != rev){
		std::cout << "GLEW Error: " << glewGetErrorString(rev) << std::endl;
		SDL_Quit();
		exit(1);
	}
	else {
		cout << "GLEW Init OK!\n";
	}
}

GLuint createShader(GLenum eShaderType, const std::string &strShaderFile)
{
	GLuint shader = glCreateShader(eShaderType);
	const char *strFileData = strShaderFile.c_str();
	glShaderSource(shader, 1, &strFileData, NULL);

	glCompileShader(shader);

	GLint status;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
	if (status == GL_FALSE)
	{
		GLint infoLogLength;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLength);

		GLchar *strInfoLog = new GLchar[infoLogLength + 1];
		glGetShaderInfoLog(shader, infoLogLength, NULL, strInfoLog);

		const char *strShaderType = NULL;
		switch (eShaderType)
		{
		case GL_VERTEX_SHADER: strShaderType = "vertex"; break;
		case GL_GEOMETRY_SHADER: strShaderType = "geometry"; break;
		case GL_FRAGMENT_SHADER: strShaderType = "fragment"; break;
		}

		fprintf(stderr, "Compile failure in %s shader:\n%s\n", strShaderType, strInfoLog);
		delete[] strInfoLog;
	}

	return shader;
}

GLuint createProgram(const std::vector<GLuint> &shaderList)
{
	GLuint program = glCreateProgram();

	for (size_t iLoop = 0; iLoop < shaderList.size(); iLoop++)
		glAttachShader(program, shaderList[iLoop]);

	glLinkProgram(program);

	GLint status;
	glGetProgramiv(program, GL_LINK_STATUS, &status);
	if (status == GL_FALSE)
	{
		GLint infoLogLength;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLogLength);

		GLchar *strInfoLog = new GLchar[infoLogLength + 1];
		glGetProgramInfoLog(program, infoLogLength, NULL, strInfoLog);
		fprintf(stderr, "Linker failure: %s\n", strInfoLog);
		delete[] strInfoLog;
	}

	for (size_t iLoop = 0; iLoop < shaderList.size(); iLoop++)
		glDetachShader(program, shaderList[iLoop]);

	return program;
}

void initializeProgram()
{
	std::vector<GLuint> shaderList;

	shaderList.push_back(createShader(GL_VERTEX_SHADER, strVertexShader));
	shaderList.push_back(createShader(GL_FRAGMENT_SHADER, strFragmentShader));

	theProgram = createProgram(shaderList);
	if (theProgram == 0)
	{
		cout << "GLSL program creation error." << std::endl;
		SDL_Quit();
		exit(1);
	}
	else {
		cout << "GLSL program creation OK! GLUint is: " << theProgram << std::endl;
	}

	//clean up shaders (we don't need them anymore as they are no in theProgram
	for_each(shaderList.begin(), shaderList.end(), glDeleteShader);
}

void initializeVertexBuffer()
{
	glGenBuffers(1, &positionBufferObject);

	glBindBuffer(GL_ARRAY_BUFFER, positionBufferObject);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertexPositions), vertexPositions, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	cout << "positionBufferObject created OK! GLUint is: " << positionBufferObject << std::endl;
}

void loadAssets()
{
	initializeProgram(); //create GLSL Shaders, link into a GLSL program

	initializeVertexBuffer(); //load data into a vertex buffer

	glGenVertexArrays(1, &vao); //create a Vertex Array Object
	glBindVertexArray(vao); //make the VAO active
	cout << "Vertex Array Object created OK! GLUint is: " << vao << std::endl;

	cout << "Loaded Assets OK!\n";
}

void cleanUp()
{
	SDL_GL_DeleteContext(context);
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
	initGlew();


	//load stuff from files
	//- usually do just once
	loadAssets();
	
	//clear framebuffer, render, and present the framebuffer to the display
	//- usually loop forever, but not in this example

	//LOOP FROM HERE - PLACEHOLDER

		//GET INPUT HERE - PLACEHOLDER

		//UPDATE SIMULATION - PLACEHOLDER

		glClear(GL_COLOR_BUFFER_BIT);
		//RENDER HERE - PLACEHOLDER

		SDL_GL_SwapWindow(win);; //present the frame buffer to the display (swapBuffers)

	//LOOP TO HERE - PLACEHOLDER


	//not looping, so delay so we can see what it does
	SDL_Delay(2000); //Wait for 2 seconds before exiting

	
	//cleanup and exit
	cleanUp();
	SDL_Quit();

	return 0;
}