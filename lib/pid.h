//
// Created by mis on 2020/3/31.
//

#ifndef SEGWAY_PID_H
#define SEGWAY_PID_H

class PID{
public:
    PID(float p, float i, float d):p(p),i(i),d(d)
    {}
    void setError(float e){
        this->cError = e;
    }
    float step(float dt){
        this->integral = this->integral + this->cError * dt;
        float derivative = (this->cError - this->pError) / dt;
        this->pError = this->cError;
        return this->p * this->cError + this->i * this->integral + this->d * derivative;
    }
private:
    float p,i,d;
    float cError,pError;
    float integral;
};

#endif //SEGWAY_PID_H
