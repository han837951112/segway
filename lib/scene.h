//
// Created by mis on 2020/3/31.
//

#ifndef SEGWAY_SCENE_H
#define SEGWAY_SCENE_H

#include <random>
#include <deque>

#include <box2d/box2d.h>

#include "draw.h"
#include "pid.h"
#include "spring.h"

// contain a deque and other information
class dequeAdapter{
public:
    dequeAdapter():sum(0),maxSize(30){}
    dequeAdapter(int maxSize):sum(0),maxSize(maxSize){}
    void push(float v){
        // make sure the length can't bigger than maxSize
        if(data.size() == maxSize){
            sum -= data.front();
            data.pop_front();
        }
        sum += v;
        data.push_back(v);
    }

    float getValue(){
        return sum / data.size();
    }

    std::deque<float> data;
private:
    int maxSize;
    float sum;
};

class Scene{
public:
    GLuint      Width,Height;
    GLboolean   Keys[1024];

    Scene(GLuint width,GLuint height);
    ~Scene();
    // Initialization
    void Init();
    void InitBox2D();

    // Render loop
    void Update(GLfloat dt);
    void Render();

    // util function
    b2Body* CreateBox(b2Vec2 pos,b2Vec2 size,bool ignoreCollision = false);
    b2Body* CreateCircle(b2Vec2 pos, float radius);
    static void Destroy();

    // handle keyboard and mouse
    void ProcessInput(GLfloat dt);
    void MouseDown(b2Vec2 pos);
    void MouseUp(b2Vec2 pos);
    void MouseMove(b2Vec2 pos);

    b2World World;
private:
    float targetX;
    // random number generator
    std::mt19937 rnd;
    // PID control
    PID clt;
    // PID data
    dequeAdapter filter;
    // speed data
    dequeAdapter debugLine;
    // mouse drag body
    Spring spring;
};

#endif //SEGWAY_SCENE_H
