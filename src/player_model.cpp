#include "player_model.hpp"

#include <iostream>

#include "gl_calls.hpp"
#include "player_mesh.hpp"
#include "shader.hpp"

PlayerModel::PlayerModel(
    int id, GLfloat x, GLfloat y, std::array<GLubyte, 3> color) {       
    std::cout << "---- INIT PLAYER ----" << std::endl;
    this->model_type = MODEL_PLAYER;
    this->color = color;
    this->id = id;

    // Compile and add the shaders
    Shader shader("../shader/player.vs", "../shader/player.fs", &this->shader_id);
    // Init the uniforms
    this->init_uniforms();

    // Add the mesh
    auto mesh = std::make_shared<PlayerMesh>();
    this->mesh = mesh;

    // Init the values of the model
    this->start_pos_x = x;
    this->start_pos_y = y;
    this->init_values();

    // Add initial positions into the point vector
    Point point{x, y};
    this->points.push_back(point);

    // Create a line model for the player
    auto line = std::make_shared<LineModel>(point, this->color);
    this->line_model = line;
}

void PlayerModel::draw() {
    glUseProgram(this->shader_id);
    this->mesh->draw();

    if (this->line_model != nullptr) {
        line_model->draw();
    }
}    

void PlayerModel::update(GLFWwindow* window) {
    // Update the time and calculate the delta:
    glUseProgram(this->shader_id);
    double new_time = glfwGetTime();
    double time_delta = new_time - this->time;

    this->time = new_time;
    GLfloat x_diff = 0;
    GLfloat y_diff = 0;

    int right_state = glfwGetKey(window, this->control.right_key);
    if (right_state == GLFW_PRESS) {
        x_diff = static_cast<GLfloat>((VELOCITY * time_delta));
        this->trans_x = this->trans_x + x_diff;
    }

    int left_state = glfwGetKey(window, this->control.left_key);
    if (left_state == GLFW_PRESS) {
        x_diff = static_cast<GLfloat>((-VELOCITY * time_delta));
        this->trans_x = this->trans_x + x_diff;
    }

    int up_state = glfwGetKey(window, this->control.up_key);
    if (up_state == GLFW_PRESS) {
        y_diff = static_cast<GLfloat>((VELOCITY * time_delta));
        this->trans_y = this->trans_y + y_diff;
    }

    int down_state = glfwGetKey(window, this->control.down_key);
    if (down_state == GLFW_PRESS) {
        y_diff = static_cast<GLfloat>((-VELOCITY * time_delta));
        this->trans_y = this->trans_y + y_diff;
    }

    // There was a movement, add a new point!
    if (x_diff != 0 || y_diff !=0) {
        Point last_point = this->points.back();
        Point point{
            this->trans_x + x_diff + this->start_pos_x,
            this->trans_y + y_diff + this->start_pos_y
        };
        this->points.push_back(point);

        if (this->line_model != nullptr) {          
            this->line_model->add_point(point);
            this->line_model->update(window); 
        }
    }

    // Update the uniform:
    glUniform1f(this->trans_y_loc, this->trans_y);
    gl_check_error("glUniform1f [trans_y]");

    glUniform1f(this->trans_x_loc, this->trans_x);
    gl_check_error("glUniform1f [trans_x]");
}

void PlayerModel::init_uniforms() {
    std::cout << "Init uniforms" << std::endl;
    // Y trans:
    this->trans_y_loc = glGetUniformLocation(this->shader_id, "trans_y");
    gl_check_error("glGetUniformLocation [trans_y]");
    check_error(this->trans_y_loc >= 0, "Failed to obtain uniform location for trans_y.");

    // X trans:
    this->trans_x_loc = glGetUniformLocation(this->shader_id, "trans_x");
    gl_check_error("glGetUniformLocation [trans_x]");
    check_error(this->trans_x_loc >= 0, "Failed to obtain uniform location for trans_x.");

    // Start pos Y:
    this->start_pos_y_loc = glGetUniformLocation(this->shader_id, "start_pos_y");
    gl_check_error("glGetUniformLocation [trans_y]");
    check_error(this->start_pos_y_loc >= 0, "Failed to obtain uniform location for start_pos_y.");

    // Start pos X:
    this->start_pos_x_loc = glGetUniformLocation(this->shader_id, "start_pos_x");
    gl_check_error("glGetUniformLocation [trans_x]");
    check_error(this->start_pos_x_loc >= 0, "Failed to obtain uniform location for start_pos_x.");
}

void PlayerModel::init_values() {
    this->time = glfwGetTime();
    this->trans_y = 0;
    this->trans_x = 0;

    glUniform1f(this->start_pos_x_loc, this->start_pos_x);
    gl_check_error("glUniform1f [start_pos_x]");

    glUniform1f(this->start_pos_y_loc, this->start_pos_y);
    gl_check_error("glUniform1f [start_pos_y]");
}

void PlayerModel::set_keys(Control control) {
    this->control = control;
}

void PlayerModel::add_line_model(std::shared_ptr<LineModel> line_model) {
    this->line_model = line_model;
}

Point PlayerModel::get_position() {
    return this->points.back();
}

std::vector<Point> PlayerModel::get_line_points() {
    return this->points;
}
