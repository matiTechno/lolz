#include "client.hpp"
#include "main.hpp"

Client* Client::ths;

Client::Client()
{
    ths = this;
    string = "Hi.\nHello.\n"
             "Hi! I'm matiTechno. C++ developer.\n"
             "What's up?\n"
             "test test test";
}

void Client::render()
{
    auto fontMetrics = Main::getFontMetrics();
    Main::setFontMode();
    Main::addText(string, {fontMetrics.advance, fontMetrics.newlineSpace}, {0.7f, 0.7f, 0.7f, 1.f});
}

void Client::keyCallback(GLFWwindow*, int key, int scancode, int action, int mods)
{
    (void)key;
    (void)scancode;
    (void)action;
    (void)mods;
}

void Client::characterCallback(GLFWwindow*, unsigned int codepoint)
{
    ths->string.push_back(codepoint);
}
