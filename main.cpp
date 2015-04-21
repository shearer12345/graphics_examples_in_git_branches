#include <iostream>
#include <vector>
#include <algorithm>

#include <GL/glew.h>
#include <SDL.h>

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
	"in vec3 position;\n"
	"void main()\n"
	"{\n"
	"   gl_Position.xyz = position;\n"
	"   gl_Position.w = 1.0;\n"
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
	"out vec4 outputColor;\n"
	"void main()\n"
	"{\n"
	"   outputColor = vec4(1.0f, 0.0f, 1.0f, 1.0f);\n"
	"}\n"
	);


//our variables
bool done = false;

struct Vertex
{
	GLfloat x, y, z;
	//float nx, ny, nz;
	//float u, v;

	//Constructor
	Vertex(GLfloat p_x, GLfloat p_y, GLfloat p_z)
		: x(p_x), y(p_y), z(p_z)
	{ }
};

std::vector<Vertex> vertices;

//our GL and GLSL variables

GLuint theProgram; //GLuint that we'll fill in to refer to the GLSL program (only have 1 at this point)
GLint positionLocation; //GLuint that we'll fill in with the location of the `offset` variable in the GLSL

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
	
	//clean up shaders (we don't need them anymore as they are no in theProgram
	for_each(shaderList.begin(), shaderList.end(), glDeleteShader);
}

void initializeVertexBuffer()
{

	//Procedurally generate some triangles

	//change the next 4 variables for interesting stuff
	float blockCount = 25;
	float blockWidth = 1.0f;
	float blockHeight = 1.0f;
	float portionOfBlockWidth = 2.0f;

	float i_scale = blockWidth / blockCount;
	float j_scale = blockHeight / blockCount;
	float i_scaleByFour = i_scale / (portionOfBlockWidth);
	float j_scaleByFour = j_scale / portionOfBlockWidth;
	//cout << "i_scale, j_scale:" << i_scale << ", " << j_scale << endl;

	for (float i = -blockWidth / 2.0f; i <  (blockWidth / 2.0f ); i += i_scale)
	{

		for (float j = -blockHeight/2.0f; j < (blockHeight / 2.0f); j += j_scale)
		{
			//cout << "i, j: " << i << ", " << j << endl;
			vertices.emplace_back(i, j, 0.0f);

			//cout << "i, j: " << i-i_scaleByFour << ", " << j-j_scaleByFour << endl;
			vertices.emplace_back(i + i_scaleByFour, j - j_scaleByFour, 0.0f);

			//cout << "i, j: " << i+i_scaleByFour << ", " << j-j_scaleByFour << endl << endl;
			vertices.emplace_back(i - i_scaleByFour, j - j_scaleByFour, 0.0f);
		}
	}

	//vertices.emplace_back(-0.5, -0.5, 0.0f); //manual testing
	//vertices.emplace_back(-0.625, -0.625, 0.0f);
	//vertices.emplace_back(-0.375, -0.625, 0.0f);

	cout << "vertices is of size: " << vertices.size() << endl;

	//Load vertices to GL
	if (!vertices.empty())
	{
		glGenBuffers(1, &positionBufferObject);
		glBindBuffer(GL_ARRAY_BUFFER, positionBufferObject);
		int verticesByteLength = sizeof(Vertex) * vertices.size();
		cout << "vertices ByteLength is: " << verticesByteLength << endl;
		glBufferData(GL_ARRAY_BUFFER, verticesByteLength, &vertices.front(), GL_STATIC_DRAW); //load the data to GL
		cout << "positionBufferObject created OK! GLUint is: " << positionBufferObject << std::endl;
	}
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	
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

void updateSimulation(double simLength) //update simulation with an amount of time to simulate for (in seconds)
{
	; // do nothing
}

void render()
{
	glUseProgram(theProgram); //installs the program object specified by program as part of current rendering state

	//load data to GLSL that **may** have changed
	//none here
	
	glBindBuffer(GL_ARRAY_BUFFER, positionBufferObject); //bind positionBufferObject

	glEnableVertexAttribArray(positionLocation); //this 0 corresponds to the location = 0 in the GLSL for the vertex shader.
		//more generically, use glGetAttribLocation() after GLSL linking to obtain the assigned attribute location.

	glVertexAttribPointer(positionLocation, 3, GL_FLOAT, GL_FALSE, 0, 0); //define **how** values are reader from positionBufferObject in Attrib 0

	glDrawArrays(GL_TRIANGLES, 0, 3 * vertices.size()); //Draw something, using Triangles, and 3 vertices - i.e. one lonely triangle

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
	SDL_GL_SwapWindow(win);; //present the frame buffer to the display (swapBuffers)

	//load stuff from files
	//- usually do just once
	loadAssets();

	SDL_GL_SwapWindow(win);; //present the frame buffer to the display (swapBuffers)
	
	while (!done && (SDL_GetTicks() < 5000)) //LOOP FROM HERE, for 2000ms (or if done flag is set)
		//WARNING: SDL_GetTicks is only accurate to milliseconds, use SDL_GetPerformanceCounter and SDL_GetPerformanceFrequency for higher accuracy
	{
		//GET INPUT HERE - PLACEHOLDER

		updateSimulation(0.02); //call update simulation with an amount of time to simulate for (in seconds)
		  //WARNING - we are always updating by a constant amount of time. This should be tied to how long has elapsed
		    // see, for example, http://headerphile.blogspot.co.uk/2014/07/part-9-no-more-delays.html

		glClear(GL_COLOR_BUFFER_BIT);

		render(); //RENDER HERE - PLACEHOLDER

		SDL_GL_SwapWindow(win);; //present the frame buffer to the display (swapBuffers)

	} //LOOP TO HERE

	//cleanup and exit
	cleanUp();
	SDL_Quit();

	return 0;
}
