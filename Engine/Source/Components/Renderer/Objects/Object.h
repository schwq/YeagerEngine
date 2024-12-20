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

#include "Components/Physics/PhysXActor.h"
#include "Components/Physics/PhysXHandle.h"
#include "Components/Renderer/AnimationEngine/Bone.h"
#include "Components/Renderer/GL/OpenGLRender.h"
#include "Components/Renderer/Objects/Entity.h"
#include "Editor/UI/ToolboxObj.h"

namespace Yeager {
class ApplicationCore;
class Animation;
class AnimationEngine;
class Importer;
class ImporterThreaded;
class ImporterThreadedAnimated;

struct CustomTextureFolder {
  CustomTextureFolder() {}
  String path = YEAGER_NULL_LITERAL;
  bool Valid = false;
};

struct ObjectCreationConfiguration {
  ObjectCreationConfiguration() {}
  void ResetValues()
  {
    TextureFolder.path = YEAGER_NULL_LITERAL;
    TextureFolder.Valid = false;
  }
  CustomTextureFolder TextureFolder;
};

struct BoneInfo {
  int ID = -1;
  Matrix4 OffSet = Matrix4(1.0f);
};

struct ObjectVertexData {
  Vector3 Position = Vector3(0.0f);
  Vector3 Normals = Vector3(0.0f);
  Vector2 TextureCoords = Vector2(0.0f);
};

#define MAX_BONE_INFLUENCE 4

struct AnimatedVertexData : public ObjectVertexData {
  Vector3 Tangent = Vector3(0.0f);
  Vector3 BiTangent = Vector3(0.0f);
  int BonesIDs[MAX_BONE_INFLUENCE];
  float Weights[MAX_BONE_INFLUENCE];
};

struct CommonMeshData {
  std::vector<MaterialTexture2D*> Textures;
  std::vector<GLuint> Indices;
  CommonMeshData(const std::vector<MaterialTexture2D*>& textures, const std::vector<GLuint>& indices)
  {
    Textures = textures;
    Indices = indices;
  }
  ElementBufferRenderer Renderer;
};

struct ObjectMeshData : public CommonMeshData {
  std::vector<ObjectVertexData> Vertices;
  ObjectMeshData(std::vector<GLuint> indices, std::vector<ObjectVertexData> vertices,
                 std::vector<MaterialTexture2D*> textures)
      : CommonMeshData(textures, indices)
  {
    Vertices = vertices;
  }
};

struct AnimatedObjectMeshData : public CommonMeshData {
  std::vector<AnimatedVertexData> Vertices;
  AnimatedObjectMeshData(std::vector<GLuint> indices, std::vector<AnimatedVertexData> vertices,
                         std::vector<MaterialTexture2D*> textures)
      : CommonMeshData(textures, indices)
  {
    Vertices = vertices;
  }
};

struct CommonModelData {
  /* TODO remake this */
  /* A vector of shared pointers of pairs
  First: MaterialTexture2D the texture loaded to the entity 
  Second: STBIDataOutput pointer linking data read during thread importer to be process in the main thread */
  std::vector<std::shared_ptr<std::pair<MaterialTexture2D, STBIDataOutput*>>> TexturesLoaded;
  bool SuccessfulLoaded = false;
};

struct ObjectModelData : public CommonModelData {
  std::vector<ObjectMeshData> Meshes;
  ~ObjectModelData() noexcept { Yeager::Log(INFO, "Destrorying model data!"); }
};

struct AnimatedObjectModelData : public CommonModelData {
  std::vector<AnimatedObjectMeshData> Meshes;
  std::map<String, BoneInfo> m_BoneInfoMap;
  int m_BoneCounter = 0;
  auto& GetBoneInfoMap() { return m_BoneInfoMap; }
  int& GetBoneCount() { return m_BoneCounter; }
};

struct ObjectGeometryData {
  std::vector<GLuint> Indices;
  std::vector<GLfloat> Vertices;
  MaterialTexture2D* Texture = YEAGER_NULLPTR;
  ElementBufferRenderer Renderer;
};

struct ObjectGeometryType {
  enum Enum { eCUBE, eTRIANGLE, eSPHERE, eCUSTOM };
};

struct RenderingGLPolygonMode {
  enum Enum { eFILL, ePOINTS, eLINES };
};

struct ObjectInstancedType {
  enum Enum { eINSTANCED, eNON_INSTACED };
  static String EnumToString(ObjectInstancedType::Enum e);
};

struct ObjectOnScreenProprieties {
  bool m_CullingEnabled = true;
  RenderingGLPolygonMode::Enum m_PolygonMode = RenderingGLPolygonMode::eFILL;
};

extern String ObjectGeometryTypeToString(ObjectGeometryType::Enum type);
extern ObjectGeometryType::Enum StringToObjectGeometryType(const String& str);
enum class YeagerTextureType { EDiffuse, ESpecular };
extern std::vector<GLfloat> GenerateCubeVertices();
extern std::vector<GLuint> GenerateCubeIndices();
extern std::vector<GLfloat> GenerateSphereVertices(int stackCount, int sectorCount);
extern std::vector<GLuint> GenerateSphereIndices(int stackCount, int sectorCount);
extern void DeleteMeshGLBuffers(ObjectMeshData* mesh);
extern void DrawSeparateMesh(ObjectMeshData* mesh, Yeager::Shader* shader);
extern void DrawSeparateInstancedMesh(ObjectMeshData* mesh, Yeager::Shader* shader, int amount);
extern std::vector<GLfloat> ExtractVerticesFromEveryMesh(ObjectModelData* model);
extern std::vector<Vector3> ExtractVerticesPositionToVector(ObjectModelData* model);

extern void SpawnCubeObject(Yeager::ApplicationCore* application, const String& name, const Vector3& position,
                            const Vector3& rotation, const Vector3& scale,
                            const ObjectPhysicsType::Enum physics = ObjectPhysicsType::eUNDEFINED);

extern void SpawnSphereObject(Yeager::ApplicationCore* application, const String& name, const Vector3& position,
                              const Vector3& rotation, const Vector3& scale,
                              const ObjectPhysicsType::Enum physics = ObjectPhysicsType::eUNDEFINED);

class Object : public GameEntity {
 public:
  Object(const EntityBuilder& builder);
  Object(const EntityBuilder& builder, GLuint amount);
  ~Object();

  virtual bool ImportObjectFromFile(Cchar path,
                                    const ObjectCreationConfiguration configuration = ObjectCreationConfiguration(),
                                    bool flip_image = false);
  virtual bool ThreadImportObjectFromFile(
      Cchar path, const ObjectCreationConfiguration configuration = ObjectCreationConfiguration(),
      bool flip_image = false);
  virtual void ThreadSetup();
  bool GenerateObjectGeometry(ObjectGeometryType::Enum geometry, const ObjectPhysXCreationBase& physics);
  virtual void Draw(Yeager::Shader* shader, float delta);

  constexpr YEAGER_FORCE_INLINE ObjectGeometryType::Enum GetGeometry() { return m_GeometryType; }
  constexpr YEAGER_FORCE_INLINE void SetGeometry(ObjectGeometryType::Enum type) { m_GeometryType = type; }
  constexpr YEAGER_FORCE_INLINE ObjectModelData* GetModelData() { return &m_ModelData; }
  constexpr YEAGER_FORCE_INLINE ObjectGeometryData* GetGeometryData() { return &m_GeometryData; }
  YEAGER_FORCE_INLINE String GetPath() { return Path; }
  constexpr inline bool IsLoaded() const { return m_ObjectDataLoaded; }

  virtual void BuildProps(const std::vector<std::shared_ptr<Transformation3D>>& transformations, Shader* shader);

  void GenerateGeometryTexture(MaterialTexture2D* texture);

  std::shared_ptr<PhysXActor> GetPhysXActor() { return m_Actor; }

  YEAGER_FORCE_INLINE ObjectPhysicsType::Enum GetObjectPhysicsType() { return m_PhysicsType; }

  YEAGER_FORCE_INLINE GLuint GetInstancedNum()
  {
    if (m_InstancedType == ObjectInstancedType::eINSTANCED) {
      return m_InstancedObjs;
    }
    Yeager::Log(WARNING, "Getting instanced object number from a non instanced object! {}", mName);
    return 1;
  }

  YEAGER_NODISCARD YEAGER_FORCE_INLINE std::vector<std::shared_ptr<Transformation3D>>* GetInstancedProps()
  {
    if (m_InstancedType == ObjectInstancedType::eINSTANCED) {
      return &m_Props;
    }
    Yeager::Log(WARNING, "Getting instanced props from a non instanced object! {}", mName);
    return YEAGER_NULLPTR;
  }

  YEAGER_FORCE_INLINE bool IsInstanced() const { return (m_InstancedType == ObjectInstancedType::eINSTANCED); }
  YEAGER_FORCE_INLINE ObjectInstancedType::Enum GetInstancedType() { return m_InstancedType; }

  YEAGER_FORCE_INLINE ObjectOnScreenProprieties* GetOnScreenProprieties() { return &m_OnScreenProprieties; }
  YEAGER_FORCE_INLINE void SetCullingEnabled(bool cull) { m_ObjectDataLoaded = cull; }

 protected:
  virtual void Setup();
  virtual void DrawGeometry(Yeager::Shader* shader);
  virtual void DrawInstancedGeometry(Yeager::Shader* shader);
  virtual void DrawModel(Yeager::Shader* shader);

  virtual void ThreadLoadIncompleteTextures();

  String Path;
  bool m_ObjectDataLoaded = false;

  ObjectOnScreenProprieties m_OnScreenProprieties;
  virtual void ProcessOnScreenProprieties();     // Before calling the drawing openGL
  virtual void PosProcessOnScreenProprieties();  // After calling the drawing openGL to sets everything to the default

  std::shared_ptr<Yeager::PhysXActor> m_Actor = YEAGER_NULLPTR;
  ObjectPhysicsType::Enum m_PhysicsType = ObjectPhysicsType::eUNDEFINED;

  ObjectModelData m_ModelData;
  ObjectGeometryData m_GeometryData;
  ObjectGeometryType::Enum m_GeometryType;
  ObjectInstancedType::Enum m_InstancedType = ObjectInstancedType::eNON_INSTACED;
  std::shared_ptr<ImporterThreaded> m_ThreadImporter = YEAGER_NULLPTR;
  GLuint m_InstancedObjs = 1;
  std::vector<std::shared_ptr<Transformation3D>> m_Props;
};

class AnimatedObject : public Object {
 public:
  AnimatedObject(const EntityBuilder& builder);
  AnimatedObject(const EntityBuilder& builder, GLuint amount);
  ~AnimatedObject();
  bool ImportObjectFromFile(Cchar path, const ObjectCreationConfiguration configuration = ObjectCreationConfiguration(),
                            bool flip_image = false);
  virtual void Draw(Shader* shader);
  bool ThreadImportObjectFromFile(Cchar path,
                                  const ObjectCreationConfiguration configuration = ObjectCreationConfiguration(),
                                  bool flip_image = false);
  virtual void ThreadSetup();
  AnimatedObjectModelData* GetModelData() { return &m_ModelData; }

  std::shared_ptr<AnimationEngine> GetAnimationEngine() { return m_AnimationEngine; }

  void UpdateAnimation(float delta);
  void BuildAnimationMatrices(Shader* shader);
  void BuildAnimation(String path);
  void ThreadLoadIncompleteTextures();

 protected:
  void Setup();
  void DrawMeshes(Shader* shader);
  AnimatedObjectModelData m_ModelData;
  std::shared_ptr<AnimationEngine> m_AnimationEngine = YEAGER_NULLPTR;
  std::shared_ptr<ImporterThreadedAnimated> m_ThreadImporter = YEAGER_NULLPTR;
};

}  // namespace Yeager
