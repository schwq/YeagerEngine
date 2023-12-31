#include "AnimationEngine.h"
using namespace Yeager;

AnimationEngine::AnimationEngine(Animation* animation)
{
  m_CurrentTime = 0.0f;
  m_CurrentAnimation = animation;
  m_FinalBoneMatrices.reserve(MAX_BONES);
  for (int x = 0; x < MAX_BONES; x++) {
    m_FinalBoneMatrices.push_back(YgMatrix4(1.0f));
  }
}

void AnimationEngine::UpdateAnimation(float dt)
{
  m_DeltaTime = dt;
  if (m_CurrentAnimation) {
    m_CurrentTime += m_CurrentAnimation->GetTicksPerSecond() * dt;
    m_CurrentTime = fmod(m_CurrentTime, m_CurrentAnimation->GetDuration());
    CalculateBoneTransform(&m_CurrentAnimation->GetRootNode(), YgMatrix4(1.0f));
  }
}
void AnimationEngine::PlayAnimation(Animation* animation)
{
  m_CurrentAnimation = animation;
  m_CurrentTime = 0.0f;
}

void AnimationEngine::CalculateBoneTransform(const AssimpNodeData* node, YgMatrix4 parentTrans)
{

  assert(node);

  YgString nodeName = node->Name;

  YgMatrix4 nodeTransform = node->Transformation;

  Bone* Bone = m_CurrentAnimation->FindBone(nodeName);

  if (Bone) {
    Bone->Update(m_CurrentTime);
    nodeTransform = Bone->GetLocalTransform();
  }

  YgMatrix4 globalTransformation = parentTrans * nodeTransform;

  auto boneInfoMap = m_CurrentAnimation->GetBoneIDMap();

  if (boneInfoMap.find(nodeName) != boneInfoMap.end()) {
    int index = boneInfoMap[nodeName].ID;
    YgMatrix4 offset = boneInfoMap[nodeName].OffSet;
    m_FinalBoneMatrices[index] = globalTransformation * offset;
  }

  for (int x = 0; x < node->ChildrenCount; x++) {
    CalculateBoneTransform(&node->Children[x], globalTransformation);
  }
}