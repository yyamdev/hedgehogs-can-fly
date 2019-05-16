# Hedgehogs Can Fly

2D physics platformer game written in C++ using [SFML](https://www.sfml-dev.org/). Binary download, screenshots and videos available on [GameJolt](https://gamejolt.com/games/hogscanfly/281774).

### Dependencies
 * [SFML](https://www.sfml-dev.org/) (version 2.3.2)
 * [SFGUI](https://github.com/TankOs/SFGUI) (version 0.3.1)
 * [imgui-sfml](https://github.com/eliasdaler/imgui-sfml)
 * [libconfig](https://hyperrealm.github.io/libconfig/)
 * [Dear ImGui](https://github.com/ocornut/imgui)
 * windows.h (minimal, see main.cpp)

### Building
NOTE: Only tested on Windows
1. Clone repo.
2. Run `git submodule update --init` to clone the correct version of imgui-sfml
3. Build or download SFML 2.3.2, SFGUI 0.3.1 and libconfig
4. Clone [Dear ImGui](https://github.com/ocornut/imgui)
5. Append the contents of [imgui-sfml/imconfig-SFML.h](https://github.com/eliasdaler/imgui-sfml/blob/3979425f629125f2e048c25948ac3208178b16bc/imconfig-SFML.h) to [imgui/imconfig.h](https://github.com/ocornut/imgui/blob/master/imconfig.h)
6. Build the game with your preferred build system (C++14). All .cpp files in the src/ directory must be compiled PLUS imgui-sfml/imgui-SFML.cpp (from [imgui-sfml](https://github.com/eliasdaler/imgui-sfml)) and imgui/imgui.cpp, imgui/imgui_draw.cpp, imgui/imgui_widgets.cpp (from [Dear ImGui](https://github.com/ocornut/imgui)). Make sure to link to SFML, SFGUI and libconfig and ensure their header files are able to be included.

### Running
The game expects an assets folder called `data` to be present in the same in the same directory as the executable. These assets are not tracked in this repository but can be obtained by downloading the game on [GameJolt](https://gamejolt.com/games/hogscanfly/281774).
