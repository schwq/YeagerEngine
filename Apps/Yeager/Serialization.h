//    Yeager Engine, free and open source 3D/2D renderer written in OpenGL
//    In case of questions and bugs, please, refer to the issue tab on github
//    Repo : https://github.com/schwq/yeager-engine

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

#include <yaml-cpp/yaml.h>
#include "Common/Common.h"
#include "Common/LogEngine.h"
#include "Common/Utilities.h"
#include "Engine/Interface/Interface.h"
#include "Engine/Renderer/Entity.h"

namespace Yeager {
class ApplicationCore;
}

namespace YAML {
template <>
struct convert<yg_vec3> {
  static Node encode(const yg_vec3& rhs)
  {
    Node node;
    node.push_back(rhs.x);
    node.push_back(rhs.y);
    node.push_back(rhs.z);
    return node;
  }

  static bool decode(const Node& node, yg_vec3& rhs)
  {
    if (!node.IsSequence() || node.size() != 3) {
      return false;
    }
    rhs.x = node[0].as<float>();
    rhs.y = node[1].as<float>();
    rhs.z = node[2].as<float>();
    return true;
  }
};
}  // namespace YAML

namespace Yeager {
class Scene;
}

namespace Yeager {

extern YAML::Emitter& operator<<(YAML::Emitter& out, const yg_vec3& vector);

class Serialization {
 public:
  Serialization(Yeager::ApplicationCore* app);
  Serialization() {}
  ~Serialization(){YEAGER_NOT_IMPLEMENTED("~Serialization")};
  ColorschemeConfig ReadColorschemeConfig();
  void ReadSceneShadersConfig(yg_string path);

  void SerializeScene(Yeager::Scene* scene, yg_string path);
  void DeserializeScene(Yeager::Scene* scene, yg_string path);

  void ReadConf(yg_string path){YEAGER_NOT_IMPLEMENTED("ReadConf")};
  void SaveConf(){YEAGER_NOT_IMPLEMENTED("SaveConf")};

 private:
  Yeager::ApplicationCore* m_app = nullptr;
  void inline DeserializeEntity(Yeager::Scene* scene, YAML::Node& node, YAML::detail::iterator_value& entity);
  void inline DeserializeSceneInfo(Yeager::Scene* scene, YAML::Node& node);
  void inline SerialBasicEntity(YAML::Emitter& out, yg_string name, yg_uint id, yg_string type);
  template <typename _T>
  void inline SerialObj(YAML::Emitter& out, const char* key, _T obj);
  void inline SerialBegin(YAML::Emitter& out, const char* key, YAML::EMITTER_MANIP manip);
};
}  // namespace Yeager