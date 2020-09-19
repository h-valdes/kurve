#ifndef PLAYER_MODEL_HPP
#define PLAYER_MODEL_HPP

#include "model.hpp"
#include "user.hpp"
#include "player_mesh.hpp"
#include "point.hpp"

extern "C" {
// Include the GLAD loader *before* including GLFW!
#include "glad/glad.h"

// Include the GLFW library (should be the same for all OS):
#include <GLFW/glfw3.h>

#include <math.h>
}

#include <vector>

#define Y_ANGULAR_VELOCITY 2

class PlayerModel : public Model {
   protected: 
    virtual void init_uniforms() override;
    virtual void init_values() override;
    std::vector<Point> points;
    // Controls
    int left_key;
    int right_key;
    int up_key;
    int down_key;
   public:
    // The uniforms locations
    GLint trans_y_loc;
    GLint trans_x_loc;
    GLfloat start_pos_y_loc;
    GLfloat start_pos_x_loc;

    double time;

    // The Uniform values
    GLfloat trans_y;
    GLfloat trans_x;
    GLfloat start_pos_y;
    GLfloat start_pos_x;


    PlayerModel(GLfloat x, GLfloat y);
    virtual void update(GLFWwindow* window) override;
    virtual void draw() override;
    void set_keys(int left_key, int right_key, int up_key, int down_key);
};

#endif