#include <GL/glew.h>
#include <cstddef>
#include <glimac/FilePath.hpp>
#include <glimac/Program.hpp>
#include <glimac/SDLWindowManager.hpp>
#include <glimac/glm.hpp>
#include <iostream>
#include <vector>

using namespace glimac;

class Vertex2DColor {
private:
public:
  glm::vec2 m_position;
  glm::vec3 m_color;

  Vertex2DColor();
  Vertex2DColor(glm::vec2 position, glm::vec3 color)
      : m_position(position), m_color(color){};
};

int main(int argc, char **argv) {
  // Initialize SDL and open a window
  SDLWindowManager windowManager(800, 600, "GLImac");

  // Initialize glew for OpenGL3+ support
  GLenum glewInitError = glewInit();
  if (GLEW_OK != glewInitError) {
    std::cerr << glewGetErrorString(glewInitError) << std::endl;
    return EXIT_FAILURE;
  }

  std::cout << "OpenGL Version : " << glGetString(GL_VERSION) << std::endl;
  std::cout << "GLEW Version : " << glewGetString(GLEW_VERSION) << std::endl;

  FilePath applicationPath(argv[0]);
  Program program =
      loadProgram(applicationPath.dirPath() + "shaders/triangle.vs.glsl",
                  applicationPath.dirPath() + "shaders/triangle.fs.glsl");
  program.use();

  GLuint vbo;
  glGenBuffers(1, &vbo);

  glBindBuffer(GL_ARRAY_BUFFER, vbo);

  // => Tableau de sommets : un seul exemplaire de chaque sommet
  Vertex2DColor vertices[] = {
      Vertex2DColor(glm::vec2(-0.5, -0.5), glm::vec3(1, 0, 0)), // Sommet 0
      Vertex2DColor(glm::vec2(0.5, -0.5), glm::vec3(0, 1, 0)),  // Sommet 1
      Vertex2DColor(glm::vec2(0.5, 0.5), glm::vec3(0, 0, 1)),   // Sommet 2
      Vertex2DColor(glm::vec2(-0.5, 0.5), glm::vec3(1, 1, 1))   // Sommet 3
  };
  // => Penser à bien changer le nombre de sommet (4 au lieu de 6):
  glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(Vertex2DColor), vertices,
               GL_STATIC_DRAW);

  glBindBuffer(GL_ARRAY_BUFFER, 0);

  // => Creation du IBO
  GLuint ibo;
  glGenBuffers(1, &ibo);

  // => On bind sur GL_ELEMENT_ARRAY_BUFFER, cible reservée pour les IBOs
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);

  // => Tableau d'indices: ce sont les indices des sommets à dessiner
  // On en a 6 afin de former deux triangles
  // Chaque indice correspond au sommet correspondant dans le VBO
  uint32_t indices[] = {0, 1, 2, 0, 2, 3};

  // => On remplit l'IBO avec les indices:
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(uint32_t), indices,
               GL_STATIC_DRAW);

  // => Comme d'habitude on debind avant de passer à autre chose
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

  GLuint vao;
  glGenVertexArrays(1, &vao);

  glBindVertexArray(vao);

  // => On bind l'IBO sur GL_ELEMENT_ARRAY_BUFFER; puisqu'un VAO est
  // actuellement bindé, cela a pour effet d'enregistrer l'IBO dans le VAO
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);

  const GLuint VERTEX_ATTR_POSITION = 3;
  const GLuint VERTEX_ATTR_COLOR = 8;
  glEnableVertexAttribArray(VERTEX_ATTR_POSITION);
  glEnableVertexAttribArray(VERTEX_ATTR_COLOR);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glVertexAttribPointer(VERTEX_ATTR_POSITION, 2, GL_FLOAT, GL_FALSE,
                        sizeof(Vertex2DColor),
                        (const GLvoid *)offsetof(Vertex2DColor, m_position));
  glVertexAttribPointer(VERTEX_ATTR_COLOR, 3, GL_FLOAT, GL_FALSE,
                        sizeof(Vertex2DColor),
                        (const GLvoid *)offsetof(Vertex2DColor, m_color));
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  glBindVertexArray(0);

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

    glClear(GL_COLOR_BUFFER_BIT);

    glBindVertexArray(vao);

    // => On utilise glDrawElements à la place de glDrawArrays
    // Cela indique à OpenGL qu'il doit utiliser l'IBO enregistré dans le VAO
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    glBindVertexArray(0);

    // Update the display
    windowManager.swapBuffers();
  }

  glDeleteBuffers(1, &vbo);
  glDeleteVertexArrays(1, &vao);

  return EXIT_SUCCESS;
}