#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <cstddef>
#include <glimac/FilePath.hpp>
#include <glimac/Image.hpp>
#include <glimac/Program.hpp>
#include <glimac/SDLWindowManager.hpp>
#include <glimac/Sphere.hpp>
#include <glimac/TrackballCamera.hpp>
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

  GLint Kd_location;
  GLint Ks_location;
  GLint Shininess_location;
  GLint LightDir_vs_location;
  GLint LightIntensity_location;

  EarthProgram(const FilePath &applicationPath)
      : m_Program(loadProgram(applicationPath.dirPath() + "shaders/3D.vs.glsl",
                              applicationPath.dirPath() +
                                  "shaders/directionallight.fs.glsl")) {

    // Get Matrix locations
    MVP_location = glGetUniformLocation(m_Program.getGLId(), "uMVPMatrix");
    MV_location = glGetUniformLocation(m_Program.getGLId(), "uMVMatrix");
    Normal_location =
        glGetUniformLocation(m_Program.getGLId(), "uNormalMatrix");

    // Get Light variables locations
    Kd_location = glGetUniformLocation(m_Program.getGLId(), "uKd");
    Ks_location = glGetUniformLocation(m_Program.getGLId(), "uKs");
    Shininess_location =
        glGetUniformLocation(m_Program.getGLId(), "uShininess");
    LightDir_vs_location =
        glGetUniformLocation(m_Program.getGLId(), "uLightDir_vs");
    LightIntensity_location =
        glGetUniformLocation(m_Program.getGLId(), "uLightIntensity");
  }
};

struct MoonProgram {
  Program m_Program;

  GLint MVP_location;
  GLint MV_location;
  GLint Normal_location;

  GLint Kd_location;
  GLint Ks_location;
  GLint Shininess_location;
  GLint LightDir_vs_location;
  GLint LightIntensity_location;

  MoonProgram(const FilePath &applicationPath)
      : m_Program(loadProgram(applicationPath.dirPath() + "shaders/3D.vs.glsl",
                              applicationPath.dirPath() +
                                  "shaders/directionallight.fs.glsl")) {

    // Get Matrix locations
    MVP_location = glGetUniformLocation(m_Program.getGLId(), "uMVPMatrix");
    MV_location = glGetUniformLocation(m_Program.getGLId(), "uMVMatrix");
    Normal_location =
        glGetUniformLocation(m_Program.getGLId(), "uNormalMatrix");

    // Get Light variables locations
    Kd_location = glGetUniformLocation(m_Program.getGLId(), "uKd");
    Ks_location = glGetUniformLocation(m_Program.getGLId(), "uKs");
    Shininess_location =
        glGetUniformLocation(m_Program.getGLId(), "uShininess");
    LightDir_vs_location =
        glGetUniformLocation(m_Program.getGLId(), "uLightDir_vs");
    LightIntensity_location =
        glGetUniformLocation(m_Program.getGLId(), "uLightIntensity");
  }
};

int main(int argc, char **argv) {

  // Initialize SDL and open a window
  SDLWindowManager windowManager(800, 600, "lumiaire svp");

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

  glEnable(GL_DEPTH_TEST);

  // Camera
  TrackballCamera C;

  // Initialize Matrix

  glm::mat4 VMatrix = C.getViewMatrix();
  glm::mat4 ProjMatrix =
      glm::perspective<float>(glm::radians(70.f), 800. / 600., 0.1f, 100.f);
  glm::mat4 MVMatrix = translate(0., 0., -5.);
  glm::mat4 NormalMatrix = glm::transpose(glm::inverse(MVMatrix));

  // Lights
  glm::vec3 Kd = glm::vec3(1., 1., 1.);
  glm::vec3 Ks = glm::vec3(1.);
  float Shininess = 15.;

  glm::vec3 LightDir_vs = glm::vec3(0., 0., 1.);
  glm::vec3 LightIntensity = glm::vec3(1., 0., 1.); // RED

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

  /*********************************
   * INITIALIZATION END
   *********************************/

  // Application loop:
  bool done = false;
  int nb_lunes = 10;

  // List of rotation axis of moons
  std::vector<glm::vec3> AxisList;
  for (int i = 0; i < nb_lunes; i++) {
    AxisList.push_back(glm::sphericalRand(2.5f));
  }

  // List of begin positions of moons
  std::vector<glm::vec3> BeginPosList;
  for (int i = 0; i < nb_lunes; i++) {
    BeginPosList.push_back(glm::sphericalRand(1.5f));
  }

  glm::vec2 startPos;
  glm::vec2 endPos;

  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);

  while (!done) {

    float time = windowManager.getTime();

    // Event loop:
    SDL_Event e;
    while (windowManager.pollEvent(e)) {

      if (e.type == SDL_QUIT) {
        done = true; // Leave the loop after this iteration
      }
    }
    glm::vec2 startPos = windowManager.getMousePosition();

    // Camera commands
    if (windowManager.isMouseButtonPressed(SDL_BUTTON_LEFT)) {
      C.rotateLeft((endPos - startPos).x);
      C.rotateUp((endPos - startPos).y);
    }
    if (windowManager.isMouseButtonPressed(SDL_BUTTON_RIGHT)) {
      C.moveFront((endPos - startPos).y / 100.);
    }

    /*********************************
     * RENDERING BEGIN
     *********************************/

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0., 0., 0.5, 1.);

    // Bind VAO
    glBindVertexArray(vao);

    /// EARTH ///
    earthProgram.m_Program.use();

    VMatrix = C.getViewMatrix();

    glm::mat4 MEarth = glm::rotate(glm::mat4(1.), time, glm::vec3(0., 1., 0.));
    glm::mat4 MVEarth = VMatrix * MEarth;
    glm::mat4 MVPEarth = ProjMatrix * MVEarth;
    glm::mat4 NormalEarth = glm::transpose(glm::inverse(MVEarth));

    glUniformMatrix4fv(earthProgram.MV_location, 1, GL_FALSE,
                       glm::value_ptr(MVEarth));
    glUniformMatrix4fv(earthProgram.MVP_location, 1, GL_FALSE,
                       glm::value_ptr(MVPEarth));

    NormalMatrix = glm::transpose(glm::inverse(VMatrix));
    glUniformMatrix4fv(earthProgram.Normal_location, 1, GL_FALSE,
                       glm::value_ptr(NormalEarth));

    glUniform3fv(earthProgram.Kd_location, 1, glm::value_ptr(Kd));
    glUniform3fv(earthProgram.Ks_location, 1, glm::value_ptr(Ks));
    glUniform1f(earthProgram.Shininess_location, Shininess);

    glm::vec4 LDMV4 =
        VMatrix * glm::vec4(LightDir_vs.x, LightDir_vs.y, LightDir_vs.z, 0.);
    glm::vec3 LDMV3 = glm::vec3(LDMV4.x, LDMV4.y, LDMV4.z);

    glUniform3fv(earthProgram.LightDir_vs_location, 1, glm::value_ptr(LDMV3));

    glUniform3fv(earthProgram.LightIntensity_location, 1,
                 glm::value_ptr(LightIntensity));

    glDrawArrays(GL_TRIANGLES, 0, S.getVertexCount()); // EARTH DRAW

    moonProgram.m_Program.use();
    for (int i = 0; i < nb_lunes; i++) {
      glm::mat4 MMoon = glm::scale(
          glm::translate(glm::rotate(glm::mat4(1.), time, AxisList[i]),
                         BeginPosList[i]),
          glm::vec3(0.2, 0.2, 0.2));
      // Translation * Rotation * Translation * Scale

      glm::mat4 MVMoon = VMatrix * MMoon;
      glm::mat4 MVPMoon = ProjMatrix * MVMoon;

      glUniformMatrix4fv(moonProgram.MVP_location, 1, GL_FALSE,
                         glm::value_ptr(MVPMoon));
      glUniformMatrix4fv(moonProgram.MV_location, 1, GL_FALSE,
                         glm::value_ptr(MVMoon));

      glm::mat4 NormalMoon = glm::transpose(glm::inverse(MVMoon));
      glUniformMatrix4fv(moonProgram.Normal_location, 1, GL_FALSE,
                         glm::value_ptr(NormalMoon));

      // glUniform3f(moonProgram.Kd_location, Kd.x, Kd.y, Kd.z);
      // glUniform3f(moonProgram.Ks_location, Ks.x, Ks.y, Ks.z);
      // glUniform1f(moonProgram.Shininess_location, Shininess);

      // glUniform3f(moonProgram.LightDir_vs_location, LightDir_vs.x,
      //             LightDir_vs.y, LightDir_vs.z);
      // glUniform3f(moonProgram.LightIntensity_location, LDMV3.x, LDMV3.y,
      //             LDMV3.z);

      glDrawArrays(GL_TRIANGLES, 0, S.getVertexCount());
    }

    glBindVertexArray(0);

    /*********************************
     * RENDERING END
     *********************************/

    // Update the display
    windowManager.swapBuffers();

    endPos = windowManager.getMousePosition();
  }

  // Debind vbo
  glDeleteBuffers(1, &vbo);

  // Debind vao
  glDeleteVertexArrays(1, &vao);

  return EXIT_SUCCESS;
}
