//
// Created by mis on 2020/3/31.
//

#include <ctime>
#include <iostream>
#include "../lib/scene.h"
#include "../lib/pendulum.h"

DebugDraw      *DebugerDrawer;
Camera         *Camera;
Pendulum       *pendulum;


Scene::Scene(GLuint width,GLuint height)
    :  Keys(), Width(width), Height(height), World(b2Vec2(0,-.5f))
    ,rnd(time(nullptr)),clt(18.5f,10.0f,10.0f),spring(&World),debugLine(Width)
{
    targetX = width/2.f;
}

Scene::~Scene() {
    delete pendulum;
}

void Scene::Destroy(){
    DebugerDrawer->Destroy();
}

void Scene::Init() {
    DebugerDrawer =&g_debugDraw;
    Camera = &g_camera;
    Camera->m_width = Width;
    Camera->m_height = Height;

    DebugerDrawer->Create();

    // do openGL config

    InitBox2D();
}

void Scene::InitBox2D() {
    DebugerDrawer->AppendFlags(DebugDraw::e_shapeBit);
    World.SetDebugDraw(DebugerDrawer);

    float top, button, left, right;
    b2Vec2 pos = Camera->ConvertScreenToWorld(b2Vec2(this->Width, this->Height));
    top = pos.y * -1;
    button = pos.y;
    left = -pos.x;
    right =pos.x;
    pendulum = new Pendulum(this,Camera,&World,b2Vec2(pos.x/2,pos.y/2));

    // define wall
    {
        b2BodyDef bd;
        b2FixtureDef fd;

        fd.density = 1.f;
        fd.friction = .5f;
        fd.restitution = .2f;
        b2Body *ground = World.CreateBody(&bd);

        b2EdgeShape shape;
        // Floor
        shape.Set(b2Vec2(left, button), b2Vec2(right,button));
        fd.shape = &shape;
        ground->CreateFixture(&fd);
        // Roof
        shape.Set(b2Vec2(left, top), b2Vec2(right, top));
        fd.shape = &shape;
        ground->CreateFixture(&fd);

        ground = World.CreateBody(&bd);
        // Left wall
        shape.Set(b2Vec2(left, button), b2Vec2(left, top));
        fd.shape = &shape;
        ground->CreateFixture(&fd);

        // Right wall
        shape.Set(b2Vec2(right, button), b2Vec2(right,top));
        fd.shape = &shape;
        ground->CreateFixture(&fd);

    }

    CreateBox(b2Vec2(left/2,top/2),b2Vec2(1,1));
    CreateBox(b2Vec2(left/2,top/3),b2Vec2(3,3));

    // heap in the left of screen
    {
        b2BodyDef bd;
        b2FixtureDef fd;

        fd.density = 1.f;
        fd.friction = .8f;
        fd.restitution = .2f;

        b2Body *bound = World.CreateBody(&bd);

        b2PolygonShape shape;
        b2Vec2 positions[] = {{left/2.f,button},{left/1.2f,button/1.2f},{left,button/1.2f},{left,button}};
        shape.Set(positions,4);
        fd.shape = &shape;
        bound->CreateFixture(&fd);
    }

}
void drawLine(float pos, b2Color color);
float timeStep = 1.f / 60.f;
int velocityIterations = 6;
int positionIterations = 2;
void Scene::Update(GLfloat dt) {
    World.Step(timeStep, velocityIterations, positionIterations);

    // draw target position
    drawLine(targetX,b2Color(1.f,.5f,.5f));

    auto pos = pendulum->getPosition();
    auto velocity = pendulum->getVelocity();

    filter.push(velocity.x);
    auto smoothVelocity = filter.getValue();

    auto predictPos = pos.x + 25 * smoothVelocity;
    if((predictPos - targetX) * (pos.x - targetX) < 0){
        predictPos = pos.x + 100 * smoothVelocity;
    }

    drawLine(predictPos,b2Color(.5f,1.f,.5f));

    float targetAngle = b2Clamp((predictPos-targetX)/Width*PI/5,-.25f,.25f);
    float nowAngle = pendulum->getAngleRadians();
    if(abs(10 * smoothVelocity) > 60 &&
            (predictPos - targetX) * (pos.x - targetX) >= 0 &&
            (predictPos - targetX) * smoothVelocity < 0){
        targetAngle = 0;
    }

    clt.setError(targetAngle - nowAngle);
    float speed = clt.step(dt);
    pendulum->setMotorSpeed(  100*speed);
    debugLine.push(23 * smoothVelocity);
}

void Scene::ProcessInput(GLfloat dt) {
    // do nothing when key pressed
}

class QueryCallback : public b2QueryCallback
{
public:
    QueryCallback(const b2Vec2& point)
    {
        m_point = point;
        m_fixture = nullptr;
    }

    bool ReportFixture(b2Fixture* fixture) override
    {
        b2Body* body = fixture->GetBody();
        if (body->GetType() == b2_dynamicBody)
        {
            bool inside = fixture->TestPoint(m_point);
            if (inside)
            {
                m_fixture = fixture;

                // We are done, terminate the query.
                return false;
            }
        }

        // Continue the query.
        return true;
    }

    b2Vec2 m_point;
    b2Fixture* m_fixture;
};
void Scene::MouseDown(b2Vec2 pos) {
    float px = pos.x;
    pos = Camera->ConvertScreenToWorld(pos);

    b2AABB aabb;
    b2Vec2 d;
    d.Set(0.001f,0.001f);
    aabb.lowerBound = pos - d;
    aabb.upperBound = pos + d;

    QueryCallback callback(pos);
    World.QueryAABB(&callback,aabb);

    if(callback.m_fixture){
        b2Body* body = callback.m_fixture->GetBody();
        spring.bind(pos,body);
        body->SetActive(true);
    } else{
        targetX = px;
    }
}

void Scene::MouseUp(b2Vec2 pos) {
    spring.destroy();
}

void Scene::MouseMove(b2Vec2 pos) {
    spring.update( Camera->ConvertScreenToWorld(pos));
}

void Scene::Render() {

    World.DrawDebugData();

    // render the debug line
    float yp = Height / 4.f;
    b2Color brush(1.f,1.f,1.f);
    DebugerDrawer->DrawSegment(Camera->ConvertScreenToWorld(b2Vec2(0,yp)),
                Camera->ConvertScreenToWorld(b2Vec2(Width,yp)),brush);
    std::deque<float>& data = debugLine.data;
    brush.Set(1.f,.0f,.1f);
    for(unsigned int i=0;i<data.size();++i){
        DebugerDrawer->DrawPoint(Camera->ConvertScreenToWorld(b2Vec2(i,yp+data[i])),.1f,brush);
    }

    DebugerDrawer->Flush();
}


//// draw a line from bottom to middle by x in screen
void drawLine(float posX, b2Color color){
    b2Vec2 p1(posX,SCR_WIDTH);
    b2Vec2 p2(posX,SCR_WIDTH/3.f);
    p1 = Camera->ConvertScreenToWorld(p1);
    p2 = Camera->ConvertScreenToWorld(p2);
    DebugerDrawer->DrawSegment(p1,p2,color);
}

//// create a box in pos(world)
b2Body* Scene::CreateBox(b2Vec2 pos,b2Vec2 size,bool ignoreCollision){
    b2BodyDef bd ;
    bd.type = b2_dynamicBody;
    bd.position = pos;

    b2FixtureDef fd;
    b2PolygonShape shape;
    shape.SetAsBox(size.x,size.y);

    fd.shape = &shape;
    fd.density = 1.f;
    fd.friction = .5f;

    if(ignoreCollision){
        fd.filter.maskBits = 0;
        bd.linearDamping = .1f;
    }
    b2Body * body = World.CreateBody(&bd);
    body->CreateFixture(&fd);
    return body;
}

//// create a circle in pos(world)
b2Body* Scene::CreateCircle(b2Vec2 pos, float radius){
    b2BodyDef bd ;
    bd.type = b2_dynamicBody;
    bd.position = pos;

    b2FixtureDef fd;
    b2CircleShape shape;
    shape.m_radius = radius;
    fd.shape = &shape;

    fd.density = 1.f;
    fd.friction = .5f;
    fd.restitution = .2f;

    b2Body* body = World.CreateBody(&bd);
    body->CreateFixture(&fd);
    return body;
}