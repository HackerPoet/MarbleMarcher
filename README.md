# Marble Marcher

Marble Marcher is a video game demo that uses a fractal physics engine and fully procedural rendering to produce beautiful and unique gameplay unlike anything you've seen before.

The goal of the game is to reach the flag as quickly as possible.  But be careful not to
fall off the level or get crushed by the fractal!  There are 15 levels to unlock.

Download Link: [Windows 64-bit](https://github.com/HackerPoet/MarbleMarcher/raw/master/MarbleMarcher.zip)

Video Explanation: https://youtu.be/9U0XVdvQwAI

## System Dependencies
* [Eigen](http://eigen.tuxfamily.org/index.php?title=Main_Page)
* [SFML 2.5.0](https://www.sfml-dev.org)
* [Boost Filesystem](https://www.boost.org)

On macOS these can be conveniently installed using [Homebrew](https://brew.sh):

`brew install eigen boost sfml`

(Note that SFML might require a newer version than the one from Homebrew, in which case a manual installation is required)

Alternatively, [vcpkg](https://github.com/Microsoft/vcpkg) can be used:

`vcpkg install eigen3 boost-filesystem sfml`

## Building
* `mkdir build && cd build`
* `cmake ..`
    * Note that the `resources.c` file containing the embedded, binary resources from the `assets` folder will only be generated if the assets have been modified
    * To successfully rebuild the binary resources the `CMakeLists.txt` might need to be resaved
* `cmake --build .`

Alternatively, one can use the platform-dependent build system, for example `Make`:

* `make`
