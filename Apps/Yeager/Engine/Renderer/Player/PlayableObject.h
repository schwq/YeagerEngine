//    Yeager Engine, free and open source 3D/2D renderer written in OpenGL
//    In case of questions and bugs, please, refer to the issue tab on github
//    Repo : https://github.com/schwq/YeagerEngine
//    Copyright (C) 2023
//
//    This program is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program.  If not, see <https://www.gnu.org/licenses/>.

#pragma once

#include "../../../Common/Common.h"
#include "../../../Common/LogEngine.h"
#include "../../../Common/Utilities.h"
#include "../Object.h"

namespace Yeager {
class ApplicationCore;

struct PlayableSetOfRules {
  bool FollowPlayerRotation = false;
  bool FollowPlayerScale = false;
  bool FollowPlayerPosition = false;
  bool LookAtPosition = false;
  Vector3 RotationOffset = Vector3(0.0f);
  Vector3 ScaleOffset = Vector3(0.0f);
  Vector3 PositionOffset = Vector3(0.0f);
  Vector3 PositionToLookAt = Vector3(0.0f);
  Vector3 PositonToLookAtOffset = Vector3(0.0f);
};

class PlayableObject : public Object {
 public:
  PlayableObject(String name, ApplicationCore* application);
  ~PlayableObject() {}

  virtual void Draw(Yeager::Shader* shader, float delta);

  PlayableSetOfRules* GetSetOfRules() { return &m_SetOfRules; }

 private:
  void ProcessPlayableSetOfRules();
  PlayableSetOfRules m_SetOfRules;
};

class PlayableAnimatedObject : public AnimatedObject {
 public:
  PlayableAnimatedObject(String name, ApplicationCore* application);
  ~PlayableAnimatedObject() {}

  virtual void Draw(Yeager::Shader* shader, float delta);

  PlayableSetOfRules* GetSetOfRules() { return &m_SetOfRules; }

 private:
  void ProcessPlayableSetOfRules();
  PlayableSetOfRules m_SetOfRules;
};

}  // namespace Yeager