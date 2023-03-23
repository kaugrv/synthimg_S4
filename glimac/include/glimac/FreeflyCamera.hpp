#pragma once
#include <glimac/glm.hpp>
#include <glm/glm.hpp>
#include <iostream>

#define PI 3.149592

class FreeflyCamera {

public:
  glm::vec3 m_Position;
  float m_fPhi;
  float m_fTheta;
  glm::vec3 m_FrontVector;
  glm::vec3 m_LeftVector;
  glm::vec3 m_UpVector;

  void computeDirectionVectors() {
    m_FrontVector =
        glm::vec3(cos(glm::radians(m_fTheta)) * sin(glm::radians(m_fPhi)),
                  sin(glm::radians(m_fTheta)),
                  cos(glm::radians(m_fTheta)) * cos(glm::radians(m_fPhi)));
    m_FrontVector = glm::vec3(sin(glm::radians(m_fPhi) + PI / 2.), 0.,
                              cos(glm::radians(m_fPhi) + PI / 2.));
    m_UpVector = glm::cross(m_FrontVector, m_LeftVector);
  }
  FreeflyCamera() : m_Position(glm::vec3(0.)), m_fPhi(PI), m_fTheta(0.) {
    computeDirectionVectors();
  };

  void moveFront(float t) { m_Position += t * m_FrontVector; }
  void moveLeft(float t) { m_Position += t * m_LeftVector; }
  void rotateLeft(float degrees) { m_fPhi += glm::radians(degrees); }
  void rotateUp(float degrees) { m_fTheta += glm::radians(degrees); }

  glm::mat4 getViewMatrix() {
    glm::vec3 eye = m_Position;
    glm::vec3 point = m_Position + m_FrontVector;
    glm::vec3 up = m_UpVector;
    computeDirectionVectors();
    return glm::lookAt(eye, point, up);
  }
};
