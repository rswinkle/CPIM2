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
				"main.c",
				"cpim.c",
				"cpim.h",
				"cvector.h",
				"c_utils.c",
				"c_utils.h",
				"clist.h",
			}
--      libdirs { }
		links { "sqlite3" }
		targetdir "build"
	
		configuration "Debug"
			defines { "DEBUG" }
			symbols "On"

		configuration "Release"
			defines { "NDEBUG" }
			symbols "Off"
			optimize "On"

		configuration { "linux", "gmake" }
			buildoptions { "-std=c99", "-pedantic-errors", "-fno-strict-aliasing", "-Wunused-variable", "-Wreturn-type" }
