#include "font.hpp"
#include "shader.hpp"
#include "gl_calls.hpp"
#include <iostream>
#include <string>

SubFont::SubFont(GLFWwindow* window, std::string path, int font_size) {
#ifdef DEBUG
    std::cout << "---- INIT SUBFONT ----" << std::endl;
#endif
    this->window = window;
    FT_Library ft;
    if (FT_Init_FreeType(&ft)) {
        std::cout << 
            "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
        return;
    }

    FT_Face face;
    std::string font_path(STATIC_FILES);
    font_path.append(path);
    if (FT_New_Face(ft, font_path.c_str(), 0, &face)) {
        std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;
        return;
    }

    FT_Set_Pixel_Sizes(face, 0, font_size);

    if (FT_Load_Char(face, 'X', FT_LOAD_RENDER)) {
        std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
        return;
    }

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);  // disable byte-alignment restriction

    std::string vs_path(STATIC_FILES);
    vs_path.append("/shader/font.vs");

    std::string fs_path(STATIC_FILES);
    fs_path.append("/shader/font.fs");
    Shader shader(
        vs_path.c_str(), 
        fs_path.c_str(), 
        &this->shader_id);

    for (unsigned char c = 0; c < 128; c++) {
        // load character glyph
        if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
            std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
            continue;
        }
        // generate texture
        unsigned int texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RED,
            face->glyph->bitmap.width,
            face->glyph->bitmap.rows,
            0,
            GL_RED,
            GL_UNSIGNED_BYTE,
            face->glyph->bitmap.buffer);
        // set texture options
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        // now store character for later use
        Character character = {
            texture,
            glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
            glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
            static_cast<unsigned int>(face->glyph->advance.x)};
        this->Characters.insert(std::pair<char, Character>(c, character));
    }
    glBindTexture(GL_TEXTURE_2D, 0);

    // Destroy FreeType
    FT_Done_Face(face);
    FT_Done_FreeType(ft);

    glGenVertexArrays(1, &this->VAO);
    glBindVertexArray(this->VAO);

    glGenBuffers(1, &this->VBO);
    glBindBuffer(GL_ARRAY_BUFFER, this->VBO);

    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
    
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);  
}


Font::Font(GLFWwindow* window) {
    // Testing FreeType
#ifdef DEBUG
    std::cout << "---- INIT FONT ----" << std::endl;
#endif
    std::vector<int> font_sizes {24, 48, 150};
    std::vector<std::string> size_names {"small", "medium", "big"};
    std::map<std::string, std::string> type_names;
    type_names.insert(std::make_pair("regular", "/fonts/UbuntuMono-R.ttf")); 
    type_names.insert(std::make_pair("bold", "/fonts/UbuntuMono-R.ttf"));
    type_names.insert(std::make_pair("regular_italic", "/fonts/UbuntuMono-R.ttf")); 
    type_names.insert(std::make_pair("bold_italic", "/fonts/UbuntuMono-R.ttf"));
    
    int count = 0;
    for (auto item : type_names) {
        auto type_name = item.first;
        auto path = item.second;
        for (auto size_name : size_names) {
            std::string new_name = type_name + "_" + size_name;
            int font_size = font_sizes.at(count);
            auto sub_font = std::make_shared<SubFont>(
                this->window, 
                path,
                font_size);
            subfonts.insert(std::make_pair(new_name, sub_font));
            count++;
        }
        count = 0;
    }
}

Font::~Font() {
#ifdef DEBUG
    std::cout << "---- DESTROY Font ----" << std::endl;
#endif
    // TODO: Fix the segmentation fault!
    // glDeleteVertexArrays(1, &this->VAO);
    // gl_check_error("glDeleteVertexArrays");

    // glDeleteBuffers(1, &this->VBO);
    // gl_check_error("glDeleteBuffers");

    // glDeleteProgram(this->shader_id);
    // gl_check_error("glDeleteProgram");
}
void Font::draw_text(
    std::string text, 
    float x, 
    float y, 
    float scale, 
    glm::vec3 color,
    bool is_blinking,
    std::string font_type) {
    if (is_blinking == true) {
        if (this->is_drawn == false) {
            return;
        }
    }
    subfonts.at(font_type)->draw_text(
            text, 
            x, 
            y, 
            scale, 
            color,
            is_blinking);
}

void SubFont::draw_text(
    std::string text, 
    float x, 
    float y, 
    float scale, 
    glm::vec3 color,
    bool is_blinking) {
	glUseProgram(this->shader_id);
    glUniform3f(glGetUniformLocation(this->shader_id, "textColor"), color.x, color.y, color.z);
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(this->VAO);
    glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
    // iterate through all characters
    std::string::const_iterator c;
    for (c = text.begin(); c != text.end(); c++)
    {
        Character ch = this->Characters[*c];

        float xpos = x + ch.Bearing.x * scale;
        float ypos = y - (ch.Size.y - ch.Bearing.y) * scale;

        float w = ch.Size.x * scale;
        float h = ch.Size.y * scale;
        // update VBO for each character
        float vertices[6][4] = {
            { xpos,     ypos + h,   0.0f, 0.0f },            
            { xpos,     ypos,       0.0f, 1.0f },
            { xpos + w, ypos,       1.0f, 1.0f },
            { xpos,     ypos + h,   0.0f, 0.0f },
            { xpos + w, ypos,       1.0f, 1.0f },
            { xpos + w, ypos + h,   1.0f, 0.0f }           
        };
        // render glyph texture over quad
        glBindTexture(GL_TEXTURE_2D, ch.TextureID);

        // update content of VBO memory
        glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
        gl_check_error("glBindBuffer");
        
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); 

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        gl_check_error("glBindBuffer");
        
        // render quad
        glDrawArrays(GL_TRIANGLES, 0, 6);

        // now advance cursors for next glyph (note that advance is number of 1/64 pixels)
        x += (ch.Advance >> 6) * scale; // bitshift by 6 to get value in pixels (2^6 = 64)
    }
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Font::set_delta_time(double delta_time) {
    this->active_time += delta_time;
    if (this->is_drawn == true) {
        if (this->active_time > this->blink_time) {
            this->is_drawn = false;
            this->active_time = 0;
        }
    } else {
        if (this->active_time > this->blink_time) {
            this->is_drawn = true;
            this->active_time = 0;
        }
    }
}