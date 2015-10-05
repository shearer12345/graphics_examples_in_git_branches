branch = dofile("getGitBranch.lua")

-- A solution contains projects, and defines the available configurations
solution "graphicsByExample"
   targetdir ( "bin" )
   configurations { "Debug", "Release"}
   
   flags { "Unicode" , "NoPCH"}	
   

   -- A project defines one build target
   project (branch)  -- this project is called the name of the current git branch
      kind "ConsoleApp"
      language "C++"
      configuration { "windows" }
         buildoptions ""
         linkoptions { "/NODEFAULTLIB:msvcrt" } -- https://github.com/yuriks/robotic/blob/master/premake5.lua
      configuration { "linux" }
         buildoptions "-std=c++11" --http://industriousone.com/topic/xcode4-c11-build-option
         toolset "gcc"
      configuration {}

      files { "**.h", "**.cpp" } -- build all .h and .cpp files recursively
      excludes { "./graphics_dependencies/**" }  -- don't build files in graphics_dependencies/


      -- where are header files?
      configuration "windows"
      includedirs {
                    "./graphics_dependencies/SDL2/include",
                    "./graphics_dependencies/glew/include"
                  }
      configuration { "linux" }
      includedirs {
                    "/usr/include/SDL2",
                  }
      configuration {}


      -- what libraries need linking to
      configuration "windows"
         links { "SDL2", "SDL2main", "opengl32", "glew32" }
      configuration "linux"
         links { "SDL2", "SDL2main", "GL", "GLEW" }
      configuration {}

	  
      -- where are libraries? 
      configuration "windows"
      libdirs { 
                "./graphics_dependencies/glew/lib/Release/Win32",
                "./graphics_dependencies/SDL2/lib/win32"
              }
      configuration "linux"
               -- should be installed as in ./graphics_dependencies/README.asciidoc
      configuration {}

 
      configuration "*Debug"
         defines { "DEBUG" }
         flags { "Symbols" }
         optimize "Off"
         targetsuffix "-debug"


      configuration "*Release"
         defines { "NDEBUG" }
         optimize "On"  

      configuration "windows"
         postbuildcommands { 
                              --
                              --copy dlls. Must escape the backslashes - https://github.com/premake/premake-core/wiki/postbuildcommands
                              --"xcopy ..\\graphics_dependencies\\glew\\bin\\Release\\Win32\\glew32.dll .\\bin\\  /d",
                              --"xcopy ..\\graphics_dependencies\\SDL2\\lib\\win32\\SDL2.dll .\\bin\\  /d",
                           }
