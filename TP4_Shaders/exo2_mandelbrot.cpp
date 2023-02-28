#include <GL/glew.h>
#include <glimac/FilePath.hpp>
#include <glimac/Program.hpp>
#include <glimac/SDLWindowManager.hpp>
#include <iostream>

using namespace glimac;

int main(int argc, char **argv) {
  // Initialize SDL and open a window
  SDLWindowManager windowManager(800, 600, "Triangle");

  // Initialize glew for OpenGL3+ support
  GLenum glewInitError = glewInit();
  if (GLEW_OK != glewInitError) {
    std::cerr << glewGetErrorString(glewInitError) << std::endl;
    return EXIT_FAILURE;
  }

  // Load shaders
  FilePath applicationPath(argv[0]);
  Program program =
      loadProgram(applicationPath.dirPath() + "shaders" + "color2D.vs.glsl",
                  applicationPath.dirPath() + "shaders" + "color2D.fs.glsl");
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
  // GLfloat vertices[] = {-0.5f, -0.5f, 0.5f, -0.5f, 0.0f, 0.5f};
  GLfloat vertices[] = {
      -0.5f, -0.5f, 1.f, 0.f, 0.f, // premier sommet
      0.5f,  -0.5f, 0.f, 1.f, 0.f, // deuxième sommet
      0.0f,  0.5f,  0.f, 0.f, 1.f  // troisième sommet
  };
  glBufferData(GL_ARRAY_BUFFER, 15 * sizeof(GLfloat), vertices, GL_STATIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  // VAO (vertex specification)
  GLuint vao;
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);

  // Activation of vertex attributes
  const GLuint VERTEX_ATTR_POSITION = 0;
  const GLuint COLOR_ATTR_POSITION = 1;

  glEnableVertexAttribArray(VERTEX_ATTR_POSITION);
  glEnableVertexAttribArray(COLOR_ATTR_POSITION);

  // Specification of vertex attributes
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glVertexAttribPointer(VERTEX_ATTR_POSITION, 2, GL_FLOAT, GL_FALSE,
                        5 * sizeof(GLfloat),
                        (const GLvoid *)(0 * sizeof(GLfloat)));
  glVertexAttribPointer(COLOR_ATTR_POSITION, 3, GL_FLOAT, GL_FALSE,
                        5 * sizeof(GLfloat),
                        (const GLvoid *)(2 * sizeof(GLfloat)));
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
