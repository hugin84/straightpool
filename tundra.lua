Build {
	Units	= "units.lua",
	Env		= {
		CCOPTS	= {
			{ "-Wall", "-Werror";     Config = { "*-gcc-*", "*-clang-*" } },
			{ "-g";                   Config = { "*-gcc-debug-*", "*-clang-debug-*", "*-gcc-production-*", "*-clang-production-*" } },
			{ "-O2";                  Config = { "*-gcc-production-*", "*-clang-production-*"	} },
			{ "-O2";                  Config = { "*-gcc-release-*", "*-clang-release-*" } },
			{ "-fomit-frame-pointer"; Config = { "*-gcc-*", "*-clang-*" } },
			{ "-std=c11";             Config = { "*-gcc-*", "*-clang-*" } },
		},
		CXXOPTS = {
			{ "-Wall", "-Werror";     Config = { "*-gcc-*", "*-clang-*" } },
			{ "-g";                   Config = { "*-gcc-debug-*", "*-clang-debug-*", "*-gcc-production-*", "*-clang-production-*" } },
			{ "-O2";                  Config = { "*-gcc-production-*", "*-clang-production-*"	} },
			{ "-O2";                  Config = { "*-gcc-release-*", "*-clang-release-*" } },
			{ "-fomit-frame-pointer"; Config = { "*-gcc-*", "*-clang-*" } },
			{ "-std=c++11";           Config = { "*-gcc-*", "*-clang-*" } },
			{ "-fno-rtti";            Config = { "*-gcc-*", "*-clang-*" } },
			{ "-fno-exceptions";      Config = { "*-gcc-*", "*-clang-*" } },
		},
		CPPDEFS = {
			{ "NDEBUG";               Config = { "*-*-release-*" } },
		},
		LIBPATH = {
			{ "/usr/lib";             Config = { "linux-*" } },
		},
		PROGOPTS = {
			{ "-s";                   Config = { "*-gcc-release", "*-clang-release" } },
			{ "-rdynamic";            Config = { "*-gcc-*", "*-clang-*", } }, -- needed for backtrace
		},
	},
	Configs	= {
		Config {
			Name  = "linux-gcc",
			Tools = { "gcc" },
			DefaultOnHost = "linux",
		},
		Config {
			Name  = "linux-clang",
			Tools = { "clang" },
			SupportedHosts = { "linux" },
		},
	},
	IdeGenerationHints = {
		Codelite = {
			PlatformMappings = {
				['linux-gcc']   = 'gcc',
				['linux-clang'] = '',
			},
		},
		CodeliteWorkspace = {
			['straightpool.workspace'] = {},
		},
		CodeliteWorkspaceDir = "codelite",
	},
	DefaultVariant = "production"
}
