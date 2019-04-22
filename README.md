# Marble Marcher: Community Edition

### Version 1.2.0

This is the community edition of Marble Marcher, a procedurally rendered fractal physics marble game in which you must get to the flag in each level as fast as you can. With 24 levels to unlock and an active speedrunning community, along with a rudimentary level editor, there's always new features being developed. If you complete all levels, you can use cheats to enhance and create a more exploratory experience.

Because version 1.1.0 was the last feature update of Marble Marcher, we (the Marble Marcher Speedrunning Community) opted to create a community edition to keep the community around the game alive and continuously improve the experience.
Currently this is maintained mainly by members of the [Marble Marcher Speedrunning Community on Discord](https://discord.gg/r3XrJxH), in addition to the members of the [subreddit](https://www.reddit.com/r/marblemarcher) and anyone else who has contributed.

All credit goes to [HackerPoet](https://github.com/HackerPoet) (aka [CodeParade](https://www.youtube.com/channel/UCrv269YwJzuZL3dH5PCgxUw)) for the [original game](https://github.com/HackerPoet/MarbleMarcher).

## Original Summary
*Marble Marcher is a video game demo that uses a fractal physics engine and fully procedural rendering to produce beautiful and unique gameplay unlike anything you've seen before.*

*The goal of the game is to reach the flag as quickly as possible.  But be careful not to*
*fall off the level or get crushed by the fractal!  There are 24 levels to unlock.*

*Download Link: https://codeparade.itch.io/marblemarcher*

*Video Explanation: https://youtu.be/9U0XVdvQwAI*

## Table of Contents
- [Changes](https://github.com/WAUthethird/Marble-Marcher-Community-Edition/blob/master/README.md#changes)
- [Proposed changes](https://github.com/WAUthethird/Marble-Marcher-Community-Edition/blob/master/README.md#proposed-changes)
- [System Dependencies](https://github.com/WAUthethird/Marble-Marcher-Community-Edition/blob/master/README.md#system-dependencies)
  - [macOS](https://github.com/WAUthethird/Marble-Marcher-Community-Edition/blob/master/README.md#macos)
  - [Arch Linux](https://github.com/WAUthethird/Marble-Marcher-Community-Edition/blob/master/README.md#arch-linux)
- [Building](https://github.com/WAUthethird/Marble-Marcher-Community-Edition/blob/master/README.md#building)
  - [macOS](https://github.com/WAUthethird/Marble-Marcher-Community-Edition/blob/master/README.md#macos-1)
  - [Arch Linux](https://github.com/WAUthethird/Marble-Marcher-Community-Edition/blob/master/README.md#arch-linux-1)
  - [Cross-Compile for Windows (macOS)](https://github.com/WAUthethird/Marble-Marcher-Community-Edition/blob/master/README.md#cross-compile-for-windows-on-macos)
  - [Compiling on Windows](https://github.com/WAUthethird/Marble-Marcher-Community-Edition/blob/master/README.md#compiling-on-windows)
- [Launching](https://github.com/WAUthethird/Marble-Marcher-Community-Edition/blob/master/README.md#launching)
  - [macOS](https://github.com/WAUthethird/Marble-Marcher-Community-Edition/blob/master/README.md#macos-2)
- [Other](https://github.com/WAUthethird/Marble-Marcher-Community-Edition/blob/master/README.md#other)
  - [Shortcuts](https://github.com/WAUthethird/Marble-Marcher-Community-Edition/blob/master/README.md#shortcuts)

## Changes
- numerous miscellaneous fixes have been applied
- easier compilation for macOS
- ingame statistics added (marble speed, ground/air state, ...). Press `o` to toggle.
- AntTweakBar editor added. Press `F4` to toggle.

## Proposed Changes
Though Marble Marcher CE is a largely finished game, there are still several improvements we'd like to implement. Here are our proposed changes:

- ### Fixes
  - make sure game runs at the same speed everywhere
  - mouse/camera speed is too fast while skipping cutscenes
  - figure out why so many people get `Failed to compile vertex shader`
- ### User Experience Improvements
  - pre-built versions for the common operating systems and make them available as GitHub releases (in progress)
  - make the fractal recoloring from cheats persistent
  - more efficient anti-aliasing modes
  - toggle to always activate fast cutscenes
  - better autosplitter integration
  - #### UI Redesign
    - confirmation, exit buttons in end screens
    - change "controls" menu to "settings" and have audio, controls, sensitivity and fullscreen options there, also allow custom input bindings
    - improve text rendering
    - better layout
- ### New Features
  - cheat: no drag
  - anaglyph/stereoscopic mode
  - screenshot mode (temporarily higher resolution and AA)
  - native controller support (+deadzone)
  - custom marble designs
  - add a script to build for all platforms at once
  - have MarbleMarcher added to package managers
- ### Currently WIP
  - in-game level editor
  - metal marble skin
  - custom map support
  - recording/replay functionality
  - debug info screen
  - cheat: unlock all levels
  - add creation of macOS `.dng` for easy distribution.

## System Dependencies
* [Eigen](http://eigen.tuxfamily.org/index.php?title=Main_Page)
* [SFML 2.5.0](https://www.sfml-dev.org)
* [AntTweakBar](http://anttweakbar.sourceforge.net/)
### MacOS
On macOS these can be conveniently installed using [HomeBrew](https://brew.sh):

`brew install cmake eigen sfml anttweakbar`

The version of SFML required is 2.5.1 or newer. It has come to my attention that HomeBrew does now have this version (unlike when these instructions where first written) so installing via HomeBrew should work but you can still [download manually](https://www.sfml-dev.org/download/sfml/2.5.1/) if you wish and install using [these instructions](https://www.sfml-dev.org/tutorials/2.5/start-osx.php). You must install the Frameworks option not the dylib option or the build script may fail.
**Note that if HomeBrew installed a version of SFML older than 2.5.1 for some reason or you wish to install manually, you must remove the version of SFML that Brew installed using `brew remove sfml`.**

Alternatively, [vcpkg](https://github.com/Microsoft/vcpkg) can be used though it is unsupported:

`vcpkg install cmake eigen3 sfml anttweakbar`

It may also be possible to use MacPorts which is also unsupported and untested.
### Arch Linux
`sudo pacman -S eigen sfml anttweakbar git cmake make`


## Building
### MacOS
#### Build Script
**Note for the current version: the macOS build is not yet entirely working properly so revert to the previous prerelease as required. SFML is properly included but as of yet, AntTweakBar is not so this executable will not work on machines without it installed.**
Simply run `./macOSBuild.sh`. This will generate the full Application bundle that can be used like any other application. It can even be used on systems without SFML as SFML is included in the bundle and the binary is relinked to these versions. Currently the script will only do the relinking part properly if you use SFML 2.5.1 specifically however it is planned to allow for any version. If you have another version, the script will still work, the app just won't work on a machine without SFML.
#### Manual
* `mkdir build && cd build`
* `cmake ..`
* `cmake -DCMAKE_CXX_FLAGS="-I/usr/local/include" ..`
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
Windows compilation should work just fine now. It's relatively easy to do without help, but in case you'd like it, [here are some configuring and compiling instructions](https://www.reddit.com/r/Marblemarcher/comments/bamqyh/how_to_configure_and_compile_source_for_windows/).

## Launching
## macOS
If the macOS build script was used, simply launch the app as normal, otherwise:
* Make sure that the current working directory contains the `assets` folder
* Run the executable generated by CMake, located in `build` (or a subdirectory)
* If running MarbleMarcher from a tarball and you see a message like

> ./MarbleMarcher: error while loading shared libraries: libsfml-graphics.so.2.5: cannot open shared object file: No such file or directory
You'll just need to run MarbleMarcher with the correct `LD_LIBRARY_PATH`:

```shell
LD_LIBRARY_PATH=`pwd`/usr/lib ./MarbleMarcher
```

## Special Controls
* Press `o` to toggle the debug screen.
* Press `F5` to take a screenshot.
* Press `F4` to open AntTweakBar.
