#include <GL/glew.h>
#include <cstddef>
#include <cstdint>
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
  SDLWindowManager windowManager(800, 600, "Disk");

  // Initialize glew for OpenGL3+ support
  GLenum glewInitError = glewInit();
  if (GLEW_OK != glewInitError) {
    std::cerr << glewGetErrorString(glewInitError) << std::endl;
    return EXIT_FAILURE;
  }

  // Load shaders
  FilePath applicationPath(argv[0]);
  Program program =
      loadProgram(applicationPath.dirPath() + "shaders/triangle.vs.glsl",
                  applicationPath.dirPath() + "shaders/triangle.fs.glsl");
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

  std::vector<Vertex2DColor> vertices;

  int n_sectors = 200;
  float R = 0.5;

  // centre
  vertices.push_back(Vertex2DColor(glm::vec2(0, 0), glm::vec3(0, 0, 1)));

  for (int i = 0; i < n_sectors; i++) {

    // sommet i
    vertices.push_back(Vertex2DColor(
        glm::vec2(R * glm::cos(2 * i * glm::pi<float>() / (float)n_sectors),
                  R * glm::sin(2 * i * glm::pi<float>() / (float)n_sectors)),
        glm::vec3(1, 0, 0)));
  }

  // => Tableau d'indices: ce sont les indices des sommets à dessiner
  // On en a 6 afin de former deux triangles
  // Chaque indice correspond au sommet correspondant dans le VBO
  std::vector<GLuint> indices;
  for (int i = 1; i < n_sectors; i++) {
    indices.push_back(0);
    indices.push_back(i);
    indices.push_back(i + 1);
  }
  indices.push_back(0);
  indices.push_back(n_sectors);
  indices.push_back(1);

  glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex2DColor),
               vertices.data(), GL_STATIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  // => Creation du IBO
  GLuint ibo;
  glGenBuffers(1, &ibo);

  // => On bind sur GL_ELEMENT_ARRAY_BUFFER, cible reservée pour les IBOs
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);

  // => On remplit l'IBO avec les indices:
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(uint32_t),
               indices.data(), GL_STATIC_DRAW);

  // => Comme d'habitude on debind avant de passer à autre chose
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

  // VAO (vertex specification)
  GLuint vao;
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);

  // => On bind l'IBO sur GL_ELEMENT_ARRAY_BUFFER; puisqu'un VAO est
  // actuellement bindé, cela a pour effet d'enregistrer l'IBO dans le VAO
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);

  // Activation of vertex attributes
  const GLuint VERTEX_ATTR_POSITION = 3;
  const GLuint COLOR_ATTR_POSITION = 8;

  glEnableVertexAttribArray(VERTEX_ATTR_POSITION);
  glEnableVertexAttribArray(COLOR_ATTR_POSITION);

  // Specification of vertex attributes
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glVertexAttribPointer(VERTEX_ATTR_POSITION, 2, GL_FLOAT, GL_FALSE,
                        sizeof(Vertex2DColor),
                        (const GLvoid *)offsetof(Vertex2DColor, m_position));
  glVertexAttribPointer(COLOR_ATTR_POSITION, 3, GL_FLOAT, GL_FALSE,
                        sizeof(Vertex2DColor),
                        (const GLvoid *)offsetof(Vertex2DColor, m_color));
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

    // => On utilise glDrawElements à la place de glDrawArrays
    // Cela indique à OpenGL qu'il doit utiliser l'IBO enregistré dans le VAO
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);

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
