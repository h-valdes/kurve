#include "grid.hpp"
#include "point.hpp"
#include <cmath>
#include <iostream>

Grid::Grid() {
#ifdef DEBUG
    std::cout << "---- INIT Grid ----" << std::endl;
#endif
    this->width = this->right_limit - this->left_limit;
    this->height = this->top_limit - this->bottom_limit;

    // Determine the number of horizontal/vertical cells
    if (this->height >= this->width) {
        this->horizontal_cells = static_cast<int>(this->width / this->height * 100); 
        this->vertical_cells = 100;
    } else {
        this->vertical_cells = static_cast<int>(this->height / this->width * 100); 
        this->horizontal_cells = 100;
    }

    // Determine the width and height of every cell
    this->cell_width = this->width / (double) this->horizontal_cells;
    this->cell_height = this->height / (double) this->vertical_cells;

    // Create matrix full of 0
    for (int i = 0; i < horizontal_cells; i++) {
        std::vector<bool> temp_vec;
        for (int j = 0; j < vertical_cells; j++) {
            temp_vec.push_back(false);
        }
        this->matrix.push_back(temp_vec);
    }
}   

void Grid::populate(std::vector<glm::vec2> all_points) {    
    if (all_points.size() > 0) {
        for (auto point : all_points) {
    
            auto coordinates = this->get_coordinates(point.x, point.y);
            this->matrix.at(coordinates.first).at(coordinates.second) = true;
        }
    }
}

std::pair<int, int> Grid::get_coordinates(double x, double y) {
    double x_diff = x - this->left_limit; 
    double y_diff = this->top_limit - y;

    // Determine the coordinates on the grid
    int x_pos = static_cast<int>(floor(x_diff / this->cell_width)) - 1;
    int y_pos = static_cast<int>(floor(y_diff / this->cell_height)) - 1;

    auto pair = std::make_pair(x_pos, y_pos); 
    return pair;
}

void Grid::clear() {
    for (int i = 0; i < horizontal_cells; i++) {
        for (int j = 0; j < vertical_cells; j++) {
            this->matrix.at(i).at(j) = false;
        }
    }
}

int Grid::direction_to_cuadrant(glm::vec2 direction) {
    double x_diff = direction.x;
    double y_diff = direction.y;

    glm::vec2 vec1 {1, 0};
    glm::vec2 vec2 {x_diff, y_diff};
    auto rotation = cross_product(vec1, vec2);
    double angle;

    if (rotation > 0) {
        angle = get_angle(vec1, vec2);
    } else if (rotation < 0) {
        angle = 2 * M_PI - get_angle(vec1, vec2);
    } else {
        angle = 0;
    }

    angle = angle * 180 / M_PI;

    int cuadrant = 0;
    double last_angle = 45 / 2;

    for (int i = 1; i < 8; i++) {
        if (angle >= (360 - (45 / 2)) || angle < (45 / 2)) {
            cuadrant = 0;
            break;
        }

        if (angle >= (last_angle) && angle < (last_angle + 45)) {
            cuadrant = i;
            break;
        } 

        last_angle += 45;
    }
    
    return cuadrant;
}

std::vector<std::pair<int, int>> Grid::get_neighbours(
    std::pair<int, int> center, int direction_cuadrant) {
    std::vector<std::pair<int, int>> neighbours;

    // Neighbour 0
    neighbours.push_back(std::make_pair(center.first + 1, center.second));

    // Neighbour 1
    neighbours.push_back(std::make_pair(center.first + 1, center.second + 1));

    // Neighbour 2
    neighbours.push_back(std::make_pair(center.first, center.second + 1));

    // Neighbour 3
    neighbours.push_back(std::make_pair(center.first - 1, center.second + 1));

    // Neighbour 4
    neighbours.push_back(std::make_pair(center.first - 1, center.second));

    // Neighbour 5
    neighbours.push_back(std::make_pair(center.first - 1, center.second - 1));

    // Neighbour 6
    neighbours.push_back(std::make_pair(center.first, center.second - 1));

    // Neighbour 7
    neighbours.push_back(std::make_pair(center.first + 1, center.second - 1));

    /*  Check which neighbours are valid. It depends of the movement 
        constraints of the player

        |-|^|-|      |x|x|x|  
        |-|c|-| -->  |-|c|-|
        |-|-|-|      |-|-|-|
    */
    std::vector<std::pair<int, int>> valid_neighbours;
    if (direction_cuadrant == 0) {
        valid_neighbours.push_back(neighbours[1]);
        valid_neighbours.push_back(neighbours[0]);
        valid_neighbours.push_back(neighbours[7]);
    } else if (direction_cuadrant == 7) {
        valid_neighbours.push_back(neighbours[0]);
        valid_neighbours.push_back(neighbours[7]);
        valid_neighbours.push_back(neighbours[6]);
    } else {
        valid_neighbours.push_back(neighbours[direction_cuadrant + 1]);
        valid_neighbours.push_back(neighbours[direction_cuadrant]);
        valid_neighbours.push_back(neighbours[direction_cuadrant - 1]);
    }

    return valid_neighbours;
}