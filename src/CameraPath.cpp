#include"CameraPath.h"

#include<glm/gtx/spline.hpp>
#include<algorithm>
#include<cctype>

CameraPath::CameraPath(bool looping):_looping(looping){}

CameraPathKeypoint CameraPath::getKeypoint(float t){
  assert(this!=nullptr);
  CameraPathKeypoint ret;
  t = glm::mod(t,1.f);
  if(this->keys.size()==0)return ret;
  if(t==1.f){
    if(this->_looping)return this->keys.front();
    else              return this->keys.back();
  }
  size_t i0,i1;
  float tt;
  if(this->_looping){
    i0 = (size_t)(float(this->keys.size())*t);
    i1 = (i0+1)%this->keys.size();
    tt = float(this->keys.size())*t-float(i0);
  }else{
    i0 = (size_t)((float(this->keys.size())-1.f)*t);
    i1 = (i0+1)%this->keys.size();
    tt = float(this->keys.size()-1)*t-float(i0);
  }
  glm::vec3 pa,va,ua;
  glm::vec3 pb,vb,ub;
  if(i0==0){
    if(this->_looping){
      pa = this->keys.back().position  ;
      va = this->keys.back().viewVector;
      ua = this->keys.back().upVector  ;
    }else{
      pa = this->keys[i0].position  *2.f - this->keys[i1].position  ;
      va = this->keys[i0].viewVector*2.f - this->keys[i1].viewVector;
      ua = this->keys[i0].upVector  *2.f - this->keys[i1].upVector  ;
    }
  }else{
    pa = this->keys[i0-1].position  ;
    va = this->keys[i0-1].viewVector;
    ua = this->keys[i0-1].upVector  ;
  }
  if(i1+1==this->keys.size()){
    if(this->_looping){
      pb = this->keys.front().position  ;
      vb = this->keys.front().viewVector;
      ub = this->keys.front().upVector  ;
    }else{
      pb = this->keys[i1].position  *2.f - this->keys[i0].position  ;
      vb = this->keys[i1].viewVector*2.f - this->keys[i0].viewVector;
      ub = this->keys[i1].upVector  *2.f - this->keys[i0].upVector  ;
    }
  }else{
    pb = this->keys[i1+1].position  ;
    vb = this->keys[i1+1].viewVector;
    ub = this->keys[i1+1].upVector  ;
  }
  ret.position   = glm::catmullRom(pa,this->keys[i0].position,this->keys[i1].position,pb,tt);
  ret.viewVector = glm::normalize(glm::catmullRom(va,this->keys[i0].viewVector,this->keys[i1].viewVector,vb,tt));
  ret.upVector   = glm::normalize(glm::catmullRom(ua,this->keys[i0].upVector,this->keys[i1].upVector,ub,tt));
  ret.upVector   = glm::normalize(glm::cross(glm::cross(ret.viewVector,ret.upVector),ret.viewVector));
  return ret;
}
