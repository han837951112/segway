//
// Created by mis on 2020/3/31.
//

#ifndef SEGWAY_PENDULUM_H
#define SEGWAY_PENDULUM_H

#include <box2d/box2d.h>
#include "draw.h"
#include "scene.h"
#include "global.h"

class Pendulum{
public:
    Pendulum(Scene* scene,Camera* camera,b2World* world,b2Vec2 pos):m_camera(camera),m_scene(scene),m_world(world){
        Len = 5.6f;

        Cart = m_scene->CreateBox(pos,b2Vec2(2.1f,1.2f));
        Wheel = m_scene->CreateCircle(pos,2.4f);
        Stick = m_scene->CreateBox(pos,b2Vec2(.6f,this->Len),true);
        Handle = m_scene->CreateBox(pos,b2Vec2(2.5f,1.2f));

        b2WeldJointDef wjd1;

        wjd1.bodyA = Cart;
        wjd1.bodyB = Stick;
        wjd1.localAnchorB = (b2Vec2(0,-this->Len));
        m_world->CreateJoint(&wjd1);

        b2WeldJointDef wjd2;
        wjd2.bodyA = Handle;
        wjd2.bodyB = Stick;
        wjd2.localAnchorB = (b2Vec2(0,this->Len));
        m_world->CreateJoint(&wjd2);

        b2WheelJointDef whjd;
        whjd.bodyA = Wheel;
        whjd.bodyB = Cart;
        whjd.frequencyHz = 30.f;
        whjd.dampingRatio = 1.0f;
        whjd.maxMotorTorque = 1000;

        Motor = dynamic_cast<b2WheelJoint *>(m_world->CreateJoint(&whjd));
        Motor->EnableMotor(true);
        Motor->SetMotorSpeed(0);
    }

    b2Vec2 getPosition(){
        return m_camera->ConvertWorldToScreen( Wheel->GetPosition());
    }

    b2Vec2 getVelocity(){
        return Cart->GetLinearVelocity();
    }

    float getAngleRadians(){
        float radians = Stick->GetAngle();
        while(radians > PI ) radians -= 2*PI;
        while(radians < -PI )   radians += 2*PI;
        return radians;
    }

    void setMotorSpeed(float speed){
        Motor->SetMotorSpeed(speed);
    }
private:
    float Len;
    b2Body* Cart;
    b2Body* Wheel;
    b2Body* Stick;
    b2Body* Handle;
    b2WheelJoint* Motor;

    Scene* m_scene;
    Camera* m_camera;
    b2World* m_world;
};

#endif //SEGWAY_PENDULUM_H
