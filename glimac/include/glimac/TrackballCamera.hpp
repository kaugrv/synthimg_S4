#pragma once
#include <glm/glm.hpp>
#include <iostream>

class TrackballCamera {
private:
  float m_fDistance; // la distance par rapport au centre de la scène
  float m_fAngleX; // l'angle effectuée par la caméra autour de l'axe x de la
                   // scène (rotation vers le haut ou vers le bas)
  float m_fAngleY; // l'angle effectuée par la caméra autour de l'axe y de la
                   // scène (rotation vers la gauche ou vers la droite)
public:
  TrackballCamera() : m_fDistance(5.f), m_fAngleX(0.f), m_fAngleY(0.f){};

  void moveFront(float delta) {
    std::cout << delta << std::endl;

    if (m_fDistance >= 0) {
      m_fDistance = delta;
    }
  }
  void rotateLeft(float degrees) {
    std::cout << degrees << std::endl;
    m_fAngleY = degrees;
  };
  void rotateUp(float degrees) {
    std::cout << degrees << std::endl;
    m_fAngleX = degrees;
  };

  glm::mat4 getViewMatrix() {
    glm::mat4 T =
        glm::mat4(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, -m_fDistance, 1);
    float cX = cos(glm::radians(m_fAngleX));
    float sX = sin(glm::radians(m_fAngleX));

    glm::mat4 rotX =
        glm::mat4(1, 0, 0, 0, 0, cX, sX, 0, 0, -sX, cX, 0, 0, 0, 0, 1);

    float cY = cos(glm::radians(m_fAngleY));
    float sY = sin(glm::radians(m_fAngleY));

    glm::mat4 rotY =
        glm::mat4(cY, 0, -sY, 0, 0, 1, 0, 0, sY, 0, cY, 0, 0, 0, 0, 1);

    return T * rotY * rotX;
  }
};
