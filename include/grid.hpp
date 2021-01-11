#ifndef GRID_HPP
#define GRID_HPP

#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

/*  Grid Notation for a pair of coordinates c
    from 0 to 1 neighbours of the center
    |3|2|1|
    |4|c|0|
    |5|6|7|
*/
class Grid {
   private:
    double left_limit = -18.75;
    double right_limit = 13.5;
    double top_limit = 18.75;
    double bottom_limit = -18.75;
    double width;
    double height;
    int horizontal_cells;
    int vertical_cells;
    double cell_width;
    double cell_height;
    std::vector<std::vector<bool>> matrix;
   public:
    Grid();
    void populate(std::vector<glm::vec2> all_points);
    std::pair<int, int> get_coordinates(double x, double y);
    int direction_to_cuadrant(glm::vec2 direction);
    std::vector<std::pair<int, int>> get_neighbours(
        std::pair<int, int> center, int direction_cuadrant);
    void clear();    
};

#endif