#include "PhysicsWorld.h"

PhysicsWorld::PhysicsWorld()
    : m_collisionConfig{std::make_unique<btDefaultCollisionConfiguration>()}
    , m_dispatcher{std::make_unique<btCollisionDispatcher>(m_collisionConfig.get())}
    , m_overlappingPairCache{std::make_unique<btDbvtBroadphase>()}
    , m_solver{std::make_unique<btSequentialImpulseConstraintSolver>()}
    , m_dynamicsWorld{std::make_unique<btDiscreteDynamicsWorld>(
            m_dispatcher.get(), m_overlappingPairCache.get(), m_solver.get(), m_collisionConfig.get())}
    , m_dbgDrawer{std::make_unique<PhysicsDebugDraw>()}
{
    m_dynamicsWorld->setGravity(btVector3{0, -10, 0});
    m_dynamicsWorld->setDebugDrawer(m_dbgDrawer.get());
    m_dbgDrawer->setDebugMode(PhysicsDebugDraw::DBG_DrawWireframe);

    // TODO: Toggling debug draw mode with keybinding
}

void PhysicsWorld::updateDbgDrawUniforms(Camera& cam)
{
    m_dbgDrawer->updateUniforms(cam);
}

void PhysicsWorld::stepSimulation(float step)
{
    m_dynamicsWorld->stepSimulation(step, 10);
    m_dynamicsWorld->debugDrawWorld();
}

void PhysicsWorld::applyTransforms(std::vector<std::unique_ptr<GameObject>>& objs)
{
    for (size_t i{}; i < getObjectCount(); ++i)
    {
        auto cobj = getObj(i);
        assert(cobj);
        auto cbody = btRigidBody::upcast(cobj);
        assert(cbody);
        if (cbody)
        {
            auto trans = cobj->getWorldTransform();
            objs[i]->setPos(
                    {trans.getOrigin().getX(), trans.getOrigin().getY(), trans.getOrigin().getZ()});
            objs[i]->setRotationQuat(
                    {trans.getRotation().w(), trans.getRotation().x(),
                    trans.getRotation().y(), trans.getRotation().z()});
        }
    }
}

void PhysicsWorld::addObject(GameObject* obj)
{
    assert(obj);
    if (!obj->m_collShape) // If not a physics object
        return;            // FIXME: Handle it properly.
                           //        The indexes get messed up if we have a
                           //        game object that is not in the pworld list.
    obj->m_collShape->setMargin(0.001f); // TODO: What to set here?
    m_collisionShapes.push_back(obj->m_collShape);

    btTransform startTransform;
    startTransform.setIdentity();

    const bool isDynamic = (obj->m_mass != GameObject::MASS_STATIC);

    btVector3 localInertia{0, 0, 0};
    if (isDynamic)
        obj->m_collShape->calculateLocalInertia(obj->m_mass, localInertia);

    startTransform.setOrigin(btVector3{obj->m_pos.x, obj->m_pos.y, obj->m_pos.z});

    // FIXME: Fix memory leaks

    btDefaultMotionState* myMotionState = new btDefaultMotionState{startTransform};
    btRigidBody::btRigidBodyConstructionInfo rbInfo{
        obj->m_mass, myMotionState, obj->m_collShape, localInertia};
    btRigidBody* body = new btRigidBody{rbInfo};

    m_dynamicsWorld->addRigidBody(body);
}

btCollisionObject* PhysicsWorld::getObj(size_t i)
{
    if (i >= (size_t)m_dynamicsWorld->getNumCollisionObjects())
        return nullptr;
    return m_dynamicsWorld->getCollisionObjectArray()[i];
}