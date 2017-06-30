#pragma once

struct GLFWwindow;
#include <string>

class Client
{
public:
    Client();
    void render();
    void keyCallback(GLFWwindow*, int key, int scancode, int action, int mods);
    void characterCallback(GLFWwindow*, unsigned int codepoint);

private:
    std::string string;
    float accumulator = 0.f;
    bool visible = true;
};
