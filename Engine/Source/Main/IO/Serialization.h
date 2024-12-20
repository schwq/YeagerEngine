// Yeager Engine, free and open source 3D / 2D renderer written in OpenGL
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

#include <yaml-cpp/yaml.h>
#include "Common/Utils/Common.h"
#include "Common/Utils/LogEngine.h"
#include "Common/Utils/Utilities.h"

#include "Components/Renderer/Objects/Entity.h"
#include "Editor/UI/Interface.h"
#include "Main/Window/Window.h"

/// @brief Creates a operator<< function that accepts the glm::vec3(aka Vector3) as input
extern YAML::Emitter& operator<<(YAML::Emitter& out, const Vector3& vector);

namespace YAML {
template <>
struct convert<Vector3> {
  static Node encode(const Vector3& rhs)
  {
    Node node;
    node.push_back(rhs.x);
    node.push_back(rhs.y);
    node.push_back(rhs.z);
    return node;
  }

  static bool decode(const Node& node, Vector3& rhs)
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

template <>
struct convert<ImVec4> {
  static Node encode(const ImVec4& rhs)
  {
    Node node;
    node.push_back(rhs.x);
    node.push_back(rhs.y);
    node.push_back(rhs.z);
    node.push_back(rhs.w);
    return node;
  }

  static bool decode(const Node& node, ImVec4& rhs)
  {
    if (!node.IsSequence() || node.size() != 4) {
      return false;
    }
    rhs.x = node[0].as<float>();
    rhs.y = node[1].as<float>();
    rhs.z = node[2].as<float>();
    rhs.w = node[3].as<float>();
    return true;
  }
};
}  // namespace YAML

namespace Yeager {
class Scene;
class ApplicationCore;
class Settings;
struct TemplateHandle;

extern std::vector<OpenProjectsDisplay> ReadProjectsToDisplay(String dir, Yeager::ApplicationCore* app);

/***
 * @brief Serialize and deserialize mostly of the engine required information, like scenes props, configurations files, and more. Every GameEntity is serialize 
 * with the needed information about itself */
class Serialization {
 public:
  Serialization(Yeager::ApplicationCore* app);

  ColorschemeConfig ReadColorschemeConfig();

  void ReadSceneShadersConfig(String path);
  void SerializeScene(Yeager::Scene* scene, String path);
  void DeserializeScene(Yeager::Scene* scene, String path);

  void ReadEngineConfiguration(const String& path);
  void WriteEngineConfiguration(const String& path);

  void WriteLoadedProjectsHandles(String externalFolder);
  void ReadLoadedProjectsHandles(String externalFolder);

  bool ReadEditorSoundsConfiguration(const String& path);

  TimePointType DeserializeProjectTimeOfCreation(YAML::Node node);

  std::vector<Yeager::TemplateHandle> FindTemplatesFromSharedFolder(const std::filesystem::path& path);
  Yeager::TemplateHandle ReadTemplateConfiguration(const std::filesystem::path& path,
                                                   const std::filesystem::path& folder);

  void DeserializeTemplateAssetsIntoScene(Yeager::Scene* scene, const std::filesystem::path& path);

  typedef std::map<String, String> LocaleData;
  std::optional<LocaleData> DeserializeLocaleData(const std::filesystem::path& path);

 private:
  Yeager::ApplicationCore* m_Application = YEAGER_NULLPTR;

  void YEAGER_FORCE_INLINE DeserializeEntity(Yeager::Scene* scene, YAML::Node& node,
                                             YAML::detail::iterator_value& entity);
  void YEAGER_FORCE_INLINE DeserializeSceneInfo(Yeager::Scene* scene, YAML::Node& node);
  ObjectGeometryType::Enum YEAGER_FORCE_INLINE DeserializeBasicObject(Yeager::Object* BaseClassObj,
                                                                      YAML::detail::iterator_value& entity);

  YEAGER_FORCE_INLINE std::vector<std::shared_ptr<Transformation3D>> DeserializeObjectProperties(
      YAML::detail::iterator_value& entity);
  void YEAGER_FORCE_INLINE SerializeBasicEntity(YAML::Emitter& out, String name, uuids::uuid uuid, String type);
  void YEAGER_FORCE_INLINE SerializeObjectTransformation(YAML::Emitter& out, String name,
                                                         Yeager::Transformation3D& transf) noexcept;

  template <typename Type>
  void YEAGER_FORCE_INLINE SerializeObject(YAML::Emitter& out, const char* key, Type obj);
  void YEAGER_FORCE_INLINE SerializeBasicObjectType(YAML::Emitter& out, Yeager::Object* obj);
  void YEAGER_FORCE_INLINE SerializeBegin(YAML::Emitter& out, const char* key, YAML::EMITTER_MANIP manip);
  void YEAGER_FORCE_INLINE SerializeSystemInfo(YAML::Emitter& out, Yeager::Scene* scene);

  template <typename Type>
  std::optional<Type> DeserializeObject(const YAML::Node& node, Cchar key);

  /**
   * @brief Given a node and a key, ensures that the key exists in the node, if so, the reference of
   * the given type T is assign to the value of the item stored inside the node with the given key.
   * @return True if the reference holds the correct value, false if not
   * @attention This function handles all YAML::Exception exceptions inside of it
   */
  template <typename T>
  bool DeserializeIfExistsIntoRef(const YAML::Node& node, const String& key, T& hold);

  void SerializeProjectTimeOfCreation(YAML::Emitter& out, Yeager::Scene* scene, const char* key);

  void DeserializeAudioHandle(YAML::detail::iterator_value& entity, Yeager::Scene* scene, const String& name,
                              const String& type, const uuids::uuid uuid);
  void DeserializeAudio3DHandle(YAML::detail::iterator_value& entity, Yeager::Scene* scene, const String& name,
                                const String& type, const uuids::uuid uuid);
  void DeserializeObject(YAML::detail::iterator_value& entity, Yeager::Scene* scene, const String& name,
                         const String& type, const uuids::uuid uuid);
  void DeserializeAnimatedObject(YAML::detail::iterator_value& entity, Yeager::Scene* scene, const String& name,
                                 const String& type, const uuids::uuid uuid);
  void DeserializeLightSource(YAML::detail::iterator_value& entity, Yeager::Scene* scene, const String& name,
                              const String& type, const uuids::uuid uuid);
  void DeserializeSkybox(YAML::detail::iterator_value& entity, Yeager::Scene* scene, const String& name,
                         const String& type, const uuids::uuid uuid);
};
}  // namespace Yeager