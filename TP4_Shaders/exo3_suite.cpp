#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <cstddef>
#include <glimac/FilePath.hpp>
#include <glimac/Program.hpp>
#include <glimac/SDLWindowManager.hpp>
#include <glimac/glm.hpp>
#include <iostream>

using namespace glimac;

class Vertex2DUV {
private:
public:
  glm::vec2 m_position;
  glm::vec2 m_texture;

  Vertex2DUV();
  Vertex2DUV(glm::vec2 position, glm::vec2 texture)
      : m_position(position), m_texture(texture){};
};

glm::mat3 translate(float tx, float ty) {
  return glm::mat3(1, 0, 0, 0, 1, 0, tx, ty, 1);
}

glm::mat3 scale(float sx, float sy) {
  return glm::mat3(sx, 0, 0, 0, sy, 0, 0, 0, 1);
}

glm::mat3 rotate(float a) {
  float c = cos(glm::radians(a));
  float s = sin(glm::radians(a));

  return glm::mat3(c, s, 0, -s, c, 0, 0, 0, 1);
}

int main(int argc, char **argv) {
  // Initialize SDL and open a window
  SDLWindowManager windowManager(800, 600, "GLImac");

  // Initialize glew for OpenGL3+ support
  GLenum glewInitError = glewInit();
  if (GLEW_OK != glewInitError) {
    std::cerr << glewGetErrorString(glewInitError) << std::endl;
    return EXIT_FAILURE;
  }

  // Load shaders
  FilePath applicationPath(argv[0]);
  Program program =
      loadProgram(applicationPath.dirPath() + "shaders/tex2D.vs.glsl",
                  applicationPath.dirPath() + "shaders/tex2D.fs.glsl");
  program.use();

  GLint uTime_location = glGetUniformLocation(program.getGLId(), "uTime");
  GLint uModelMatrix_location =
      glGetUniformLocation(program.getGLId(), "uModelMatrix");

  std::cout << "OpenGL Version : " << glGetString(GL_VERSION) << std::endl;
  std::cout << "GLEW Version : " << glewGetString(GLEW_VERSION) << std::endl;
  std::cout << "Application Path : " << applicationPath.dirPath() << std::endl;

  /*********************************
   * INITIALIZATION BEGIN
   *********************************/

  // VBO
  GLuint vbo;
  glGenBuffers(1, &vbo);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);

  Vertex2DUV vertices[] = {Vertex2DUV(glm::vec2(-1, -1), glm::vec2(0, 0)),
                           Vertex2DUV(glm::vec2(1, -1), glm::vec2(0, 0)),
                           Vertex2DUV(glm::vec2(0, 1), glm::vec2(0, 0))};

  glBufferData(GL_ARRAY_BUFFER, 3 * sizeof(Vertex2DUV), vertices,
               GL_STATIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  // VAO (vertex specification)
  GLuint vao;
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);

  // Activation of vertex attributes
  const GLuint VERTEX_ATTR_POSITION = 0;
  const GLuint UV_ATTR_POSITION = 1;

  glEnableVertexAttribArray(VERTEX_ATTR_POSITION);
  glEnableVertexAttribArray(UV_ATTR_POSITION);

  // Specification of vertex attributes
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glVertexAttribPointer(VERTEX_ATTR_POSITION, 2, GL_FLOAT, GL_FALSE,
                        sizeof(Vertex2DUV),
                        (const GLvoid *)offsetof(Vertex2DUV, m_position));
  glVertexAttribPointer(UV_ATTR_POSITION, 2, GL_FLOAT, GL_FALSE,
                        sizeof(Vertex2DUV),
                        (const GLvoid *)offsetof(Vertex2DUV, m_texture));
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  glBindVertexArray(0);

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

    glUniform1f(uTime_location, t);

    glm::mat3 R = rotate(t);

    glUniformMatrix3fv(uModelMatrix_location, 1, GL_FALSE, glm::value_ptr(R));

    /*********************************
     * RENDERING BEGIN
     *********************************/
    glBindVertexArray(vao);
    glClear(GL_COLOR_BUFFER_BIT);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glBindVertexArray(0);

    /*********************************
     * RENDERING END
     *********************************/

    // Update the display
    windowManager.swapBuffers();
  }

  glDeleteBuffers(1, &vbo);
  glDeleteVertexArrays(1, &vao);

  return EXIT_SUCCESS;
}
