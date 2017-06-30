#include "client.hpp"
#include "main.hpp"

Client::Client()
{
    string = "Hi.\nHello.\n"
             "Hi! I'm matiTechno. C++ developer.\n"
             "What's up?\n"
             "test test test";
}

void Client::render()
{
    accumulator += Main::getFrametime();
    if(accumulator > 0.5f)
    {
        accumulator = 0.f;
        visible = !visible;
    }
    auto fontMetrics = Main::getFontMetrics();
    Main::setFontMode();
    glm::vec2 pos{fontMetrics.advance, fontMetrics.newlineSpace};
    glm::vec4 color{0.7f, 0.7f, 0.7f, 1.f};
    Main::addText(string, pos, color);

    if(visible)
    {
        auto offset = Main::getPosOffset(string, string.size());
        Main::setTexture(0);
        Main::addInstance(pos + offset + glm::vec2(0.f, -fontMetrics.ascent),
                          glm::vec2(fontMetrics.advance, fontMetrics.ascent - fontMetrics.descent), color, {});
    }
}

void Client::keyCallback(GLFWwindow*, int key, int scancode, int action, int mods)
{
    accumulator = 0.f;
    visible = true;
    (void)scancode;
    (void)mods;

    if(action == GLFW_RELEASE)
        return;

    switch(key)
    {
    case GLFW_KEY_ENTER: string.push_back('\n'); break;
    case GLFW_KEY_BACKSPACE: if(string.size()) string.pop_back(); break;
    }
}

void Client::characterCallback(GLFWwindow*, unsigned int codepoint)
{
    string.push_back(codepoint);
}
