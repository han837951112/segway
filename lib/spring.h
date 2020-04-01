//
// Created by mis on 2020/4/1.
//

#ifndef SEGWAY_SPRING_H
#define SEGWAY_SPRING_H

#include <box2d/box2d.h>
#include <iostream>

class Spring{
public:
    Spring(b2World* world):m_world(world){}
    void update(b2Vec2 target){
        if(mouseJoint != nullptr){
            mouseJoint->SetTarget(target);
        }
    }
    void bind(b2Vec2 target, b2Body* body){
        b2MouseJointDef md;
        md.bodyA = m_world->CreateBody(new b2BodyDef());
        md.bodyB = body;

        body->SetFixedRotation(true);

        md.target = target;
        md.maxForce = 1000.f * body->GetMass();
        md.frequencyHz = 5.f;
        md.dampingRatio = .9f;

        mouseJoint = dynamic_cast<b2MouseJoint *>(m_world->CreateJoint(&md));
    }

    void destroy(){
        if(mouseJoint != nullptr){
            mouseJoint->GetBodyB()->SetFixedRotation(false);
            m_world->DestroyJoint(mouseJoint);
            mouseJoint = nullptr;
        }
    }
private:
    b2MouseJoint* mouseJoint;
    b2World* m_world;
};

#endif //SEGWAY_SPRING_H
