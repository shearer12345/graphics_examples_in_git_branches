#include <iostream>
#include <vector>
#include <algorithm>

#include <GL/glew.h>
#include <SDL.h>

#define GLM_FORCE_RADIANS //force glm to use radians //must do **before** including GLM headers
//NOTE: GLSL uses radians, so will do the same, for consistency

#include <glm/glm.hpp> //include the main glm header
#include <glm/gtc/matrix_transform.hpp> //include functions to ease the calculation of the view and projection matrices
#include <glm/gtc/type_ptr.hpp> //include functionality for converting a matrix object into a float array for usage in OpenGL


#include "cubeWithColorAndNormals.h"

using namespace std;

/////////////////////
// global variables

std::string exeName;
SDL_Window *win; //pointer to the SDL_Window
SDL_GLContext context; //the SDL_GLContext


//string holding the **source** of our vertex shader, to save loading from a file
const std::string strVertexShader(
#ifdef OPENGL_VERSION_3_1
	"#version 140\n"
#endif
#ifdef OPENGL_VERSION_3_3
	"#version 140\n"
#endif
	"in vec4 position;\n"
	"in vec4 color;\n"
	"in vec2 normal;\n"
	"uniform mat4 rotateMatrix;\n"
	"smooth out vec4 theColor;\n"
	"void main()\n"
	"{\n"
	"   gl_Position = rotateMatrix * position;\n" //multiple the position by the transformation matrix (rotate)
	"   theColor = color;\n" //just pass on the color. It's a **smooth**, so will be interpolated
	"}\n"
	);

//string holding the **source** of our fragment shader, to save loading from a file
const std::string strFragmentShader(
#ifdef OPENGL_VERSION_3_1
	"#version 140\n"
#endif
#ifdef OPENGL_VERSION_3_3
	"#version 140\n"
#endif
	"smooth in vec4 theColor;\n"
	"out vec4 outputColor;\n"
	"void main()\n"
	"{\n"
	"   outputColor = theColor;\n"
    "}\n"
	);


//our variables
bool done = false;

//the rotate we'll pass to the GLSL
glm::mat4 rotateMatrix; // the transformation matrix for our object - which is the identity matrix by default
float rotateSpeed = 1.0f; //rate of change of the rotate - in radians per second


//our GL and GLSL variables

GLuint theProgram; //GLuint that we'll fill in to refer to the GLSL program (only have 1 at this point)
GLint positionLocation; //GLuint that we'll fill in with the location of the `position` attribute in the GLSL
GLint colorLocation; //GLuint that we'll fill in with the location of the `color` attribute in the GLSL
GLint normalLocation;

GLint rotateMatrixLocation; //GLuint that we'll fill in with the location of the `rotateMatrix` variable in the GLSL
GLint textureSamplerLocation; 

GLuint vertexBufferObject;
GLuint vao;

GLuint textureID;

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
	//get executable name, and use as window title
	int beginIdxWindows = exeName.rfind("\\"); //find last occurrence of a backslash
	int beginIdxLinux = exeName.rfind("/"); //find last occurrence of a forward slash
	int beginIdx = max(beginIdxWindows, beginIdxLinux);
	std::string exeNameEnd = exeName.substr(beginIdx + 1);
	const char *exeNameCStr = exeNameEnd.c_str();

	//create window
	win = SDL_CreateWindow(exeNameCStr, 100, 100, 600, 600, SDL_WINDOW_OPENGL); //same height and width makes the window square ...

	//error handling
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
	#ifdef OPENGL_VERSION_3_1
		cout << "Built for OpenGL Version 3.1" << endl;
		// set the opengl context version
		int major = 3;
		int minor = 1;
	#endif
	#ifdef OPENGL_VERSION_3_3
		cout << "Built for OpenGL Version 3.3" << endl;
		// set the opengl context version
		int major = 3;
		int minor = 3;
	#endif
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

	positionLocation = glGetAttribLocation(theProgram, "position");
	colorLocation = glGetAttribLocation(theProgram, "color");
	normalLocation = glGetAttribLocation(theProgram, "normal");

	//Error check Attributes
	if (positionLocation < 0 || colorLocation < 0) //TODO not checking normalLocation yet - it is optimized out at this point
	{
		cout << "GLSL getAttributeLocation failed." << std::endl;
		cout << "positionLocation= " << positionLocation << std::endl;
		cout << "colorLocation= " << colorLocation << std::endl;
		cout << "normalLocation= " << normalLocation << std::endl;
		SDL_Quit();
		exit(1);
	}
	
	rotateMatrixLocation = glGetUniformLocation(theProgram, "rotateMatrix");
	
	//Error check Uniforms
	if (rotateMatrixLocation < 0 || textureSamplerLocation < 0)
	{
		cout << "GLSL getUniformeLocation failed." << std::endl;
		cout << "rotateMatrixLocation= " << rotateMatrixLocation << std::endl;
		SDL_Quit();
		exit(1);
	}


	//clean up shaders (we don't need them anymore as they are no in theProgram
	for_each(shaderList.begin(), shaderList.end(), glDeleteShader);
}

void initializeVertexBuffer()
{
	glGenBuffers(1, &vertexBufferObject);

	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeWithColorAndNormals), cubeWithColorAndNormals, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	cout << "positionBufferObject created OK! GLUint is: " << vertexBufferObject << std::endl;
}

void initializeTexturesAndSamplers()
{
	SDL_Surface* image = SDL_LoadBMP("assets/hello.bmp");
	if (image == NULL)
	{
		cout << "iamge loading (for texture) failed." << std::endl;
		SDL_Quit();
		exit(1);
	}

	glEnable(GL_TEXTURE_2D); //enable 2D texturing
	glGenTextures(1, &textureID); //generate a texture ID and store it
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, image->format->BytesPerPixel, image->w, image->h, 0, GL_RGB, GL_UNSIGNED_BYTE, image->pixels);

	glBindTexture(GL_TEXTURE_2D, 0);
	SDL_FreeSurface(image);


	cout << "texture created OK! GLUint is: " << textureID << std::endl;

}


void loadAssets()
{
	initializeProgram(); //create GLSL Shaders, link into a GLSL program

	initializeVertexBuffer(); //load data into a vertex buffer

	initializeTexturesAndSamplers();

	glGenVertexArrays(1, &vao); //create a Vertex Array Object
	glBindVertexArray(vao); //make the VAO active
	cout << "Vertex Array Object created OK! GLUint is: " << vao << std::endl;

	//setup depth testing
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	//setup face culling details.
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW); //front faces are counter-clockwise

	cout << "Loaded Assets OK!\n";
}

void updateSimulation(double simLength) //update simulation with an amount of time to simulate for (in seconds)
{

	//calculate the amount of rotate for this timestep
	float rotate = (float)simLength * rotateSpeed; //simlength is a double for precision, but rotateSpeedVector in a vector of float, alternatively use glm::dvec3

	//modify the rotateMatrix with the rotate, as a rotate, around the z-axis
	const glm::vec3 unitX = glm::vec3(1, 0, 0);
	const glm::vec3 unitY = glm::vec3(0, 1, 0);
	const glm::vec3 unitZ = glm::vec3(0, 0, 1);
	const glm::vec3 unit45 = glm::normalize(glm::vec3(0, 1, 1));
	rotateMatrix = glm::rotate(rotateMatrix, rotate, unit45);
}

void render()
{
	glUseProgram(theProgram); //installs the program object specified by program as part of current rendering state

	//load data to GLSL that **may** have changed
	glUniformMatrix4fv(rotateMatrixLocation, 1, GL_FALSE, glm::value_ptr(rotateMatrix)); //uploaed the rotateMatrix to the appropriate uniform location
	           // upload only one matrix, and don't transpose it

	int s = sizeof(cubeWithColorAndNormals);
	size_t colorData = 0 + sizeof(GL_FLOAT) * 4 * 36; //0 plus number of bytes for position
	size_t normalData = colorData + sizeof(GL_FLOAT) * 4 * 36; //colorData plus number of bytes for color
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject); //bind positionBufferObject

	glEnableVertexAttribArray(positionLocation);
    glEnableVertexAttribArray(colorLocation);
	glEnableVertexAttribArray(normalLocation);

	glVertexAttribPointer(positionLocation, 4, GL_FLOAT, GL_FALSE, 0, 0); //define **how** values are reader from positionBufferObject in Attrib 0
	glVertexAttribPointer(colorLocation, 4, GL_FLOAT, GL_FALSE, 0, (void*)colorData); //define **how** values are reader from positionBufferObject in Attrib 1
	glVertexAttribPointer(normalLocation, 4, GL_FLOAT, GL_FALSE, 0, (void*)normalData); //define **how** values are reader from positionBufferObject in Attrib 1

	//glVertexAttribP
	glDrawArrays(GL_TRIANGLES, 0, 36); //Draw something, using Triangles, and 3 vertices - i.e. one lonely triangle

	glDisableVertexAttribArray(0); //cleanup
	glUseProgram(0); //clean up

}
void cleanUp()
{
	SDL_GL_DeleteContext(context);
	SDL_DestroyWindow(win);
	cout << "Cleaning up OK!\n";
}

int main( int argc, char* args[] )
{
	exeName = args[0];
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


	while (!done && (SDL_GetTicks() < 5000)) //LOOP FROM HERE, for 2000ms (or if done flag is set)
		//WARNING: SDL_GetTicks is only accurate to milliseconds, use SDL_GetPerformanceCounter and SDL_GetPerformanceFrequency for higher accuracy
	{
		//GET INPUT HERE - PLACEHOLDER

		updateSimulation(0.02); //call update simulation with an amount of time to simulate for (in seconds)
		  //WARNING - we are always updating by a constant amount of time. This should be tied to how long has elapsed
		    // see, for example, http://headerphile.blogspot.co.uk/2014/07/part-9-no-more-delays.html

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		render(); //RENDER HERE - PLACEHOLDER

		SDL_GL_SwapWindow(win);; //present the frame buffer to the display (swapBuffers)

	} //LOOP TO HERE

	//cleanup and exit
	cleanUp();
	SDL_Quit();

	return 0;
}
