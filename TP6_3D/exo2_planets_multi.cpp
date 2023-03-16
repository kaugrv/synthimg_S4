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

int main(int argc, char **argv) {
  // Initialize SDL and open a window
  SDLWindowManager windowManager(800, 600, "Planètes ???");

  // Initialize glew for OpenGL3+ support
  GLenum glewInitError = glewInit();
  if (GLEW_OK != glewInitError) {
    std::cerr << glewGetErrorString(glewInitError) << std::endl;
    return EXIT_FAILURE;
  }

  // Load shaders
  FilePath applicationPath(argv[0]);
  Program program =
      loadProgram(applicationPath.dirPath() + "shaders/3D.vs.glsl",
                  applicationPath.dirPath() + "shaders/multiTex3D.fs.glsl");
  program.use();

  // Load Textures
  auto earth_ptr = glimac::loadImage(applicationPath.dirPath() + ".." + ".." +
                                     "assets/texture/EarthMap.jpg");
  auto moon_ptr = glimac::loadImage(applicationPath.dirPath() + ".." + ".." +
                                    "assets/texture/MoonMap.jpg");
  auto cloud_ptr = glimac::loadImage(applicationPath.dirPath() + ".." + ".." +
                                     "assets/texture/CloudMap.jpg");

  // Get Matrix locations
  GLint MVP_location = glGetUniformLocation(program.getGLId(), "uMVPMatrix");
  GLint MV_location = glGetUniformLocation(program.getGLId(), "uMVMatrix");
  GLint Normal_location =
      glGetUniformLocation(program.getGLId(), "uNormalMatrix");

  GLint uPlanetTexture_location =
      glGetUniformLocation(program.getGLId(), "uPlanetTexture");
  GLint uCloudTexture_location =
      glGetUniformLocation(program.getGLId(), "uCloudTexture");

  glEnable(GL_DEPTH_TEST);

  glm::mat4 ProjMatrix =
      glm::perspective<float>(glm::radians(70.f), 800. / 600., 0.1f, 100.f);

  glm::mat4 MVMatrix = translate(0., 0., -5.);
  glm::mat4 NormalMatrix = glm::transpose(glm::inverse(MVMatrix));

  /*********************************
   * INITIALIZATION BEGIN
   *********************************/

  // VBO
  Sphere S(1, 16, 32);

  GLuint vbo;
  glGenBuffers(1, &vbo);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);

  glBufferData(GL_ARRAY_BUFFER,
               sizeof(glimac::ShapeVertex) * S.getVertexCount(),
               S.getDataPointer(), GL_STATIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  // VAO (vertex specification)
  GLuint vao;
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);

  // Activation of vertex attributes
  const GLuint VERTEX_ATTR_POSITION = 0;
  const GLuint NORMAL_ATTR_POSITION = 1;
  const GLuint TEX_ATTR_POSITION = 2;

  glEnableVertexAttribArray(VERTEX_ATTR_POSITION);
  glEnableVertexAttribArray(NORMAL_ATTR_POSITION);
  glEnableVertexAttribArray(TEX_ATTR_POSITION);

  // Specification of vertex attributes
  glBindBuffer(GL_ARRAY_BUFFER, vbo);

  glVertexAttribPointer(
      VERTEX_ATTR_POSITION, 3, GL_FLOAT, GL_FALSE, sizeof(glimac::ShapeVertex),
      (const GLvoid *)offsetof(glimac::ShapeVertex, position));
  glVertexAttribPointer(NORMAL_ATTR_POSITION, 3, GL_FLOAT, GL_FALSE,
                        sizeof(ShapeVertex),
                        (const GLvoid *)offsetof(glimac::ShapeVertex, normal));
  glVertexAttribPointer(
      TEX_ATTR_POSITION, 2, GL_FLOAT, GL_FALSE, sizeof(ShapeVertex),
      (const GLvoid *)offsetof(glimac::ShapeVertex, texCoords));

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);

  // Textures
  GLuint planet_texture;
  glGenTextures(1, &planet_texture);
  glActiveTexture(GL_TEXTURE0);

  glBindTexture(GL_TEXTURE_2D, planet_texture);
  glTexImage2D(GL_TEXTURE_2D, 0, 3, earth_ptr->getWidth(),
               earth_ptr->getHeight(), 0, GL_RGBA, GL_FLOAT,
               earth_ptr->getPixels());
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  glBindTexture(GL_TEXTURE_2D, 0);

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

  /*********************************
   * INITIALIZATION END
   *********************************/

  // Application loop:
  bool done = false;
  int nb_lunes = 32;

  std::vector<glm::vec3> AxisList;
  for (int i = 0; i < nb_lunes; i++) {
    AxisList.push_back(glm::sphericalRand(2.f));
  }

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

    glBindVertexArray(vao);

    glm::mat4 MVEarth = glm::rotate(MVMatrix, time, glm::vec3(0, 1, 0));

    glUniformMatrix4fv(MVP_location, 1, GL_FALSE,
                       glm::value_ptr(ProjMatrix * MVEarth));
    glUniformMatrix4fv(MV_location, 1, GL_FALSE, glm::value_ptr(MVEarth));
    glUniformMatrix4fv(Normal_location, 1, GL_FALSE,
                       glm::value_ptr(NormalMatrix));

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, planet_texture);

    glTexImage2D(GL_TEXTURE_2D, 0, 3, earth_ptr->getWidth(),
                 earth_ptr->getHeight(), 0, GL_RGBA, GL_FLOAT,
                 earth_ptr->getPixels());

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, cloud_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, 3, cloud_ptr->getWidth(),
                 cloud_ptr->getHeight(), 0, GL_RGBA, GL_FLOAT,
                 cloud_ptr->getPixels());

    glUniform1i(uPlanetTexture_location, 0);
    glUniform1i(uCloudTexture_location, 1);

    glDrawArrays(GL_TRIANGLES, 0, S.getVertexCount()); // Grande sphère

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, planet_texture);

    glTexImage2D(GL_TEXTURE_2D, 0, 3, moon_ptr->getWidth(),
                 moon_ptr->getHeight(), 0, GL_RGBA, GL_FLOAT,
                 moon_ptr->getPixels());

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, 0); // débind sur l'unité GL_TEXTURE1

    for (int i = 0; i < nb_lunes; i++) {
      glm::mat4 MVMoon = glm::scale(
          glm::translate(
              glm::rotate(glm::translate(glm::mat4(1), glm::vec3(0, 0, -5)),
                          time, AxisList[i]),
              BeginPosList[i]),
          glm::vec3(0.2, 0.2, 0.2));
      // Translation * Rotation * Translation * Scale

      glUniformMatrix4fv(MVP_location, 1, GL_FALSE,
                         glm::value_ptr(ProjMatrix * MVMoon));
      glUniformMatrix4fv(MV_location, 1, GL_FALSE, glm::value_ptr(MVMoon));

      glDrawArrays(GL_TRIANGLES, 0, S.getVertexCount());
    }

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0); // débind sur l'unité GL_TEXTURE0

    glBindVertexArray(0);

    /*********************************
     * RENDERING END
     *********************************/

    // Update the display
    windowManager.swapBuffers();
  }

  glDeleteTextures(1, &planet_texture);
  glDeleteBuffers(1, &vbo);
  glDeleteVertexArrays(1, &vao);

  return EXIT_SUCCESS;
}
