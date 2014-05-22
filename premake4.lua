branch = dofile("getGitBranch.lua")

-- A solution contains projects, and defines the available configurations
solution "graphicsExamplesInGitBranches"
   configurations { "Debug", "Release" }
 
 
   -- A project defines one build target
   project (branch)  -- this project is called the name of the current git branch
      kind "ConsoleApp"
      language "C++"
      files { "**.h", "**.cpp" }
 
      configuration "Debug"
         defines { "DEBUG" }
         flags { "Symbols" }
 
      configuration "Release"
         defines { "NDEBUG" }
         flags { "Optimize" }    