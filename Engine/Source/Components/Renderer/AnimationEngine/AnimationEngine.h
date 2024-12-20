//    Yeager Engine, free and open source 3D/2D renderer written in OpenGL
//    In case of questions and bugs, please, refer to the issue tab on github
//    Repo : https://github.com/schwq/YeagerEngine
//    Copyright (C) 2023 - Present
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

#include "Common/Utils/Common.h"
#include "Common/Utils/LogEngine.h"
#include "Common/Utils/Utilities.h"

#include "Animation.h"

#define MAX_BONES 100

namespace Yeager {
class AnimationEngine {
 public:
  AnimationEngine();

  void Initialize();
  void LoadAnimationsFromFile(const String& path, AnimatedObject* model);

  void UpdateAnimation(float dt);
  void PlayAnimation(Animation* animation);
  void PlayAnimation(Uint index);
  void CalculateBoneTransform(const AssimpNodeData* node, Matrix4 parentTrans);
  std::vector<Matrix4> GetFinalBoneMatrices() const { return m_FinalBoneMatrices; }
  std::vector<Animation>* GetAnimations() { return &m_Animations; }

  bool IsAnimationsLoaded() const { return m_AnimationsLoaded; }
  bool IsPlayingAnimation() const { return m_PlayingAnimation; }

 protected:
  std::vector<Animation> m_Animations;
  std::vector<Matrix4> m_FinalBoneMatrices;
  Animation* m_CurrentAnimation = YEAGER_NULLPTR;
  float m_CurrentTime;
  float m_DeltaTime;
  bool m_PlayingAnimation = false;
  bool m_AnimationsLoaded = false;
};
}  // namespace Yeager