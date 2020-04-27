require "tundra.syntax.glob"


local straightpool = Program {
	Name    = "straightpool",
	Env		= {
		CPPPATH	= { 
			"source",
			"/usr/include",
			"thirdparty"
		},
		CPPDEFS = {
			{ "_GNU_SOURCE";          Config = { "linux-*" } },
		},
		CXXOPTS = {
			{ "-Wno-class-memaccess";        Config = { "*-gcc-*" } },
		},
		LIBS = {
			{
				  "c"
				, "rt"
				, "pthread"
				, "dl"
				, "m"
				, "GL"
				, "SDL2"
				, "stdc++"
				; Config = { "linux-*-*" }
			},
		},
	},
	Sources = {
		Glob {
			Dir        = "source",
			Extensions = { ".c", ".h", ".cpp", ".hpp" },
		},
		"thirdparty/imgui/imgui.cpp",
		"thirdparty/imgui/imgui_draw.cpp",
		"thirdparty/imgui/imgui_widgets.cpp"
	},
}


Default "straightpool"

