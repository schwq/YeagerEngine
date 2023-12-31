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

#include "../../Common/Common.h"
#include "../../Common/LogEngine.h"
#include "../../Common/Utilities.h"
#include "../Editor/ToolboxObj.h"
#include "../Physics/PhysicsHandle.h"
#include "AABBCollision.h"
#include "Animation/Bone.h"
#include "Entity.h"

namespace Yeager {
class ApplicationCore;
class Animation;
class AnimationEngine;

struct BoneInfo {
  int ID;
  YgMatrix4 OffSet;
};

struct ObjectTexture {
  bool FlipImage = true;
  YgString Path;
  YgString Type;
  YgString Name;
  GLuint ID;
};

struct ObjectVertexData {
  YgVector3 Position;
  YgVector3 Normals;
  YgVector2 TextureCoords;
};

#define MAX_BONE_INFLUENCE 4

struct AnimatedVertexData : public ObjectVertexData {
  YgVector3 Tangent;
  YgVector3 BiTangent;
  int BonesIDs[MAX_BONE_INFLUENCE];
  float Weights[MAX_BONE_INFLUENCE];
};

struct CommonMeshData {
  std::vector<ObjectTexture> Textures;
  std::vector<GLuint> Indices;
  CommonMeshData(const std::vector<ObjectTexture>& textures, const std::vector<GLuint>& indices)
  {
    Textures = textures;
    Indices = indices;
  }
  GLuint m_Vao = -1, m_Vbo = -1, m_Ebo = -1;
};

struct ObjectMeshData : public CommonMeshData {
  std::vector<ObjectVertexData> Vertices;
  ObjectMeshData(std::vector<GLuint> indices, std::vector<ObjectVertexData> vertices,
                 std::vector<ObjectTexture> textures)
      : CommonMeshData(textures, indices)
  {
    Vertices = vertices;
  }
};

struct AnimatedObjectMeshData : public CommonMeshData {
  std::vector<AnimatedVertexData> Vertices;
  AnimatedObjectMeshData(std::vector<GLuint> indices, std::vector<AnimatedVertexData> vertices,
                         std::vector<ObjectTexture> textures)
      : CommonMeshData(textures, indices)
  {
    Vertices = vertices;
  }
};

struct CommonModelData {
  std::vector<ObjectTexture> TexturesLoaded;
  bool SuccessfulLoaded = false;
};

struct ObjectModelData : public CommonModelData {
  std::vector<ObjectMeshData> Meshes;
};

struct AnimatedObjectModelData : public CommonModelData {
  std::vector<AnimatedObjectMeshData> Meshes;
  std::map<YgString, BoneInfo> m_BoneInfoMap;
  int m_BoneCounter = 0;
  auto& GetBoneInfoMap() { return m_BoneInfoMap; }
  int& GetBoneCount() { return m_BoneCounter; }
};

struct ObjectGeometryData {
  std::vector<GLuint> Indices;
  std::vector<GLfloat> Vertices;
  std::vector<ObjectTexture> Textures;
  GLuint m_Vao, m_Vbo, m_Ebo;
};

enum class ObjectGeometryType { ECube, ETriangule, ESphere, ECustom };

extern YgString ObjectGeometryTypeToString(ObjectGeometryType type);
extern ObjectGeometryType StringToObjectGeometryType(const YgString& str);

enum class YeagerTextureType { EDiffuse, ESpecular };

extern std::vector<GLfloat> GenerateCubeVertices();

extern std::vector<GLuint> GenerateCubeIndices();

extern std::vector<GLfloat> GenerateSphereVertices(int stackCount, int sectorCount);

extern std::vector<GLuint> GenerateSphereIndices(int stackCount, int sectorCount);

extern std::vector<GLfloat> GenerateAllDataIntoVector(ObjectModelData* data);

extern void DeleteMeshGLBuffers(ObjectMeshData* mesh);

extern bool DrawSeparateMesh(ObjectMeshData* mesh, Yeager::Shader* shader);

extern bool DrawSeparateInstancedMesh(ObjectMeshData* mesh, Yeager::Shader* shader, int amount);

extern bool DrawSeparateInstancedAnimatedMesh(AnimatedObjectMeshData* mesh, Yeager::Shader* shader, int amount);

extern std::vector<GLfloat> ExtractVerticesFromEveryMesh(ObjectModelData* model);
extern std::vector<YgVector3> ExtractVerticesPositionToVector(ObjectModelData* model);

class Object : public GameEntity {
 public:
  Object(YgString name, ApplicationCore* application);
  ~Object();

  virtual bool ImportObjectFromFile(YgCchar path, bool flip_image = false);
  bool GenerateObjectGeometry(ObjectGeometryType geometry);
  virtual void Draw(Yeager::Shader* shader);

  constexpr inline ObjectGeometryType GetGeometry() { return m_GeometryType; }
  constexpr inline void SetGeometry(ObjectGeometryType type) { m_GeometryType = type; }
  constexpr inline ObjectModelData* GetModelData() { return &m_ModelData; }
  constexpr inline ObjectGeometryData* GetGeometryData() { return &m_GeometryData; }
  constexpr inline EntityPhysics* GetPhysics() { return &m_Physics; }
  inline YgString GetPath() { return m_Path; }

 protected:
  virtual void Setup();
  virtual void DrawGeometry(Yeager::Shader* shader);
  virtual void DrawModel(Yeager::Shader* shader);

  YgString m_Path;
  bool m_ObjectDataLoaded = false;
  ApplicationCore* m_Application;
  ObjectModelData m_ModelData;
  ObjectGeometryData m_GeometryData;
  ObjectGeometryType m_GeometryType;
  EntityPhysics m_Physics;
  std::shared_ptr<ToolBoxObject> m_Toolbox;
};

class InstancedObject : public Object {
 public:
  InstancedObject(YgString name, ApplicationCore* application, GLuint number) : Object(name, application)
  {
    m_InstancedObjs = number;
  }
  ~InstancedObject() {}

  void Draw(Yeager::Shader* shader, int amount);
  void BuildProp(std::vector<Transformation>& positions, Shader* shader);
  std::vector<Transformation>* GetProps() { return &m_Props; }

  GLuint GetInstancedNumber() const { return m_InstancedObjs; }

 protected:
  void DrawGeometry(Yeager::Shader* shader);
  std::vector<Transformation> m_Props;
  void DrawModel(Yeager::Shader* shader, int amount);
  GLuint m_InstancedObjs = 0;
};

class AnimatedObject : public Object {
 public:
  AnimatedObject(YgString name, ApplicationCore* application);
  ~AnimatedObject();
  bool ImportObjectFromFile(YgCchar path, bool flip_image = false);
  virtual void Draw(Shader* shader);
  AnimatedObjectModelData* GetModelData() { return &m_ModelData; }

  void UpdateAnimation(float delta);
  void BuildAnimationMatrices(Shader* shader);
  void BuildAnimation(YgString path);

 protected:
  void Setup();
  void DrawMeshes(Shader* shader);
  AnimatedObjectModelData m_ModelData;
  Animation* m_Animation = YEAGER_NULLPTR;
  AnimationEngine* m_AnimationEngine = YEAGER_NULLPTR;
};

class InstancedAnimatedObject : public AnimatedObject {
 public:
  InstancedAnimatedObject(YgString name, ApplicationCore* application, GLuint number)
      : AnimatedObject(name, application)
  {
    m_InstancedObjs = number;
    m_AABBCollisions.reserve(number);
    m_Props.reserve(number);
  }
  ~InstancedAnimatedObject() {}

  void Draw(Yeager::Shader* shader);
  void BuildProp(std::vector<Transformation>& positions, Shader* shader);
  std::vector<Transformation>* GetProps() { return &m_Props; }

  GLuint GetInstancedNumber() const { return m_InstancedObjs; }

 protected:
  std::vector<AABBCollision> m_AABBCollisions;
  std::vector<Transformation> m_Props;
  void DrawAnimatedMeshes(Shader* shader);
  GLuint m_InstancedObjs = 0;
};

}  // namespace Yeager
