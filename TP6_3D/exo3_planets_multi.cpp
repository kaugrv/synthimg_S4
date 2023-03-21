#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <cstddef>
#include <glimac/FilePath.hpp>
#include <glimac/Image.hpp>
#include <glimac/Program.hpp>
#include <glimac/SDLWindowManager.hpp>
#include <glimac/Sphere.hpp>
#include <glimac/glm.hpp>
#include <iostream>

using namespace glimac;

glm::mat4 translate(float tx, float ty, float tz) {
  return glm::mat4(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, tx, ty, tz, 1);
}

struct EarthProgram {
  Program m_Program;

  GLint MVP_location;
  GLint MV_location;
  GLint Normal_location;
  GLint uEarthTexture_location;
  GLint uCloudTexture_location;

  EarthProgram(const FilePath &applicationPath)
      : m_Program(loadProgram(applicationPath.dirPath() + "shaders/3D.vs.glsl",
                              applicationPath.dirPath() +
                                  "shaders/multiTex3D.fs.glsl")) {

    // Get Matrix locations
    MVP_location = glGetUniformLocation(m_Program.getGLId(), "uMVPMatrix");
    MV_location = glGetUniformLocation(m_Program.getGLId(), "uMVMatrix");
    Normal_location =
        glGetUniformLocation(m_Program.getGLId(), "uNormalMatrix");

    uEarthTexture_location =
        glGetUniformLocation(m_Program.getGLId(), "uPlanetTexture");
    uCloudTexture_location =
        glGetUniformLocation(m_Program.getGLId(), "uCloudTexture");
  }
};

struct MoonProgram {
  Program m_Program;

  GLint MVP_location;
  GLint MV_location;
  GLint Normal_location;
  GLint uTexture_location;

  MoonProgram(const FilePath &applicationPath)
      : m_Program(
            loadProgram(applicationPath.dirPath() + "shaders/3D.vs.glsl",
                        applicationPath.dirPath() + "shaders/tex3D.fs.glsl")) {

    // Get Matrix locations
    MVP_location = glGetUniformLocation(m_Program.getGLId(), "uMVPMatrix");
    MV_location = glGetUniformLocation(m_Program.getGLId(), "uMVMatrix");
    Normal_location =
        glGetUniformLocation(m_Program.getGLId(), "uNormalMatrix");

    uTexture_location =
        glGetUniformLocation(m_Program.getGLId(), "uPlanetTexture");
  }
};

int main(int argc, char **argv) {

  // Initialize SDL and open a window
  SDLWindowManager windowManager(800, 600, "Planètes ???");

  // Initialize glew for OpenGL3+ support
  GLenum glewInitError = glewInit();
  if (GLEW_OK != glewInitError) {
    std::cerr << glewGetErrorString(glewInitError) << std::endl;
    return EXIT_FAILURE;
  }

  // Initialize programs
  FilePath applicationPath(argv[0]);
  EarthProgram earthProgram(applicationPath);
  MoonProgram moonProgram(applicationPath);

  // Load Textures
  auto earth_ptr = glimac::loadImage(applicationPath.dirPath() + ".." + ".." +
                                     "assets/texture/EarthMap.jpg");
  auto moon_ptr = glimac::loadImage(applicationPath.dirPath() + ".." + ".." +
                                    "assets/texture/MoonMap.jpg");
  auto cloud_ptr = glimac::loadImage(applicationPath.dirPath() + ".." + ".." +
                                     "assets/texture/CloudMap.jpg");

  glEnable(GL_DEPTH_TEST);

  // Initialize Matrix
  glm::mat4 ProjMatrix =
      glm::perspective<float>(glm::radians(70.f), 800. / 600., 0.1f, 100.f);
  glm::mat4 MVMatrix = translate(0., 0., -5.);
  glm::mat4 NormalMatrix = glm::transpose(glm::inverse(MVMatrix));

  /*********************************
   * INITIALIZATION BEGIN
   *********************************/

  Sphere S(1, 16, 32);

  //////////// VBO
  GLuint vbo;
  glGenBuffers(1, &vbo);
  // Bind VBO
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER,
               sizeof(glimac::ShapeVertex) * S.getVertexCount(),
               S.getDataPointer(), GL_STATIC_DRAW);
  // Debind VBO
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  ////////////

  ////////////  VAO (vertex specification)
  GLuint vao;
  glGenVertexArrays(1, &vao);
  // Bind VAO
  glBindVertexArray(vao);
  ////////////

  // Activation of vertex attributes
  const GLuint VERTEX_ATTR_POSITION = 0;
  const GLuint NORMAL_ATTR_POSITION = 1;
  const GLuint TEX_ATTR_POSITION = 2;

  glEnableVertexAttribArray(VERTEX_ATTR_POSITION);
  glEnableVertexAttribArray(NORMAL_ATTR_POSITION);
  glEnableVertexAttribArray(TEX_ATTR_POSITION);

  // Specification of vertex attributes
  // Bind VBO
  glBindBuffer(GL_ARRAY_BUFFER, vbo);

  // Vertex Attrib Pointers
  glVertexAttribPointer(
      VERTEX_ATTR_POSITION, 3, GL_FLOAT, GL_FALSE, sizeof(glimac::ShapeVertex),
      (const GLvoid *)offsetof(glimac::ShapeVertex, position));
  glVertexAttribPointer(NORMAL_ATTR_POSITION, 3, GL_FLOAT, GL_FALSE,
                        sizeof(ShapeVertex),
                        (const GLvoid *)offsetof(glimac::ShapeVertex, normal));
  glVertexAttribPointer(
      TEX_ATTR_POSITION, 2, GL_FLOAT, GL_FALSE, sizeof(ShapeVertex),
      (const GLvoid *)offsetof(glimac::ShapeVertex, texCoords));

  // Debind VBO
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  // Debind VAO
  glBindVertexArray(0);

  // Textures

  // Earth
  GLuint planet_texture;
  glGenTextures(1, &planet_texture);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, planet_texture);
  glTexImage2D(GL_TEXTURE_2D, 0, 3, earth_ptr->getWidth(),
               earth_ptr->getHeight(), 0, GL_RGBA, GL_FLOAT,
               earth_ptr->getPixels());
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glBindTexture(GL_TEXTURE_2D, 0); // débind sur l'unité GL_TEXTURE0

  GLuint cloud_texture;
  glGenTextures(1, &cloud_texture);
  glActiveTexture(GL_TEXTURE1);

  glBindTexture(GL_TEXTURE_2D, cloud_texture);
  glTexImage2D(GL_TEXTURE_2D, 0, 3, cloud_ptr->getWidth(),
               cloud_ptr->getHeight(), 0, GL_RGBA, GL_FLOAT,
               cloud_ptr->getPixels());
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glBindTexture(GL_TEXTURE_2D, 0); // débind sur l'unité GL_TEXTURE1

  // Moon
  GLuint moon_texture;
  glGenTextures(1, &moon_texture);
  glActiveTexture(GL_TEXTURE0);

  glBindTexture(GL_TEXTURE_2D, moon_texture);

  glTexImage2D(GL_TEXTURE_2D, 0, 3, moon_ptr->getWidth(), moon_ptr->getHeight(),
               0, GL_RGBA, GL_FLOAT, moon_ptr->getPixels());
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glBindTexture(GL_TEXTURE_2D, 0); // débind sur l'unité GL_TEXTURE0

  /*********************************
   * INITIALIZATION END
   *********************************/

  // Application loop:
  bool done = false;
  int nb_lunes = 20;

  // List of rotation axis of moons
  std::vector<glm::vec3> AxisList;
  for (int i = 0; i < nb_lunes; i++) {
    AxisList.push_back(glm::sphericalRand(2.f));
  }

  // List of begin positions of moons
  std::vector<glm::vec3> BeginPosList;
  for (int i = 0; i < nb_lunes; i++) {
    BeginPosList.push_back(glm::sphericalRand(1.5f));
  }

  while (!done) {

    float time = windowManager.getTime();

    // Event loop:
    SDL_Event e;
    while (windowManager.pollEvent(e)) {
      if (e.type == SDL_QUIT) {
        done = true; // Leave the loop after this iteration
      }
    }

    /*********************************
     * RENDERING BEGIN
     *********************************/

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Bind VAO
    glBindVertexArray(vao);

    /// EARTH ///

    earthProgram.m_Program.use();
    glm::mat4 MVEarth = glm::rotate(MVMatrix, time, glm::vec3(0, 1, 0));

    glUniformMatrix4fv(earthProgram.MVP_location, 1, GL_FALSE,
                       glm::value_ptr(ProjMatrix * MVEarth));
    glUniformMatrix4fv(earthProgram.MV_location, 1, GL_FALSE,
                       glm::value_ptr(MVEarth));
    glUniformMatrix4fv(earthProgram.Normal_location, 1, GL_FALSE,
                       glm::value_ptr(NormalMatrix));

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, planet_texture);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, cloud_texture);
    glUniform1i(earthProgram.uEarthTexture_location, 0);
    glUniform1i(earthProgram.uCloudTexture_location, 1);

    glDrawArrays(GL_TRIANGLES, 0, S.getVertexCount()); // EARTH DRAW

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, 0);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, moon_texture);
    glUniform1i(moonProgram.uTexture_location, 0);

    moonProgram.m_Program.use();
    for (int i = 0; i < nb_lunes; i++) {
      glm::mat4 MVMoon = glm::scale(
          glm::translate(
              glm::rotate(glm::translate(glm::mat4(1), glm::vec3(0, 0, -5)),
                          time, AxisList[i]),
              BeginPosList[i]),
          glm::vec3(0.2, 0.2, 0.2));
      // Translation * Rotation * Translation * Scale

      glUniformMatrix4fv(moonProgram.MVP_location, 1, GL_FALSE,
                         glm::value_ptr(ProjMatrix * MVMoon));
      glUniformMatrix4fv(moonProgram.MV_location, 1, GL_FALSE,
                         glm::value_ptr(MVMoon));

      glDrawArrays(GL_TRIANGLES, 0, S.getVertexCount());
    }

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);

    glBindVertexArray(0);

    /*********************************
     * RENDERING END
     *********************************/

    // Update the display
    windowManager.swapBuffers();
  }

  // Delete textures
  glDeleteTextures(1, &planet_texture);

  // Debind vbo
  glDeleteBuffers(1, &vbo);

  // Debind vao
  glDeleteVertexArrays(1, &vao);

  return EXIT_SUCCESS;
}
