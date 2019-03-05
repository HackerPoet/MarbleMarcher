# Marble Marcher: Community Edition

This is the community edition of Marble Marcher, created mainly by the members of the [Marble Marcher Speedrunning Community on Discord](https://discord.gg/r3XrJxH), in addition to the members of the [subreddit](https://www.reddit.com/r/marblemarcher) and anyone else who has contributed.

All credit goes to [HackerPoet](https://github.com/HackerPoet) for the [original game](https://github.com/HackerPoet/MarbleMarcher).

## Original Summary
*Marble Marcher is a video game demo that uses a fractal physics engine and fully procedural rendering to produce beautiful and unique gameplay unlike anything you've seen before.*

*The goal of the game is to reach the flag as quickly as possible.  But be careful not to*
*fall off the level or get crushed by the fractal!  There are 24 levels to unlock.*

*Download Link: https://codeparade.itch.io/marblemarcher*

*Video Explanation: https://youtu.be/9U0XVdvQwAI*

## Goals
Because version 1.1.1 is most likely going to be the last official version of Marble Marcher, we (as a community) have opted to create a community edition of the game. Here are our proposed changes:

- ### Fixes
  - make sure game runs at the same speed everywhere
  - mouse/camera speed is too fast while skipping cutscenes
  - figure out why so many people get `Failed to compile vertex shader`
- ### User Experience Improvements
  - pre-built versions for the common operating systems and make them available as github releases (in progress)
  - confirmation, exit buttons in end screens
  - change "controls" menu to "settings" and have audio, controls, sensitivity and fullscreen options there, also allow custom input bindings
  - make the fractal recoloring from cheats persistent
  - more efficient anti-aliasing modes
  - toggle to always activate fast cutscenes
  - debug mode
  - better autosplitter integration
  - improve text rendering
- ### New Features
  - recording/replay functionality
  - custom map support
  - custom marble designs
  - cheat: no drag
  - anaglyph/stereoscopic mode (easy to implement but how do we switch between them)
  - screenshot mode (temporarily higher resolution and AA)
  - native controller support (+deadzone)
  - in-game fractal editor, using sliders (currently using mousewheel scroll and numbers)

## Current Version
Numerous fixes have been applied, along with adding easier MacOS compilation.

## System Dependencies
* [Eigen](http://eigen.tuxfamily.org/index.php?title=Main_Page)
* [SFML 2.5.0](https://www.sfml-dev.org)
### MacOS
On macOS these can be conveniently installed using [HomeBrew](https://brew.sh):

`brew install eigen sfml`

The version of SFML required is 2.5.1 or newer. HomeBrew does not have this version yet so it must be [downloaded manually](https://www.sfml-dev.org/download/sfml/2.5.1/) and installed using [these instructions](https://www.sfml-dev.org/tutorials/2.5/start-osx.php). You must install the Frameworks option not the dylib option or the build script may fail.
**It is very important that if you installed SFML with brew before realizing that you have to install it manually, that you remove the version of SFML that Brew installed using `brew remove sfml`.**

Alternatively, [vcpkg](https://github.com/Microsoft/vcpkg) can be used:

`vcpkg install eigen3 sfml`
### Arch Linux
`sudo pacman -S eigen sfml git cmake make`



## Building
### MacOS
#### Build Script
Simply run `./macOSBuild.sh`. This will generate the full Application bundle that can be used like any other application. It can even be used on systems without SFML as SFML is included in the bundle and the binary is relinked to these versions. Currently the script will only do the relinking part properly if you use SFML 2.5.1 specifically however it is planned to allow for any version. If you have another version, the script will still work, the app just won't work on a machine without SFML.
#### Manual
* `mkdir build && cd build`
* `cmake ..`
    * If you use `vcpkg`, add the flag `-DCMAKE_TOOLCHAIN_FILE=[path/to/vcpkg]/scripts/buildsystems/vcpkg.cmake`
* `cd ..`
* `cmake --build build`

Note that this just builds a binary and not an Application bundle like you might be used to seeing. To run properly, you must move the binary (which, after building, is `build/MarbleMarcher`) to the same folder as the assets folder. It is not recommended to build the Application bundle manually so no instructions for that are provided however you may peek in `macOSBuild.sh` to see how it is done.

Alternatively, one can use the platform-dependent build system, for example `Make`:

* `make -C build`
### Arch Linux
* `cd ~`
* `git clone https://github.com/WAUthethird/Marble-Marcher-Community-Edition.git`
* `cd Marble-Marcher-Community-Edition`
* `mkdir build && cd build`
* `cmake ..`
* `cd ..`
* `cmake --build build`
* `cp build/MarbleMarcher ./`

### Cross-Compile for Windows on macOS
This requires you to install wget, mingw-w64, and git (which you probably already have) either with HomeBrew (recommended) or otherwise. Theoretically, you should be able to just run `winMacOSBuild.sh`. There are no manual instructions because due to issues I had to just compile the thing manually which is annoying and has too many steps.

### Compiling on Windows
Windows compilation has proved to be quite tough, and all efforts to compile via CMake have failed. There's only been one person who has managed to compile and run on Windows, and that was via manual compilation. But in case you'd like to try your hand at it, [here are some configuring (not compiling) instructions](https://www.reddit.com/r/Marblemarcher/comments/atpq47/how_to_configure_source_for_windows_with_cmake/). Keep in mind that after you finish the configuration, (which is a lot easier to pull off than compilation) you're on your own. But don't fret; we'll be fixing these issues once we figure out *how* exactly to do it.

## Launching
If the macOS build script was used, simply launch the app as normal, otherwise:
* Make sure that the current working directory contains the `assets` folder
* Run the executable generated by CMake, located in `build` (or a subdirectory)
* If running MarbleMarcher from a tarball and you see a message like

> ./MarbleMarcher: error while loading shared libraries: libsfml-graphics.so.2.5: cannot open shared object file: No such file or directory

You'll just need to run MarbleMarcher with the correct `LD_LIBRARY_PATH`:

```shell
LD_LIBRARY_PATH=`pwd`/usr/lib ./MarbleMarcher
```

### Debug Screen
Press `o` to toggle the debug screen.
