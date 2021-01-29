#include <fstream>
#include <sstream>
#include <stdexcept>

#include "Utility.h"

std::string Utility::readFileContents(const std::string &fileName) {
    std::ifstream input(fileName.c_str());
    if (!input.is_open()) {
        std::ostringstream message;
        message << "unable to read file: " << fileName;
        throw std::runtime_error(message.str().c_str());
    }
    size_t fileSize = input.seekg(0, std::ios::end).tellg();
    input.seekg(0);
    std::string contents(fileSize, '\0');
    input.read(&contents[0], fileSize);
    input.close();
    return contents;
}

GLuint Utility::compileShader(GLenum type, const std::string &source) {
    GLuint shader = glCreateShader(type);
    const char *data = source.c_str();
    glShaderSource(shader, 1, &data, NULL);
    glCompileShader(shader);
    GLint status;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
    if (status != GL_TRUE) {
        std::ostringstream message;
        message << "shader compilation failed" << std::endl;
        char compileLog[2048];
        glGetShaderInfoLog(shader, 2048, NULL, compileLog);
        message << compileLog;
        throw std::runtime_error(message.str().c_str());
    }
    return shader;
}

