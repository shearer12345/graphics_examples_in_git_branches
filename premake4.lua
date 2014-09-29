branch = dofile("getGitBranch.lua")

-- A solution contains projects, and defines the available configurations
solution "graphicsExamplesInGitBranches"
   targetdir ( "bin" )
   configurations { "Debug", "Release" }
 
 
   -- A project defines one build target
   project (branch)  -- this project is called the name of the current git branch
      kind "ConsoleApp"
      language "C++"
      buildoptions "-std=c++11" --http://industriousone.com/topic/xcode4-c11-build-option

      files { "**.h", "**.cpp" }
 
      configuration "Debug"
         defines { "DEBUG" }
         flags { "Symbols" }
         targetsuffix "-debug"
 
      configuration "Release"
         defines { "NDEBUG" }
         flags { "Optimize" }    