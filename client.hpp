#pragma once

struct GLFWwindow;
#include <string>

class Client
{
public:
    Client();
    void render();
    static void keyCallback(GLFWwindow*, int key, int scancode, int action, int mods);
    static void characterCallback(GLFWwindow*, unsigned int codepoint);

private:
    static Client* ths;
    std::string string;
};
