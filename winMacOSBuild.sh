#! /bin/bash

mkdir -p winBuild
cp  src/* winBuild/
cd winBuild
wget -nc https://bitbucket.org/eigen/eigen/get/3.3.7.zip
wget -nc https://www.sfml-dev.org/files/SFML-2.5.1-windows-gcc-7.3.0-mingw-64-bit.zip
wget -nc https://astuteinternet.dl.sourceforge.net/project/anttweakbar/AntTweakBar_116.zip
unzip -u -o 3.3.7.zip
unzip -u -o SFML-2.5.1-windows-gcc-7.3.0-mingw-64-bit.zip
unzip -u -o AntTweakBar_116.zip
git clone https://github.com/SFML/SFML.git
export CPATH=`pwd`/eigen-eigen-323c052e1731/:`pwd`/SFML-2.5.1/include:`pwd`/SFML/exttlibs/headers:`pwd`/AntTweakBar/lib:`pwd`/AntTweakBar/include

x86_64-w64-mingw32-g++ -Ofast -c Level.cpp -D SFML_STATIC
x86_64-w64-mingw32-g++ -Ofast -c Level.h -D SFML_STATIC
x86_64-w64-mingw32-g++ -Ofast -c Overlays.cpp -D SFML_STATIC
x86_64-w64-mingw32-g++ -Ofast -c Overlays.h -D SFML_STATIC
x86_64-w64-mingw32-g++ -Ofast -c Res.h -D SFML_STATIC
x86_64-w64-mingw32-g++ -Ofast -c Scene.cpp -D SFML_STATIC
x86_64-w64-mingw32-g++ -Ofast -c Scene.h -D SFML_STATIC
x86_64-w64-mingw32-g++ -Ofast -c Scores.cpp -D SFML_STATIC
x86_64-w64-mingw32-g++ -Ofast -c Scores.h -D SFML_STATIC
x86_64-w64-mingw32-g++ -Ofast -c SelectRes.cpp -D SFML_STATIC
x86_64-w64-mingw32-g++ -Ofast -c SelectRes.h -D SFML_STATIC
x86_64-w64-mingw32-g++ -Ofast -c Settings.h -D SFML_STATIC

rm MarbleMarcherSources.a
x86_64-w64-mingw32-ar rvs MarbleMarcherSources.a Level.o
x86_64-w64-mingw32-ar rvs MarbleMarcherSources.a Level.h.gch
x86_64-w64-mingw32-ar rvs MarbleMarcherSources.a Overlays.o
x86_64-w64-mingw32-ar rvs MarbleMarcherSources.a Overlays.h.gch
x86_64-w64-mingw32-ar rvs MarbleMarcherSources.a Res.h.gch
x86_64-w64-mingw32-ar rvs MarbleMarcherSources.a Scene.o
x86_64-w64-mingw32-ar rvs MarbleMarcherSources.a Scene.h.gch
x86_64-w64-mingw32-ar rvs MarbleMarcherSources.a Scores.o
x86_64-w64-mingw32-ar rvs MarbleMarcherSources.a Scores.h.gch
x86_64-w64-mingw32-ar rvs MarbleMarcherSources.a SelectRes.o
x86_64-w64-mingw32-ar rvs MarbleMarcherSources.a SelectRes.h.gch
x86_64-w64-mingw32-ar rvs MarbleMarcherSources.a Settings.h.gch

x86_64-w64-mingw32-g++ -Ofast -pipe -o MarbleMarcher Main.cpp -D SFML_STATIC -D SFML_USE_STATIC_LIBS=true \
-static -static-libgcc -static-libstdc++ MarbleMarcherSources.a -L `pwd`/SFML/extlibs/libs-mingw/x64 -L \
`pwd`/SFML-2.5.1/lib -L `pwd`/AntTweakBar/lib/AntTweakBar.dll -lsfml-window-s -lsfml-graphics-s -lsfml-audio-s -lsfml-system-s -lopengl32 -lfreetype \
-lwinmm -lgdi32 -lopenal32 -lvorbisfile -lvorbisenc -lvorbis -lflac -logg

cd ..
cp winBuild/MarbleMarcher.exe ./
