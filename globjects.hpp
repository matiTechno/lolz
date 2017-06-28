#pragma once

#include "nonmvcp.hpp"
#include "glad.h"
#include <map>
#include <string>

class Vao: public NonMvcp
{
public:
    Vao() {glGenVertexArrays(1, &id);}
    ~Vao() {glDeleteVertexArrays(1, &id);}
    GLuint id;
};

class Bo: public NonMvcp
{
public:
    Bo() {glGenBuffers(1, &id);}
    ~Bo() {glDeleteBuffers(1, &id);}
    GLuint id;
};

class Texture: public NonMvcp
{
public:
    Texture() {glGenTextures(1, &id);}
    ~Texture() {glDeleteTextures(1, &id);}
    GLuint id;
};

class Program: public NonMvcp
{
public:
    Program() {id = glCreateProgram();}
    ~Program() {glDeleteProgram(id);}
    GLint getLocation(const std::string& name);
    GLuint id;
    std::map<std::string, GLint> locations;
};

class ShaderPart: public NonMvcp
{
public:
    ShaderPart(GLenum type) {id = glCreateShader(type);}
    ~ShaderPart() {glDeleteShader(id);}
    GLuint id;
};

class Shader
{
public:
    Shader(const char* vetexSource, const char* fragmentSource, const char* id);
    void bind();
    GLint getLocation(const char* name);

private:
    Program program;
    const char* id;
    std::map<std::string, GLint> locations;
    void processLog(bool isProgram, GLuint id, GLenum flag, const char* errorMsg);
};
