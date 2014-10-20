branch = dofile("getGitBranch.lua")

-- A solution contains projects, and defines the available configurations
solution "graphicsExamplesInGitBranches"
   targetdir ( "bin" )
   configurations { "Debug", "Release" }
    
   -- A project defines one build target
   project (branch)  -- this project is called the name of the current git branch
      kind "ConsoleApp"
      language "C++"
      configuration { "linux" }
         buildoptions "-std=c++11" --http://industriousone.com/topic/xcode4-c11-build-option
      configuration {}

      files { "**.h", "**.cpp" }

      -- where are header files?
      includedirs {
                    "./graphics_dependencies/SDL2-2.0.3/include",
                    "/usr/include/SDL2",
                    "./graphics_dependencies/glew-1.11.0/include"
                  }

      -- what libraries need linking to
      configuration "windows"
         links { "SDL2", "SDL2main", "opengl32", "glew32" }
      configuration "linux"
         links { "SDL2", "SDL2main", "GL", "GLEW" }
      configuration {}

      -- where are libraries? 
      libdirs { 
                    "./graphics_dependencies/glew-1.11.0/lib/Release/Win32",
                    "./graphics_dependencies/SDL2-2.0.3/lib/x86"
              }
 
      configuration "Debug"
         defines { "DEBUG" }
         --flags { "Symbols" }
         targetsuffix "-debug"
 
      configuration "Release"
         defines { "NDEBUG" }
         flags { "Optimize" }    
