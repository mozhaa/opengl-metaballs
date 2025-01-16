#pragma once

#include <GL/glew.h>
#include <filesystem>
#include <string>
#include <vector>

std::string get_shader_source(std::filesystem::path source_file);

GLuint load_shader(std::filesystem::path source_file);

std::vector<GLuint> load_shaders(std::vector<std::filesystem::path> source_files);

GLuint create_program(std::vector<std::filesystem::path> source_files);