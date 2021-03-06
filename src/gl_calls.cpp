#include "gl_calls.hpp"

#include <iostream>

extern "C" {
#include <stdio.h>
#include <stdlib.h>
}

void check_error(int condition, const char* error_text) {
    if (!condition) {
        fprintf(stderr, "%s\n", error_text);
        exit(EXIT_FAILURE);
    }
}

void gl_check_error(const char* error_text) {
    GLenum error = glGetError();

    if (error != GL_NO_ERROR) {
        fprintf(stderr, "GLError: %s (%d)\n", error_text, error);
        exit(EXIT_FAILURE);
    }
}

void init_gl(GLFWwindow* window) {
#ifdef DEBUG
    std::cout << "---- INIT GL ----" << std::endl;
#endif
    
    // Obtain the internal size of the framebuffer:
    int fb_width, fb_height;
    glfwGetFramebufferSize(window, &fb_width, &fb_height);

    // Align the viewport to the framebuffer:
    glViewport(0, 0, fb_width, fb_height);
    gl_check_error("glViewport");

    // Specify the clear color:
    glClearColor(0.1, 0.1, 0.1, 1);
    gl_check_error("glClearColor");

    glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}
