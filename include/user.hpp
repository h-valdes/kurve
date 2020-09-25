#ifndef USER_HPP
#define USER_HPP

#include "gl_calls.hpp"


extern "C" {
// Include the GLAD loader *before* including GLFW!
#include "glad/glad.h"

// Include the GLFW library (should be the same for all OS):
#include <GLFW/glfw3.h>
}

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <vector>
#include <map>
#include <string>

struct Control {
    int left_key; 
    int right_key;
};

enum GameState {
    GAME_ACTIVE,
    GAME_MENU,
    GAME_WIN,
    GAME_OVER, 
    GAME_PAUSE,
    GAME_TRANSITION
};

typedef struct {
    bool is_active;
    int id;
    std::string name;
    std::string menu_text;
    Control control;
    std::array<GLubyte, 3> color;
    glm::vec3 menu_color;
    int score;
} player_info_t;

typedef struct {
    // Dimensions of the window:
    int window_width;
    int window_height;
    GameState game_state;
    std::vector<player_info_t>* player_info;
} user_data_t;

#endif