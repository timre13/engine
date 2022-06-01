#pragma once

#include <memory>
#include "GameObject.h"
#include "PhysicsDebugDraw.h"
#include <bullet/BulletCollision/btBulletCollisionCommon.h>
#include <bullet/BulletDynamics/ConstraintSolver/btSequentialImpulseConstraintSolver.h>
#include <bullet/BulletDynamics/Dynamics/btDiscreteDynamicsWorld.h>

class PhysicsWorld final
{
private:
    std::unique_ptr<btDefaultCollisionConfiguration> m_collisionConfig;
    std::unique_ptr<btCollisionDispatcher> m_dispatcher;
    std::unique_ptr<btBroadphaseInterface> m_overlappingPairCache;
    std::unique_ptr<btSequentialImpulseConstraintSolver> m_solver;
    std::unique_ptr<btDynamicsWorld> m_dynamicsWorld;
    btAlignedObjectArray<btCollisionShape*> m_collisionShapes;

    std::unique_ptr<PhysicsDebugDraw> m_dbgDrawer;

public:
    PhysicsWorld();

    void setDbgMode(PhysicsDebugDraw::DebugDrawModes mode);
    void setDbgMode(int mode);
    PhysicsDebugDraw::DebugDrawModes getDbgMode() const;
    void updateDbgDrawUniforms(Camera& cam);
    void stepSimulation(float step);
    void applyTransforms(std::vector<std::unique_ptr<GameObject>>& objs);

    void addObject(GameObject* obj);
    inline size_t getObjectCount() const { return m_dynamicsWorld->getNumCollisionObjects(); };
    btCollisionObject* getObj(size_t i);

    inline btDynamicsWorld* getWorld() { return m_dynamicsWorld.get(); }
};

