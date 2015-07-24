#Learning 3D computer graphics in OpenGL

- A series of examples each as a git branch

- Build Status of master branch - [![Build Status](https://travis-ci.org/shearer12345/graphics_examples_in_git_branches.svg?branch=master)](https://travis-ci.org/shearer12345/graphics_examples_in_git_branches)

```bash
git clone --recursive https://github.com/shearer12345/graphics_examples_in_git_branches.git
```

##Branches available (order of age-ish, but grouped)

- master (working point)

###glTriangle GLSL (GPU) effects
- glTriangleGLSLTranslatedWithUniformChangingOverTime
- glTriangleGLSLTranslatedWithUniform
- glTriangleGLSLScaled
- glTriangleGLSLTranslated
- glTriangleGLSLGreen

###glTriangle CPU effects
- glTriangleWhiteCPUScaled
- glTriangleWhiteCPUTranslated
- glTriangleWhiteDifferentShape
- glTriangleWhiteWithRedBackground

###glTriangle
- glTriangleWhiteWithWindowNamed
- glTriangleWhite

###glSetup
- glGenVertexBuffer
- glCreateShadersAndProgram
- glGlew
- glIncluded

###SDL setup
- sdlHelloWorldWithFunctions
- sdlHelloWorld
- sdlCreateContext
- sdlCreateWindow
- sdlInitialise
- sdlIncludeAndLink

###Base build
- baseBuildWithNoFeaturesWithTravisTesting
- baseBuildWithNoFeatures

##Dependencies

- git
- premake4 (already in the repository for Windows)
- libsdl2
- libglew
- a build environment
    - Linux:
        - ```sudo apt-get install build-essential premake4 libsdl2-dev libglew-dev```
    - Windows:
        - Visual Studio, or something else of your choice
  
##Getting the code

- you'll need to recursively clone to repository, then run the `branchAndTrack.py` script to pull all the branches locally and track them
    - on Windows, start Git Shell
    - on Linux, use any shell

###Clone
```bash
git clone --recursive https://github.com/shearer12345/graphics_examples_in_git_branches.git
```

###Change in the directory
```bash
cd graphics_examples_in_git_branches
```

###pull and track all branches
```bash
python branchAndTrack.py
```
or
```bash
py branchAndTrack.py
```

- *Note: If python is not in your path, you may need to install it, reference it directly, or add it to your path*
     - it may be called `py.exe` on your system (if a Lincoln Student in Lab A), so try `py branchAndTrack.py`
     - install from https://www.python.org/
     - reference it directly (Windows)  - `C:\Python34\python.exe .\branchAndTrack.py`
     - add to your, or the system environment variable `PATH`

###Check which branches are local
```bash
git branch -l
```

##Usage

- For each example, you'll need to check out its branch
    - you can also **diff** between branches to see what's changed

- the cpp main function is consistently in the file main.cpp

- run premake4 to build the build files for your platform / build tool of choice
    - on Windows, you'll need to use premake4.bat
    - Linux example: `premake4 gmake` or `premake4 vs2010` (you can build the VS solution on Linux!)
    - Windows example: `.\premake4.bat vs2012` #**note the `.\`

- build using your tool of choice
    - e.g. `make`, or load the solution in visual studio and build
  
- run the created exe (named after the branch)
    - Linux: `./baseBuildWithNoFeatures.exe`
    - or from Visual Studio
  
