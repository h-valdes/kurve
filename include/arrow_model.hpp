#ifndef ARROW_MODEL_HPP
#define ARROW_MODEL_HPP

#include "model.hpp"
#include "arrow_mesh.hpp"
#include "point.hpp"
#include "user.hpp"

extern "C" {
// Include the GLAD loader *before* including GLFW!
#include "glad/glad.h"

// Include the GLFW library (should be the same for all OS):
#include <GLFW/glfw3.h>
#include <math.h>
}

#include <array>
#include <memory>
#include <vector>

#define VELOCITY 10

class ArrowModel : public Model {
   protected:
    virtual void init_uniforms() override;
    virtual void init_values() override;

    glm::vec3 color;
    glm::vec2 direction;

   public:
    // The uniforms locations
    GLint trans_y_loc;
    GLint trans_x_loc;
    GLfloat start_pos_y_loc;
    GLfloat start_pos_x_loc;

    // The Uniform values
    GLfloat trans_y;
    GLfloat trans_x;
    GLfloat start_pos_y;
    GLfloat start_pos_x;

    ArrowModel(int id, GLfloat x, GLfloat y, glm::vec3 color, glm::vec2 direction);
    ~ArrowModel();
    virtual void update(GLFWwindow* window) override;
    virtual void draw() override;
};

#endif