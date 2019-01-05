/* This file is part of the Marble Marcher (https://github.com/HackerPoet/MarbleMarcher).
* Copyright(C) 2018 CodeParade
* 
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 2 of the License, or
* (at your option) any later version.
* 
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
* 
* You should have received a copy of the GNU General Public License
* along with this program.If not, see <http://www.gnu.org/licenses/>.
*/
#include "Scene.h"
#include "Level.h"
#include "Overlays.h"
#include "LoadedResources.h"
#include "SelectRes.h"
#include "Scores.h"
#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>
#include <sys/types.h>
#include <sys/stat.h>
#include <string>
#include <iostream>
#include <fstream>
#include <thread>
#include <mutex>

#ifdef _WIN32
#include <Windows.h>
#define ERROR_MSG(x) MessageBox(nullptr, TEXT(x), TEXT("ERROR"), MB_OK);
#else
#define ERROR_MSG(x) std::cerr << x << std::endl;
#endif

//Constants
static const float mouse_sensitivity = 0.005f;
static const float wheel_sensitivity = 0.2f;
static const float music_vol = 75.0f;

//Game modes
enum GameMode {
  MAIN_MENU,
  PLAYING,
  PAUSED,
  SCREEN_SAVER,
  CONTROLS,
  LEVELS,
  CREDITS
};

//Global variables
static sf::Vector2i mouse_pos;
static float mouse_wheel = 0.0f;
static bool all_keys[sf::Keyboard::KeyCount] = { 0 };
static int frame_num = 0;
static bool lock_mouse = false;
static GameMode game_mode = MAIN_MENU;

void LockMouse(sf::RenderWindow& window) {
  window.setMouseCursorVisible(false);
  const sf::Vector2u size = window.getSize();
  mouse_pos = sf::Vector2i(size.x / 2, size.y / 2);
  sf::Mouse::setPosition(mouse_pos);
}
void UnlockMouse(sf::RenderWindow& window) {
  window.setMouseCursorVisible(true);
}
int dirExists(const char *path) {
  struct stat info;
  if (stat(path, &info) != 0) {
    return 0;
  } else if (info.st_mode & S_IFDIR) {
    return 1;
  }
  return 0;
}

#if defined(_WIN32)
int WinMain(HINSTANCE hInstance, HINSTANCE, LPTSTR lpCmdLine, int nCmdShow) {
#else
int main(int argc, char *argv[]) {
#endif
  //Make sure shader is supported
  if (!sf::Shader::isAvailable()) {
    ERROR_MSG("Graphics card does not support shaders");
    return 1;
  }
  //Load the vertex shader
  sf::Shader shader;
  if (!shader.loadFromMemory(vert_glsl_resource.Str(), sf::Shader::Vertex)) {
    ERROR_MSG("Failed to compile vertex shader");
    return 1;
  }
  //Load the fragment shader
  if (!shader.loadFromMemory(frag_glsl_resource.Str(), sf::Shader::Fragment)) {
    ERROR_MSG("Failed to compile fragment shader");
    return 1;
  }

  //Load the font
  sf::Font font;
  const Res font_res = Orbitron_Bold_ttf_resource;
  if (!font.loadFromMemory(font_res.ptr, font_res.size)) {
    ERROR_MSG("Unable to load font");
    return 1;
  }
  //Load the mono font
  sf::Font font_mono;
  const Res font_mono_res = Inconsolata_Bold_ttf_resource;
  if (!font_mono.loadFromMemory(font_mono_res.ptr, font_mono_res.size)) {
    ERROR_MSG("Unable to load mono font");
    return 1;
  }

  //Load the music
  sf::Music menu_music;
  const Res menu_music_res = menu_ogg_resource;
  menu_music.openFromMemory(menu_music_res.ptr, menu_music_res.size);
  menu_music.setLoop(true);
  menu_music.setVolume(music_vol);
  sf::Music level1_music;
  const Res level1_music_res = level1_ogg_resource;
  level1_music.openFromMemory(level1_music_res.ptr, level1_music_res.size);
  level1_music.setLoop(true);
  sf::Music level2_music;
  const Res level2_music_res = level2_ogg_resource;
  level2_music.openFromMemory(level2_music_res.ptr, level2_music_res.size);
  level2_music.setLoop(true);
  sf::Music credits_music;
  const Res credits_music_res = credits_ogg_resource;
  credits_music.openFromMemory(credits_music_res.ptr, credits_music_res.size);
  credits_music.setLoop(true);

  //Get the directory for saving and loading high scores
#ifdef _WIN32
  const std::string save_dir = std::string(std::getenv("APPDATA")) + "\\MarbleMarcher";
#else
  const std::string save_dir = std::string(std::getenv("HOME")) + "/.MarbleMarcher";
#endif
  
  if (!dirExists(save_dir.c_str())) {
#if defined(_WIN32)
    bool success = CreateDirectory(save_dir.c_str(), NULL) != 0 || GetLastError() == ERROR_ALREADY_EXISTS;
#else
    bool success = mkdir(save_dir.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == 0;
#endif
    if (!success) {
      ERROR_MSG("Failed to create save directory");
      return 1;
    }
  }
  const std::string save_file = save_dir + "/scores.bin";

  //Load scores if available
  high_scores.Load(save_file);

  //Have user select the resolution
  SelectRes select_res(&font_mono);
  const Resolution* resolution = select_res.Run();
  bool fullscreen = select_res.FullScreen();
  if (resolution == nullptr) {
    return 0;
  }

  //GL settings
  sf::ContextSettings settings;
  settings.depthBits = 24;
  settings.stencilBits = 8;
  settings.antialiasingLevel = 1;
  settings.majorVersion = 3;
  settings.minorVersion = 0;

  //Create the window
  sf::VideoMode screen_size;
  sf::Uint32 window_style;
  const sf::Vector2i screen_center(resolution->width / 2, resolution->height / 2);
  if (fullscreen) {
    screen_size = sf::VideoMode::getDesktopMode();
    window_style = sf::Style::Fullscreen;
  } else {
    screen_size = sf::VideoMode(resolution->width, resolution->height, 24);
    window_style = sf::Style::Close;
  }
  sf::RenderWindow window(screen_size, "Marble Marcher", window_style, settings);
  window.setVerticalSyncEnabled(true);
  window.setKeyRepeatEnabled(false);
  window.requestFocus();

  //If native resolution is the same, then we don't need a render texture
  if (resolution->width == screen_size.width && resolution->height == screen_size.height) {
    fullscreen = false;
  }

  //Create the render texture if needed
  sf::RenderTexture renderTexture;
  if (fullscreen) {
    renderTexture.create(resolution->width, resolution->height, settings);
    renderTexture.setSmooth(true);
    renderTexture.setActive(true);
    window.setActive(false);
  }

  //Create the fractal scene
  Scene scene(&level1_music, &level2_music);
  const sf::Glsl::Vec2 window_res((float)resolution->width, (float)resolution->height);
  shader.setUniform("iResolution", window_res);
  scene.Write(shader);

  //Create the menus
  Overlays overlays(&font, &font_mono);
  overlays.SetScale(float(screen_size.width) / 1280.0f);
  menu_music.play();

  //Main loop
  sf::Clock clock;
  float smooth_fps = 60.0f;
  while (window.isOpen()) {
    sf::Event event;
    while (window.pollEvent(event)) {
      if (event.type == sf::Event::Closed) {
        window.close();
        break;
      } else if (event.type == sf::Event::KeyPressed) {
        const sf::Keyboard::Key keycode = event.key.code;
        if (event.key.code < 0 || event.key.code >= sf::Keyboard::KeyCount) { continue; }
        if (game_mode == CREDITS) {
          UnlockMouse(window);
          scene.SetMode(Scene::INTRO);
          scene.SetExposure(1.0f);
          credits_music.stop();
          menu_music.play();
          game_mode = MAIN_MENU;
        } else if (keycode == sf::Keyboard::Escape) {
          if (game_mode == MAIN_MENU) {
            window.close();
            break;
          } else if (game_mode == CONTROLS || game_mode == LEVELS) {
            game_mode = MAIN_MENU;
            scene.SetExposure(1.0f);
          } else if (game_mode == SCREEN_SAVER) {
            scene.SetMode(Scene::INTRO);
            game_mode = MAIN_MENU;
          } else if (game_mode == PAUSED) {
            scene.GetCurMusic().setVolume(music_vol);
            scene.SetExposure(1.0f);
            LockMouse(window);
            game_mode = PLAYING;
          } else if (game_mode == PLAYING) {
            scene.GetCurMusic().setVolume(music_vol / 4);
            UnlockMouse(window);
            scene.SetExposure(0.5f);
            game_mode = PAUSED;
          }
        } else if (keycode == sf::Keyboard::R) {
          scene.ResetLevel();
        }
        all_keys[keycode] = true;
      } else if (event.type == sf::Event::KeyReleased) {
        const sf::Keyboard::Key keycode = event.key.code;
        if (event.key.code < 0 || event.key.code >= sf::Keyboard::KeyCount) { continue; }
        all_keys[keycode] = false;
      } else if (event.type == sf::Event::MouseButtonPressed) {
        if (event.mouseButton.button == sf::Mouse::Left) {
          mouse_pos = sf::Vector2i(event.mouseButton.x, event.mouseButton.y);
          if (game_mode == MAIN_MENU) {
            const Overlays::Texts selected = overlays.GetOption(Overlays::PLAY, Overlays::EXIT);
            if (selected == Overlays::PLAY) {
              menu_music.stop();
              game_mode = PLAYING;
              scene.StartNewGame();
              scene.GetCurMusic().setVolume(music_vol);
              scene.GetCurMusic().play();
              LockMouse(window);
            } else if (selected == Overlays::CONTROLS) {
              game_mode = CONTROLS;
            } else if (selected == Overlays::LEVELS) {
              game_mode = LEVELS;
              scene.SetExposure(0.5f);
            } else if (selected == Overlays::SCREEN_SAVER) {
              scene.SetMode(Scene::SCREEN_SAVER);
              game_mode = SCREEN_SAVER;
            } else if (selected == Overlays::EXIT) {
              window.close();
              break;
            }
          } else if (game_mode == CONTROLS) {
            const Overlays::Texts selected = overlays.GetOption(Overlays::BACK, Overlays::BACK);
            if (selected == Overlays::BACK) {
              game_mode = MAIN_MENU;
            }
          } else if (game_mode == LEVELS) {
            const Overlays::Texts selected = overlays.GetOption(Overlays::L0, Overlays::BACK2);
            if (selected == Overlays::BACK2) {
              game_mode = MAIN_MENU;
              scene.SetExposure(1.0f);
            } else if (selected >= Overlays::L0 && selected <= Overlays::L14) {
              if (high_scores.Has(selected - Overlays::L0)) {
                menu_music.stop();
                game_mode = PLAYING;
                scene.SetExposure(1.0f);
                scene.StartSingle(selected - Overlays::L0);
                scene.GetCurMusic().setVolume(music_vol);
                scene.GetCurMusic().play();
                LockMouse(window);
              }
            }
          } else if (game_mode == SCREEN_SAVER) {
            scene.SetMode(Scene::INTRO);
            game_mode = MAIN_MENU;
          } else if (game_mode == PAUSED) {
            const Overlays::Texts selected = overlays.GetOption(Overlays::QUIT, Overlays::MOUSE);
            if (selected == Overlays::QUIT) {
              scene.SetMode(Scene::INTRO);
              scene.StopAllMusic();
              menu_music.play();
              if (scene.IsSinglePlay()) {
                game_mode = LEVELS;
              } else {
                game_mode = MAIN_MENU;
                scene.SetExposure(1.0f);
              }
            } else if (selected == Overlays::CONTINUE) {
              scene.GetCurMusic().setVolume(music_vol);
              scene.SetExposure(1.0f);
              LockMouse(window);
              game_mode = PLAYING;
            } else if (selected == Overlays::MOUSE) {
              mouse_setting = (mouse_setting + 1) % 3;
            }
          }
        } else if (event.mouseButton.button == sf::Mouse::Right) {
          scene.ResetLevel();
        }
      } else if (event.type == sf::Event::MouseButtonReleased) {
        mouse_pos = sf::Vector2i(event.mouseButton.x, event.mouseButton.y);
      } else if (event.type == sf::Event::MouseMoved) {
        mouse_pos = sf::Vector2i(event.mouseMove.x, event.mouseMove.y);
      } else if (event.type == sf::Event::MouseWheelScrolled) {
        mouse_wheel += event.mouseWheelScroll.delta;
      }
    }

    //Check if the game was beat
    if (scene.GetMode() == Scene::FINAL && game_mode != CREDITS) {
      game_mode = CREDITS;
      scene.StopAllMusic();
      scene.SetExposure(0.5f);
      credits_music.play();
    }

    //Main game update
    if (game_mode == MAIN_MENU) {
      scene.UpdateCamera();
      overlays.UpdateMenu((float)mouse_pos.x, (float)mouse_pos.y);
    } else if (game_mode == CONTROLS) {
      scene.UpdateCamera();
      overlays.UpdateControls((float)mouse_pos.x, (float)mouse_pos.y);
    } else if (game_mode == LEVELS) {
      scene.UpdateCamera();
      overlays.UpdateLevels((float)mouse_pos.x, (float)mouse_pos.y);
    } else if (game_mode == SCREEN_SAVER) {
      scene.UpdateCamera();
    } else if (game_mode == PLAYING || game_mode == CREDITS) {
      //Collect keyboard input
      const float force_lr =
        (all_keys[sf::Keyboard::Left] || all_keys[sf::Keyboard::A] ? -1.0f : 0.0f) +
        (all_keys[sf::Keyboard::Right] || all_keys[sf::Keyboard::D] ? 1.0f : 0.0f);
      const float force_ud =
        (all_keys[sf::Keyboard::Down] || all_keys[sf::Keyboard::S] ? -1.0f : 0.0f) +
        (all_keys[sf::Keyboard::Up] || all_keys[sf::Keyboard::W] ? 1.0f : 0.0f);

      //Collect mouse input
      const sf::Vector2i mouse_delta = mouse_pos - screen_center;
      sf::Mouse::setPosition(screen_center, window);
      float ms = mouse_sensitivity;
      if (mouse_setting == 1) {
        ms *= 0.5f;
      } else if (mouse_setting == 2) {
        ms *= 0.25f;
      }
      const float cam_lr = float(-mouse_delta.x) * ms;
      const float cam_ud = float(-mouse_delta.y) * ms;
      const float cam_z = mouse_wheel * wheel_sensitivity;

      //Apply forces to marble and camera
      scene.UpdateMarble(force_lr, force_ud);
      scene.UpdateCamera(cam_lr, cam_ud, cam_z);
    } else if (game_mode == PAUSED) {
      overlays.UpdatePaused((float)mouse_pos.x, (float)mouse_pos.y);
    }

    //Update the shader values
    scene.Write(shader);

    //Setup full-screen shader
    sf::RenderStates states = sf::RenderStates::Default;
    states.shader = &shader;
    sf::RectangleShape rect;
    rect.setSize(window_res);
    rect.setPosition(0, 0);

    //Draw the fractal
    if (fullscreen) {
      //Draw to the render texture
      renderTexture.draw(rect, states);
      renderTexture.display();

      //Draw render texture to main window
      sf::Sprite sprite(renderTexture.getTexture());
      sprite.setScale(float(screen_size.width) / float(resolution->width),
                      float(screen_size.height) / float(resolution->height));
      window.draw(sprite);
    } else {
      //Draw directly to the main window
      window.draw(rect, states);
    }

    //Draw text overlays to the window
    if (game_mode == MAIN_MENU) {
      overlays.DrawMenu(window);
    } else if (game_mode == CONTROLS) {
      overlays.DrawControls(window);
    } else if (game_mode == LEVELS) {
      overlays.DrawLevels(window);
    } else if (game_mode == PLAYING) {
      if (scene.GetMode() == Scene::ORBIT && scene.GetMarble().x() < 998.0f) {
        overlays.DrawLevelDesc(window, scene.GetLevel());
      } else if (scene.GetMode() == Scene::MARBLE) {
        overlays.DrawArrow(window, scene.GetGoalDirection());
      }
      overlays.DrawTimer(window, scene.GetCountdownTime(), scene.IsHighScore());
    } else if (game_mode == PAUSED) {
      overlays.DrawPaused(window);
    } else if (game_mode == CREDITS) {
      overlays.DrawCredits(window);
    }
    overlays.DrawFPS(window, int(smooth_fps + 0.5f));

    //Finally display to the screen
    window.display();
    frame_num += 1;
    mouse_wheel = 0.0f;

    //If V-Sync is running higher than desired fps, slow down!
    const float s = clock.restart().asSeconds();
    if (s > 0.001f) {
      smooth_fps = smooth_fps*0.9f + (1.0f / s)*0.1f;
    }
    const int time_diff_ms = int(16.66667f - s*1000.0f);
    if (time_diff_ms > 0) {
      sf::sleep(sf::milliseconds(time_diff_ms));
    }
  }

  //Stop all music
  menu_music.stop();
  level1_music.stop();
  level2_music.stop();
  credits_music.stop();
  high_scores.Save(save_file);

#ifdef _DEBUG
  system("pause");
#endif
  return 0;
}
