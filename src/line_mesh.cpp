#include "line_mesh.hpp"

#include <iostream>
#include <cmath>

void LineMesh::update() {
    if (this->points.size() > 0 ) {
        this->bind();
        std::vector<vertex_data_t> vertex_data;

        for (auto point : this->points) {        
            vertex_data.push_back(
                {
                    .position = {
                        static_cast<GLfloat>(point.x), 
                        static_cast<GLfloat>(point.y), 
                        0
                    }, 
                    .color = this->color
                }
            );
        }

        // Upload the vertex data to the GPU:
        glBufferData(
            GL_ARRAY_BUFFER, 
            this->points.size() * sizeof(vertex_data_t), 
            (const GLvoid*)vertex_data.data(), 
            GL_DYNAMIC_DRAW);
        gl_check_error("glBufferData");
    }
}

void LineMesh::set_points(std::vector<glm::vec2> points) {
    this->points = points;
}

void LineMesh::add_point(glm::vec2 point) {
    if (this->line_points.size() == 0) {
        this->line_points.push_back(point);
    } else if (this->line_points.size() == 1) {
        this->last_point = this->line_points.back();
        this->line_points.push_back(point);
    } else {
        this->second_last_point = this->last_point;
        this->last_point = this->line_points.back();

        glm::vec2 v1 = this->second_last_point - this->last_point; // second last - last
        glm::vec2 v2 = point - this->last_point; // actual - last
        
        double angle = get_angle(v1, v2);
        // std::cout << "angle: " << angle << std::endl;
        double direction = cross_product(v1, v2);
        direction = round(direction);
        // std::cout << "direction: " << direction << std::endl;

        GLfloat r{0.17};
        glm::vec2 left_point, right_point;

        double v2_length = get_length(v2);
        v2.x = (v2.x / (GLfloat) v2_length) * r;
        v2.y = (v2.y / (GLfloat) v2_length) * r;

        if (direction < 0) {
            // Rotation clockwise
            angle = angle / 2;
            left_point = {
                v2.x * (GLfloat)cos(M_PI - angle) - v2.y * (GLfloat)sin(M_PI - angle),
                v2.x * (GLfloat)sin(M_PI - angle) + v2.y * (GLfloat)cos(M_PI - angle)};
            right_point = {
                v2.x * (GLfloat)cos(-angle) - v2.y * (GLfloat)sin(-angle),
                v2.x * (GLfloat)sin(-angle) + v2.y * (GLfloat)cos(-angle)};

        } else if (direction > 0) {
            // Rotation anti-clockwise
            angle = angle / 2;
            left_point = {
                v2.x * (GLfloat)cos(angle) - v2.y * (GLfloat)sin(angle),
                v2.x * (GLfloat)sin(angle) + v2.y * (GLfloat)cos(angle)};
            right_point = {
                v2.x * (GLfloat)cos(-M_PI + angle) - v2.y * (GLfloat)sin(-M_PI + angle),
                v2.x * (GLfloat)sin(-M_PI + angle) + v2.y * (GLfloat)cos(-M_PI + angle)};
        } else {
            angle = M_PI/2;
            left_point = {
                v2.x * (GLfloat)cos(angle) - v2.y * (GLfloat)sin(angle),
                v2.x * (GLfloat)sin(angle) + v2.y * (GLfloat)cos(angle)};
            right_point = {
                v2.x * (GLfloat)cos(-angle) - v2.y * (GLfloat)sin(-angle),
                v2.x * (GLfloat)sin(-angle) + v2.y * (GLfloat)cos(-angle)};
        }

        left_point.x += this->last_point.x;
        left_point.y += this->last_point.y;

        right_point.x += this->last_point.x;
        right_point.y += this->last_point.y;

        // std::cout << "Diameter: " << Point::get_distance(left_point, right_point) << std::endl;

        this->line_points.push_back(point);
        this->points.push_back(left_point);
        this->points.push_back(right_point);
    }

}

LineMesh::LineMesh(glm::vec2 first_point, glm::vec3 color) {
    this->color = color;
    
    std::vector<vertex_data_t> vertex_data;
    vertex_data.push_back(
        {
            .position = {
                static_cast<GLfloat>(first_point.x), 
                static_cast<GLfloat>(first_point.y), 
                0
            }, 
            .color = this->color
        }
    );

    // Create a VAO
    GLuint vao;

    glGenVertexArrays(1, &vao);
    gl_check_error("glGenVertexArrays");

    glBindVertexArray(vao);
    gl_check_error("glBindVertexArray");

    this->vao = vao;

    // Generate and bind a vertex buffer object (VBO):
    GLuint vbo;

    glGenBuffers(1, &vbo);
    gl_check_error("glGenBuffers");

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    gl_check_error("glBindBuffer");
    
    this->vbo = vbo;

    // Upload the vertex data to the GPU:
    glBufferData(
        GL_ARRAY_BUFFER, 
        this->points.size() * sizeof(vertex_data_t), 
        (const GLvoid*)vertex_data.data(), 
        GL_STATIC_DRAW);
    gl_check_error("glBufferData");

    // Position attribute:
    // Number of attribute, number of components, 
    // data type, normalize, stride, pointer (= offset)
    glVertexAttribPointer(
        ATTRIB_POSITION, 
        3, 
        GL_FLOAT, 
        GL_FALSE, 
        sizeof(vertex_data_t), 
        (GLvoid*)offsetof(vertex_data_t, position));
    gl_check_error("glVertexAttribPointer [position]");

    glVertexAttribPointer(
        ATTRIB_COLOR, 
        3, 
        GL_FLOAT, 
        GL_FALSE, 
        sizeof(vertex_data_t), 
        (GLvoid*)offsetof(vertex_data_t, color));
    gl_check_error("glVertexAttribPointer [color]");

    // Enable the attributes:
    glEnableVertexAttribArray(ATTRIB_POSITION);
    gl_check_error("glEnableVertexAttribArray [position]");

    glEnableVertexAttribArray(ATTRIB_COLOR);
    gl_check_error("glEnableVertexAttribArray [color]");

}

LineMesh::~LineMesh() {
#ifdef DEBUG
    std::cout << "---- DESTROY LineMesh ----" << std::endl;
#endif
    glDeleteVertexArrays(1, &this->vao);
    gl_check_error("glDeleteVertexArrays");

    glDeleteBuffers(1, &this->vbo);
    gl_check_error("glDeleteBuffers");
}

void LineMesh::draw() {
    if (this->points.size() > 0) {
        this->bind();
        glDrawArrays(GL_TRIANGLE_STRIP, 0, this->points.size());
        gl_check_error("glDrawArrays");
    }
}

void LineMesh::clear() {
    this->line_points.clear();
    this->points.clear();   
}