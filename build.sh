#!/bin/bash
mkdir output
gcc  -c -D_GNU_SOURCE -Isource -I/usr/include -Ithirdparty -Wall -Werror -g -O2 -fomit-frame-pointer -std=c11   -o output/bb_string-c.o source/bb_string.c
g++  -c -D_GNU_SOURCE -Isource -I/usr/include -Ithirdparty -Wall -Werror -g -O2 -fomit-frame-pointer -std=c++11 -fno-rtti -fno-exceptions -Wno-class-memaccess   -o output/main-cpp.o source/main.cpp
gcc  -c -D_GNU_SOURCE -Isource -I/usr/include -Ithirdparty -Wall -Werror -g -O2 -fomit-frame-pointer -std=c11   -o output/bb_assert-c.o source/bb_assert.c
gcc  -c -D_GNU_SOURCE -Isource -I/usr/include -Ithirdparty -Wall -Werror -g -O2 -fomit-frame-pointer -std=c11   -o output/bb_filesystem-c.o source/bb_filesystem.c
g++  -c -D_GNU_SOURCE -Isource -I/usr/include -Ithirdparty -Wall -Werror -g -O2 -fomit-frame-pointer -std=c++11 -fno-rtti -fno-exceptions -Wno-class-memaccess   -o output/imgui_impl_opengl3-cpp.o source/imgui_impl_opengl3.cpp
g++  -c -D_GNU_SOURCE -Isource -I/usr/include -Ithirdparty -Wall -Werror -g -O2 -fomit-frame-pointer -std=c++11 -fno-rtti -fno-exceptions -Wno-class-memaccess   -o output/imgui_impl_sdl-cpp.o source/imgui_impl_sdl.cpp
g++  -c -D_GNU_SOURCE -Isource -I/usr/include -Ithirdparty -Wall -Werror -g -O2 -fomit-frame-pointer -std=c++11 -fno-rtti -fno-exceptions -Wno-class-memaccess   -o output/bb_app-cpp.o source/bb_app.cpp
gcc  -c -D_GNU_SOURCE -Isource -I/usr/include -Ithirdparty -Wall -Werror -g -O2 -fomit-frame-pointer -std=c11   -o output/stb_image_impl-c.o source/stb_image_impl.c
g++  -c -D_GNU_SOURCE -Isource -I/usr/include -Ithirdparty -Wall -Werror -g -O2 -fomit-frame-pointer -std=c++11 -fno-rtti -fno-exceptions -Wno-class-memaccess   -o output/imgui_draw-cpp.o thirdparty/imgui/imgui_draw.cpp
g++  -c -D_GNU_SOURCE -Isource -I/usr/include -Ithirdparty -Wall -Werror -g -O2 -fomit-frame-pointer -std=c++11 -fno-rtti -fno-exceptions -Wno-class-memaccess   -o output/imgui_widgets-cpp.o thirdparty/imgui/imgui_widgets.cpp
g++  -c -D_GNU_SOURCE -Isource -I/usr/include -Ithirdparty -Wall -Werror -g -O2 -fomit-frame-pointer -std=c++11 -fno-rtti -fno-exceptions -Wno-class-memaccess   -o output/imgui-cpp.o thirdparty/imgui/imgui.cpp
gcc -rdynamic -Loutput -L/usr/lib -o output/straightpool output/bb_app-cpp.o output/bb_assert-c.o output/bb_filesystem-c.o output/bb_string-c.o output/imgui_impl_opengl3-cpp.o output/imgui_impl_sdl-cpp.o output/main-cpp.o output/stb_image_impl-c.o output/imgui-cpp.o output/imgui_draw-cpp.o output/imgui_widgets-cpp.o -lc -lrt -lpthread -ldl -lm -lGL -lSDL2 -lstdc++
