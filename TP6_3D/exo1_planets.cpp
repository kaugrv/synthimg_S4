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
                  applicationPath.dirPath() + "shaders/normals.fs.glsl");
  program.use();

  // // Load Textures
  // auto triforce_ptr = glimac::loadImage(applicationPath.dirPath() + ".." +
  //                                       ".." +
  //                                       "assets/texture/triforce.png");

  // std::cout << (triforce_ptr == NULL) << std::endl;

  GLint MVP_location = glGetUniformLocation(program.getGLId(), "uMVPMatrix");
  GLint MV_location = glGetUniformLocation(program.getGLId(), "uMVMatrix");
  GLint Normal_location =
      glGetUniformLocation(program.getGLId(), "uNormalMatrix");

  glm::mat4 ProjMatrix =
      glm::perspective<float>(glm::radians(70.f), 800 / 600., 0.1f, 100.f);
  glm::mat4 MVMatrix = glm::translate(MVMatrix, glm::vec3(0, 0, -5));
  glm::mat4 NormalMatrix = glm::transpose(glm::inverse(MVMatrix));

  /*********************************
   * INITIALIZATION BEGIN
   *********************************/

  glEnable(GL_DEPTH_TEST);

  // VBO
  Sphere S(1, 32, 16);

  GLuint vbo;
  glGenBuffers(1, &vbo);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);

  glBufferData(GL_ARRAY_BUFFER, sizeof(S) * S.getVertexCount(),
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

  glVertexAttribPointer(VERTEX_ATTR_POSITION, 3, GL_FLOAT, GL_FALSE,
                        sizeof(ShapeVertex),
                        (const GLvoid *)offsetof(ShapeVertex, position));
  glVertexAttribPointer(NORMAL_ATTR_POSITION, 3, GL_FLOAT, GL_FALSE,
                        sizeof(ShapeVertex),
                        (const GLvoid *)offsetof(ShapeVertex, normal));
  glVertexAttribPointer(TEX_ATTR_POSITION, 2, GL_FLOAT, GL_FALSE,
                        sizeof(ShapeVertex),
                        (const GLvoid *)offsetof(ShapeVertex, texCoords));

  glBindBuffer(GL_ARRAY_BUFFER, 0);

  glBindVertexArray(0);

  // // Textures
  // GLuint texture;
  // glGenTextures(1, &texture);
  // glBindTexture(GL_TEXTURE_2D, texture);
  // glTexImage2D(GL_TEXTURE_2D, 0, 3, triforce_ptr->getWidth(),
  //              triforce_ptr->getHeight(), 0, GL_RGBA, GL_FLOAT,
  //              triforce_ptr->getPixels());

  // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  // glBindTexture(GL_TEXTURE_2D, 0);

  /*********************************
   * INITIALIZATION END
   *********************************/

  // Application loop:
  bool done = false;
  float t = 0.;

  while (!done) {
    t += 1.;
    // Event loop:
    SDL_Event e;
    while (windowManager.pollEvent(e)) {
      if (e.type == SDL_QUIT) {
        done = true; // Leave the loop after this iteration
      }
    }

    // glUniform1f(uTime_location, t);

    // glm::mat3 uModelMatrix;
    // glUniformMatrix3fv(uModelMatrix_location, 1, GL_FALSE,
    //                    glm::value_ptr(uModelMatrix));

    // glm::vec3 uColor;
    // glUniform3f(uColor_location, uColor.x, uColor.y, uColor.z);

    /*********************************
     * RENDERING BEGIN
     *********************************/

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUniformMatrix4fv(MVP_location, 1, GL_FALSE, glm::value_ptr(ProjMatrix));
    glUniformMatrix4fv(MV_location, 1, GL_FALSE, glm::value_ptr(MVMatrix));
    glUniformMatrix4fv(Normal_location, 1, GL_FALSE,
                       glm::value_ptr(NormalMatrix));

    glBindVertexArray(vao);

    // uColor = glm::vec3(1, 0, 1);
    // uModelMatrix = rotate(t);
    // glUniformMatrix3fv(uModelMatrix_location, 1, GL_FALSE,
    //                    glm::value_ptr(uModelMatrix));
    // glUniform3f(uColor_location, uColor.x, uColor.y, uColor.z);
    // glBindTexture(GL_TEXTURE_2D, texture);
    // glUniform1i(uTexture_location, 0);

    glDrawArrays(GL_TRIANGLES, 0, S.getVertexCount());

    glBindVertexArray(0);
    // glBindTexture(GL_TEXTURE_2D, 0);

    /*********************************
     * RENDERING END
     *********************************/

    // Update the display
    windowManager.swapBuffers();
  }

  // glDeleteTextures(1, &texture);
  glDeleteBuffers(1, &vbo);
  glDeleteVertexArrays(1, &vao);

  return EXIT_SUCCESS;
}
