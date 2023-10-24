#include "PhysicsHandle.h"
using namespace Yeager;
EntityPhysics::EntityPhysics(Yeager::GameEntity* entity, float weight,
                             float gravity_const)
    : m_entity(entity), m_weight(weight), m_gravity_const(gravity_const)
{
  Yeager::Log(INFO, kSystem,
              "Physics engine for entity {} ID {} was created with success",
              m_entity->GetName(), m_entity->GetId());
}

void EntityPhysics::ApplyGravity()
{
  if (m_gravity_enabled) {
    Transformation* trans = m_entity->GetTransformationPtr();
    if (!m_reverse_gravity) {
      trans->position.y -= m_weight * m_gravity_const * m_delta_brake;
    } else {
      trans->position.y += m_weight * m_gravity_const * m_delta_brake;
    }
  }
}
