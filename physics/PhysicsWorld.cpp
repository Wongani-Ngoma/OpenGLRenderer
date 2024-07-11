#include "PhysicsWorld.h"

PhysicsWorld::PhysicsWorld()
{
}

void PhysicsWorld::init() {

    collisionConfiguration = new btDefaultCollisionConfiguration();
    dispatcher = new btCollisionDispatcher(collisionConfiguration);
    overlappingPairCache = new btDbvtBroadphase();
    solver = new btSequentialImpulseConstraintSolver;
    dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, overlappingPairCache, solver, collisionConfiguration);

    dynamicsWorld->setGravity(btVector3(0, -9.8, 0));

    numOfUniqueRigidBodies = 0;
}

void PhysicsWorld::addRigidBodyFromModel(Model* model) {

    // the arguments are half the sides of the box, so (1,1,1) will create a box with sides of two units
    glm::vec3 s{};

    s.x = model->rigidBody.size.x;
    s.y = model->rigidBody.size.y;
    s.z = model->rigidBody.size.z;

    btCollisionShape* shape = new btBoxShape(btVector3(btScalar(model->rigidBody.size.x), btScalar(model->rigidBody.size.y), btScalar(model->rigidBody.size.z)));
    
    //s.x = (float) shape->getLocalScaling().getX();
    //s.y = (float)shape->getLocalScaling().getY();
    //s.z = (float)shape->getLocalScaling().getZ();

    uniqueRigidBodyDimensions.insert(s);
    collisionShapes.push_back(shape);

    btTransform transform;
    transform.setIdentity();
    transform.setOrigin(btVector3(model->rigidBody.position.x, model->rigidBody.position.y, model->rigidBody.position.z));

    btScalar mass = model->rigidBody.mass;

    //rigidbody is dynamic if and only if mass is non zero, otherwise static
    bool isDynamic = (mass != 0.f);

    btVector3 localInertia(0, 0, 0);
    if (isDynamic)
        shape->calculateLocalInertia(mass, localInertia);

    //using motionstate is optional, it provides interpolation capabilities, and only synchronizes 'active' objects
    btDefaultMotionState* myMotionState = new btDefaultMotionState(transform);

    btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState,shape, localInertia);
    btRigidBody* body = new btRigidBody(rbInfo);

    //add the body to the dynamics world
    dynamicsWorld->addRigidBody(body);

    models.push_back(model);

}

void PhysicsWorld::resetSimulation() {

    delete dynamicsWorld;


    dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, overlappingPairCache, solver, collisionConfiguration);

    dynamicsWorld->setGravity(btVector3(0, -9.8, 0));

    /*for (int i = 0; i < uniqueRigidBodyDimensions.size(); i++) {
        btCollisionObject* obj = dynamicsWorld->getCollisionObjectArray()[i];
        btRigidBody* body = btRigidBody::upcast(obj); // rigid body is a sub class of collision object
        dynamicsWorld->removeRigidBody(body);
    }*/

    models.clear();
    collisionShapes.clear();
}

glm::vec3 PhysicsWorld::getVec3(btVector3 v) {

    return glm::vec3((float)v.getX(), (float)v.getY(), (float)v.getZ());;
}

glm::mat4 PhysicsWorld::getMat4(btScalar* m) {
    glm::mat4 mat(1.0);
    for (int i = 0; i < 4; i++) {
        for (int k = 0; k < 4; k++) {
            mat[i][k] = m[(4 * i) + k];
        }
    }
    return mat;
}


// currently useless
void PhysicsWorld::setPosition(Model& model, glm::vec3 newPos) {
    std::string n = model.name;
    int id = stoi(n.substr(n.find_last_not_of("model"), n.length() - 4)); // get the model's id, since model ids start at 0, can and is used as an index

    models[id]->rigidBody.position = newPos;

}

void PhysicsWorld::update(float timeStep) {
    dynamicsWorld->stepSimulation(timeStep, 10);

    //update positions of all btCollisionObjects in dynamic world, then send the new transformations to the actual models
    for(Model* m : models)
    {
        std::string n = m->name;
        int currentModel = stoi(n.substr(n.find_last_not_of("model"), n.length() - 4)); // get the model's id, since model ids start at 0, can and is used as an index
        btCollisionObject* obj = dynamicsWorld->getCollisionObjectArray()[currentModel];
        btRigidBody* body = btRigidBody::upcast(obj); // rigid body is a sub class of collision object
        btTransform trans;
        if (body && body->getMotionState())
        {
            body->getMotionState()->getWorldTransform(trans);
        }
        else
        {
            trans = obj->getWorldTransform();
        }
        // this openGLMatrix will contain a matrix telling the position and rotation of an object, we need a temp btScalar
        // array cause the getOpenGLMatrix accepts a btScalar* argument, then we transfer the contents in the btScalar array
        // to the glm matrix
        // we can also get the position as an isolated vec3 just in case we want to do something with it alone
        // i dont do it now cause that would cause the the models to be rendered with the physics position    ////
        glm::mat4 openGLMatrix;                                                                                 //
        btScalar m[16];                                                                                         //
        trans.getOpenGLMatrix(m);                                                                               //
        openGLMatrix = getMat4(m);                                                                              //
        btVector3 position = trans.getOrigin();                                                                 //
        glm::vec3 newPosition = getVec3(position);                                                              //
        //models[currentModel]->rigidBody.setPosition(newPosition);     </////////////////////////////////////////
        models[currentModel]->physicsModel = openGLMatrix;              
    }
}

void PhysicsWorld::clearCollisionShapes() {
    collisionShapes.clear();
    models.clear();

}

void PhysicsWorld::cleanup() {
    //cleanup in the reverse order of creation/initialization

    ///-----cleanup_start-----

    //remove the rigidbodies from the dynamics world and delete them
    for (int i = dynamicsWorld->getNumCollisionObjects() - 1; i >= 0; i--)
    {
        btCollisionObject* obj = dynamicsWorld->getCollisionObjectArray()[i];
        btRigidBody* body = btRigidBody::upcast(obj);
        if (body && body->getMotionState())
        {
            delete body->getMotionState();
        }
        dynamicsWorld->removeCollisionObject(obj);
        delete obj;
    }

    //delete collision shapes
    for (int j = 0; j < collisionShapes.size(); j++)
    {
        btCollisionShape* shape = collisionShapes[j];
        collisionShapes[j] = 0;
        delete shape;
    }

    //delete dynamics world
    delete dynamicsWorld;

    //delete solver
    delete solver;

    //delete broadphase
    delete overlappingPairCache;

    //delete dispatcher
    delete dispatcher;

    delete collisionConfiguration;

    //next line is optional: it will be cleared by the destructor when the array goes out of scope
    collisionShapes.clear();

    models.clear();
}
