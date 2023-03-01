#include <GL/glew.h>
#include <cstddef>
#include <glimac/FilePath.hpp>
#include <glimac/Program.hpp>
#include <glimac/SDLWindowManager.hpp>
#include <glimac/glm.hpp>
#include <iostream>

using namespace glimac;

class Vertex2DColor {
private:
public:
  glm::vec2 m_position;

  Vertex2DColor();
  Vertex2DColor(glm::vec2 position) : m_position(position){};
};

int main(int argc, char **argv) {
  // Initialize SDL and open a window
  SDLWindowManager windowManager(800, 600, "Mandelbrotu");

  // Initialize glew for OpenGL3+ support
  GLenum glewInitError = glewInit();
  if (GLEW_OK != glewInitError) {
    std::cerr << glewGetErrorString(glewInitError) << std::endl;
    return EXIT_FAILURE;
  }

  // Load shaders
  FilePath applicationPath(argv[0]);
  Program program =
      loadProgram(applicationPath.dirPath() + "shaders/mandelbrot.vs.glsl",
                  applicationPath.dirPath() + "shaders/mandelbrot.fs.glsl");
  program.use();

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

  // Using my class
  Vertex2DColor vertices[] = {
      Vertex2DColor(glm::vec2(-1, 1)),  Vertex2DColor(glm::vec2(1, 1)),
      Vertex2DColor(glm::vec2(1, -1)),  Vertex2DColor(glm::vec2(-1, 1)),
      Vertex2DColor(glm::vec2(-1, -1)), Vertex2DColor(glm::vec2(1, -1))};

  glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(Vertex2DColor), vertices,
               GL_STATIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  // VAO (vertex specification)
  GLuint vao;
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);

  // Activation of vertex attributes
  const GLuint VERTEX_ATTR_POSITION = 0;

  glEnableVertexAttribArray(VERTEX_ATTR_POSITION);

  // Specification of vertex attributes
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glVertexAttribPointer(VERTEX_ATTR_POSITION, 2, GL_FLOAT, GL_FALSE,
                        sizeof(Vertex2DColor),
                        (const GLvoid *)offsetof(Vertex2DColor, m_position));

  glBindBuffer(GL_ARRAY_BUFFER, 0);

  glBindVertexArray(0);

  /*********************************
   * INITIALIZATION END
   *********************************/

  // Application loop:
  bool done = false;
  while (!done) {
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
    glBindVertexArray(vao);
    glClear(GL_COLOR_BUFFER_BIT);
    glDrawArrays(GL_TRIANGLES, 0, 6);
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
