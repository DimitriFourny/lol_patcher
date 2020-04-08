#pragma once
#include <d3dx9.h>

struct Vector2 {
  float x;
  float y;

  Vector2(float x, float y) {
    this->x = x;
    this->y = y;
  }
  Vector2() : Vector2(0, 0) {}

  D3DXVECTOR2 ToD3DX() {
    D3DXVECTOR2 r;
    r.x = this->x;
    r.y = this->y;
    return r;
  }
};

struct Vector3 {
  float x;
  float y;
  float z;

  Vector3(float x, float y, float z) {
    this->x = x;
    this->y = y;
    this->z = z;
  }
  Vector3() : Vector3(0, 0, 0) {}

  D3DXVECTOR3 ToD3DX() {
    D3DXVECTOR3 r;
    r.x = this->x;
    r.y = this->y;
    r.z = this->z;
    return r;
  }
};