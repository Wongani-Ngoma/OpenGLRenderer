#ifndef PHYSICS_WORLD
#define PHYSICS_WORLD

#include <map>
#include <algorithm>
#include "btBulletDynamicsCommon.h"
#include "../graphics/Model.h"
#include <set>

class PhysicsWorld {
public:
    PhysicsWorld();
    void init();
    void addRigidBodyFromModel(Model* model);
    void resetSimulation();
    glm::vec3 getVec3(btVector3 v);
    glm::mat4 getMat4(btScalar* m);
    void setPosition(Model& model, glm::vec3 newPos);
    void update(float timeStep);
    void clearCollisionShapes();
    void cleanup();

private:
    //collision configuration contains default setup for memory, collision setup. Advanced users can create their own configuration.
    btDefaultCollisionConfiguration* collisionConfiguration;

    //use the default collision dispatcher. For parallel processing you can use a diffent dispatcher (see Extras/BulletMultiThreaded)
    btCollisionDispatcher* dispatcher;

    //btDbvtBroadphase is a good general purpose broadphase. You can also try out btAxis3Sweep.
    btBroadphaseInterface* overlappingPairCache;

    //the default constraint solver. For parallel processing you can use a different solver (see Extras/BulletMultiThreaded)
    btSequentialImpulseConstraintSolver* solver;

    btDiscreteDynamicsWorld* dynamicsWorld;

    // shapes have a unit size (usually 1), the arguments here specify the scale
    btCollisionShape* shape;
    btAlignedObjectArray<btCollisionShape*> collisionShapes;

    //std::map<std::string, Model&> models;
    std::vector<Model*> models;

    struct Vec3Comparator {
        bool operator()(const glm::vec3& lhs, const glm::vec3& rhs) const {
            if (lhs.x != rhs.x) return lhs.x < rhs.x;
            if (lhs.y != rhs.y) return lhs.y < rhs.y;
            return lhs.z < rhs.z;
        }
    };

    std::set<glm::vec3, Vec3Comparator> uniqueRigidBodyDimensions;

    int numOfUniqueRigidBodies;


};

#endif

