#include "globjects.hpp"
#include <vector>

Shader::Shader(const char* vertexSource, const char* fragmentSource, const char* id):
    id(id)
{
    ShaderPart vertexShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader.id, 1, &vertexSource, nullptr);
    glCompileShader(vertexShader.id);
    processLog(false, vertexShader.id, GL_COMPILE_STATUS, "vertex shader compilation error\n");

    ShaderPart fragmentShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader.id, 1, &fragmentSource, nullptr);
    glCompileShader(fragmentShader.id);
    processLog(false, fragmentShader.id, GL_COMPILE_STATUS, "fragment shader compilation error\n");

    glAttachShader(program.id, vertexShader.id);
    glAttachShader(program.id, fragmentShader.id);
    glLinkProgram(program.id);
    processLog(true, program.id, GL_LINK_STATUS, "program linking error\n");

    glDetachShader(program.id, vertexShader.id);
    glDetachShader(program.id, fragmentShader.id);

    GLint numUniforms;
    glGetProgramiv(program.id, GL_ACTIVE_UNIFORMS, &numUniforms);
    std::vector<char> uniformName(255);

    for(int i = 0; i < numUniforms; ++i)
    {
        GLint dum1;
        GLenum dum2;
        glGetActiveUniform(program.id,  i, uniformName.size(), nullptr, &dum1, &dum2, uniformName.data());
        auto location = glGetUniformLocation(program.id, uniformName.data());
        locations[uniformName.data()] = location;
    }
}

void Shader::bind() {glUseProgram(program.id);}

GLint Shader::getLocation(const char* name)
{
    auto result = locations.find(name);
    if(result == locations.end())
    {
        printf("%s: no active uniform: %s\n", this->id, name);
        throw std::exception();
    }
    return result->second;
}

void Shader::processLog(bool isProgram, GLuint id, GLenum flag, const char* errorMsg)
{
    GLint success;
    if(isProgram)
        glGetProgramiv(id, flag, &success);
    else
        glGetShaderiv(id, flag, &success);

    if(success == GL_TRUE)
        return;

    GLint length;
    if(isProgram)
        glGetProgramiv(id, GL_INFO_LOG_LENGTH, &length);
    else
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);

    std::vector<GLchar> log(length);

    if(isProgram)
        glGetProgramInfoLog(id, length, nullptr, log.data());
    else
        glGetShaderInfoLog(id, length, nullptr, log.data());

    printf("%s: %s%s\n", this->id, errorMsg, log.data());
    throw std::exception();
}
