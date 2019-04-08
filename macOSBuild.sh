#! /bin/bash

mkdir -p build && cd build
cmake ..
cd ..
cmake --build build

rm -R -f MarbleMarcher.app
cp -R macOS/template MarbleMarcher.app
cp macOS/MarbleMarcher.icns MarbleMarcher.app/Contents/Resources/
cp -R assets MarbleMarcher.app/Contents/Resources/
mv build/MarbleMarcher MarbleMarcher.app/Contents/Resources/
cp -R game_folder/* MarbleMarcher.app/Contents/Resources
cp build/AntTweakBar.dll MarbleMarcher.app/Contents/Resources/
cp -R build/assets MarbleMarcher.app/Contents/Resources
cp -R build/images MarbleMarcher.app/Contents/Resources
cp -R build/screenshots MarbleMarcher.app/Contents/Resources
cp -R build/shaders MarbleMarcher.app/Contents/Resources
cp -R build/sound MarbleMarcher.app/Contents/Resources


cd MarbleMarcher.app/Contents/Resources
install_name_tool -change @rpath/sfml-graphics.framework/Versions/2.5.1/sfml-graphics @executable_path/sfml-graphics MarbleMarcher
install_name_tool -change @rpath/sfml-audio.framework/Versions/2.5.1/sfml-audio @executable_path/sfml-audio MarbleMarcher
install_name_tool -change @rpath/sfml-window.framework/Versions/2.5.1/sfml-window @executable_path/sfml-window MarbleMarcher
install_name_tool -change @rpath/sfml-system.framework/Versions/2.5.1/sfml-system @executable_path/sfml-system MarbleMarcher
