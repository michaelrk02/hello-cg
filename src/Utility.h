#pragma once

#include <GL/glew.h>
#include <string>

class Utility {
public:
    static std::string readFileContents(const std::string &fileName);
    static GLuint compileShader(GLenum type, const std::string &source);
};

