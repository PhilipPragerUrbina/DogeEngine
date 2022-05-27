//
// Created by Philip on 5/14/2022.
//

#ifndef DOGEENGINE_PHYSICSMANAGER_HPP
#define DOGEENGINE_PHYSICSMANAGER_HPP
#include "Bullet/btBulletDynamicsCommon.h"
#include <glm/gtx/matrix_decompose.hpp>

class PhysicsManager {
public:

PhysicsManager(){
    //create systems
    collisionConfiguration = new btDefaultCollisionConfiguration () ;
    dispatcher = new btCollisionDispatcher ( collisionConfiguration ) ;
    overlappingPairCache = new btDbvtBroadphase () ;
    solver = new btSequentialImpulseConstraintSolver ;
    //create world
    dynamicsWorld = new btDiscreteDynamicsWorld ( dispatcher ,overlappingPairCache , solver , collisionConfiguration );
    //set settings
    dynamicsWorld  -> setGravity ( btVector3 (0 , -10 ,0) ) ;
}

    void addObject(Node* object, float mass) {


        if (object->m_object != nullptr && !object->m_physics) {
            btCollisionShape *boxCollisionShape = new btConvexHullShape((btScalar *) object->m_object->m_vertices.data(),
                                                                        object->m_object->m_vertices.size() / 14,
                                                                        14 * sizeof(float));

            object->m_physics = true;
            glm::vec3 scale;
            glm::quat rotation;
            glm::vec3 translation;
            glm::vec3 skew;
            glm::vec4 perspective;
            glm::decompose(object->m_global_transform, scale, rotation, translation, skew, perspective);
            boxCollisionShape->setLocalScaling(btVector3(scale.x,scale.y,scale.z));
            btDefaultMotionState *motionstate = new btDefaultMotionState(btTransform(
                    btQuaternion(rotation.x, rotation.y, rotation.z, rotation.w),
                    btVector3(translation.x, translation.y, translation.z)
            ));

            btRigidBody::btRigidBodyConstructionInfo rigidBodyCI(mass,motionstate,boxCollisionShape,  btVector3(1, 1, 1));
            btRigidBody *rigidBody = new btRigidBody(rigidBodyCI);
            dynamicsWorld->addRigidBody(rigidBody);
            rigidbodies.push_back(rigidBody);
            rigidBody->setUserPointer(object);
        }
        for(Node* child : object->m_children){
            addObject(child, mass);
        }
    }



void update(){
    dynamicsWorld->stepSimulation(1.0f/60.0f,10);
    for(btRigidBody* body : rigidbodies){
         if(body->getMass() > 0){
            btTransform transform;
            body->getMotionState()->getWorldTransform(transform);
            Node* node = (Node*)body->getUserPointer();
            transform.getOpenGLMatrix((float*)&node->m_global_transform);
            node->m_global_transform = glm::scale(node->m_global_transform ,glm::vec3(body->getCollisionShape()->getLocalScaling().x(),body->getCollisionShape()->getLocalScaling().y(),body->getCollisionShape()->getLocalScaling().z()));
            node->applyTransform();
            if(node->m_object->force != glm::vec3(0)){
                body->activate();
                body->applyCentralForce(btVector3(node->m_object->force.x,node->m_object->force.y,node->m_object->force.z));
            }

        }
    }
}



private:
    btDefaultCollisionConfiguration * collisionConfiguration;
    btCollisionDispatcher* dispatcher;
    btBroadphaseInterface* overlappingPairCache;
    btSequentialImpulseConstraintSolver* solver;
    btDiscreteDynamicsWorld * dynamicsWorld;

    std::vector<btRigidBody*> rigidbodies;
};

#endif DOGEENGINE_PHYSICSMANAGER_HPP



/*
void cast(int mouseX, int mouseY, int screenWidth, int screenHeight,glm::mat4 ProjectionMatrix, glm::mat4 ViewMatrix){
    glm::vec4 lRayStart_NDC(
            ((float)mouseX/(float)screenWidth  - 0.5f) * 2.0f, // [0,1024] -> [-1,1]
            ((float)mouseY/(float)screenHeight - 0.5f) * 2.0f, // [0, 768] -> [-1,1]
            -1.0, // The near plane maps to Z=-1 in Normalized Device Coordinates
            1.0f
    );
    glm::vec4 lRayEnd_NDC(
            ((float)mouseX/(float)screenWidth  - 0.5f) * 2.0f,
            ((float)mouseY/(float)screenHeight - 0.5f) * 2.0f,
            0.0,
            1.0f
    );
    glm::mat4 InverseProjectionMatrix = glm::inverse(ProjectionMatrix);

// The View Matrix goes from World Space to Camera Space.
// So inverse(ViewMatrix) goes from Camera Space to World Space.
    glm::mat4 InverseViewMatrix = glm::inverse(ViewMatrix);

    glm::vec4 lRayStart_camera = InverseProjectionMatrix * lRayStart_NDC;    lRayStart_camera/=lRayStart_camera.w;
    glm::vec4 lRayStart_world  = InverseViewMatrix       * lRayStart_camera; lRayStart_world /=lRayStart_world .w;
    glm::vec4 lRayEnd_camera   = InverseProjectionMatrix * lRayEnd_NDC;      lRayEnd_camera  /=lRayEnd_camera  .w;
    glm::vec4 lRayEnd_world    = InverseViewMatrix       * lRayEnd_camera;   lRayEnd_world   /=lRayEnd_world   .w;
    glm::vec3 lRayDir_world(lRayEnd_world - lRayStart_world);
    lRayDir_world = glm::normalize(lRayDir_world);
    glm::vec3 out_end = glm::vec3(lRayStart_world) + lRayDir_world * 100000.0f;

    btCollisionWorld::ClosestRayResultCallback RayCallback(
            btVector3(lRayStart_world.x, lRayStart_world.y, lRayStart_world.z),
            btVector3(out_end.x, out_end.y, out_end.z)
    );
    dynamicsWorld->rayTest(
            btVector3(lRayStart_world.x, lRayStart_world.y, lRayStart_world.z),
            btVector3(out_end.x, out_end.y, out_end.z),
            RayCallback
    );

    if(RayCallback.hasHit()) {
        Node* node = (Node*)RayCallback.m_collisionObject->getUserPointer();
        std::cout << "mesh " << node->m_name << "\n";

    }else{
     //   std::cout << "background\n";
    }
}*//*
void scene(SceneGraph* graph){
    graph->updateTransform();
    for(Node* child : graph->m_children){
        sceneNode(child);
    }
}
void sceneNode(Node* node){
    if(node->m_children.size() == 0){

        glm::vec3 scale;
        glm::quat rotation;
        glm::vec3 translation;
        glm::vec3 skew;
        glm::vec4 perspective;
        glm::decompose(node->m_global_transform, scale, rotation, translation, skew, perspective);
        std::cout << "addding objevy " << translation.x << " " << translation.y << " " << translation.z << " \n";
        addObject(translation, rotation, node);
    }else{
        for(Node* child : node->m_children){
            sceneNode(child);
        }
    }
}

void addObject(glm::vec3 positions, glm::quat rotation, Node* user){
    if(user->m_object != nullptr){
        btCollisionShape* boxCollisionShape = new btConvexHullShape((btScalar*)user->m_object->m_vertices.data(),user->m_object->m_vertices.size()/14, 14 * sizeof(float));
        btDefaultMotionState* motionstate = new btDefaultMotionState(btTransform(
                btQuaternion(rotation.x, rotation.y, rotation.z, rotation.w),
                btVector3(positions.x, positions.y,positions.z)
        ));

        btRigidBody::btRigidBodyConstructionInfo rigidBodyCI(
                0,                  // mass, in kg. 0 -> Static object, will never move.
                motionstate,
                boxCollisionShape,  // collision shape of body
                btVector3(0,0,0)    // local inertia
        );
        btRigidBody *rigidBody = new btRigidBody(rigidBodyCI);
        dynamicsWorld->addRigidBody(rigidBody);
        rigidbodies.push_back(rigidBody);
        rigidBody->setUserPointer(user);
    }

}*/