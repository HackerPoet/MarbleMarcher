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
#pragma once
#include "Level.h"
#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <Eigen/Dense>

class Scene {
public:
  enum CamMode {
    INTRO,
    SCREEN_SAVER,
    ORBIT,
    DEORBIT,
    MARBLE,
    GOAL,
    FINAL,
  };

  Scene(sf::Music* m1, sf::Music* m2);

  void LoadLevel(int level);
  void SetMarble(float x, float y, float z, float r);
  void SetFlag(float x, float y, float z);
  void SetMode(CamMode mode);
  void SetExposure(float e) { exposure = e; }

  const Eigen::Vector3f& GetMarble() const { return marble_pos; };
  float GetCamLook() const { return cam_look_x_smooth; }
  CamMode GetMode() const { return cam_mode; }
  int GetLevel() const { return cur_level; }
  int GetCountdownTime() const;
  sf::Vector3f GetGoalDirection() const;
  bool IsSinglePlay() const { return play_single; }
  bool IsHighScore() const;

  sf::Music& GetCurMusic() const;
  void StopAllMusic();

  void StartNewGame(bool moon_gravity, bool scale_marble);
  void StartNextLevel();
  void StartSingle(int level, bool moon_gravity, bool scale_marble);
  void ResetLevel();

  void UpdateMarble(float dx=0.0f, float dy=0.0f);
  void UpdateCamera(float dx=0.0f, float dy=0.0f, float dz=0.0f);

  void SnapCamera();
  void HideObjects();

  void Write(sf::Shader& shader) const;

  float DE(const Eigen::Vector3f& pt) const;
  Eigen::Vector3f NP(const Eigen::Vector3f& pt) const;
  bool MarbleCollision(float& delta_v);

protected:
  void UpdateIntro(bool ssaver);
  void UpdateOrbit();
  void UpdateDeOrbit();
  void UpdateNormal(float dx, float dy, float dz);
  void UpdateGoal();
  void MakeCameraRotation();

private:
  int             cur_level;
  bool            intro_needs_snap;
  bool            play_single;

  Eigen::Matrix4f cam_mat;
  float           cam_look_x;
  float           cam_look_y;
  float           cam_dist;
  Eigen::Vector3f cam_pos;
  CamMode         cam_mode;

  float           cam_look_x_smooth;
  float           cam_look_y_smooth;
  float           cam_dist_smooth;
  Eigen::Vector3f cam_pos_smooth;

  float           marble_rad;
  Eigen::Vector3f marble_pos;
  Eigen::Vector3f marble_vel;
  Eigen::Matrix3f marble_mat;

  Eigen::Vector3f flag_pos;

  FractalParams   frac_params;
  FractalParams   frac_params_smooth;

  int             timer;
  int             final_time;
  float           exposure;

  sf::Sound sound_goal;
  sf::SoundBuffer buff_goal;
  sf::Sound sound_bounce1;
  sf::SoundBuffer buff_bounce1;
  sf::Sound sound_bounce2;
  sf::SoundBuffer buff_bounce2;
  sf::Sound sound_bounce3;
  sf::SoundBuffer buff_bounce3;
  sf::Sound sound_shatter;
  sf::SoundBuffer buff_shatter;

  sf::Music* music_1;
  sf::Music* music_2;
};
