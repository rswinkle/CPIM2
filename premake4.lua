-- A solution contains projects, and defines the available configurations
solution "CPIM2"
	 configurations { "Debug", "Release" }
	 location "build"
 
	 -- A project defines one build target
	 project "cpim2"
			location "build"
			kind "ConsoleApp"
			language "C"
			files {
				"main.c", "c_utils.c", "c_utils.h", "cvector.h"
			}
--      excludes { "vector_template.*", "cvector.h", "vector_tests2.c" }
--      libdirs { }
--      links { "cunit" }
		targetdir "build"
	
		configuration "Debug"
			defines { "DEBUG" }
			flags { "Symbols" }

		configuration "Release"
			defines { "NDEBUG" }
			flags { "Optimize" }

		configuration { "linux", "gmake" }
			buildoptions { "-std=c99", "-pedantic-errors", "-fno-strict-aliasing", "-Wunused-variable", "-Wreturn-type" }
