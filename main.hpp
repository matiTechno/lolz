#pragma once

#include "glad.h"
#include <GLFW/glfw3.h>
#include "deleter.hpp"
#define maxInstances 10000
#include <mutex>
#include "globjects.hpp"
#include <memory>
#include <vector>
#define GLM_FORCE_NO_CTOR_INIT
#include <glm/vec2.hpp>
#include <glm/vec4.hpp>

// works for monospaced fonts
// (advance is the same for all glyphs)
struct Font
{
    struct Glyph
    {
        glm::ivec2 offset;
        glm::ivec4 texCoords;
    } glyphs[127];

    int newlineSpace;
    int ascent;
    int descent;
    Texture texture;
    glm::ivec2 texSize;
    int advance;
};

struct Instance
{
    glm::vec2 pos;
    glm::vec2 size;
    glm::vec4 color;
    glm::ivec4 texCoords;
};

struct Batch
{
    int first;
    int numInstances;
    glm::ivec2 texSize;
    glm::vec2 projStart;
    glm::vec2 projRange;
    bool isFont;     // default: false
    GLuint texId;    // default: 0
};

class Main
{
public:
    Main();
    // set proper batch states before using these functions
    // ***
    void addText(const std::string& string, glm::vec2 pos, const glm::vec4& color);
    void addInstance(glm::vec2 pos, glm::vec2 size, const glm::vec4& color, const glm::ivec4& texCoords);
    // ***
    // border is inside specified rect
    void addBorder(glm::vec2 pos, glm::vec2 size, const glm::vec4& color, float width);

    // if id == 0 texture sampling will not be used
    void setTexture(GLuint id, glm::ivec2 size = glm::ivec2());

    // implicitly calls setTexture with Font::texture
    void setFont(bool on);

    void setProjection(glm::vec2 start, glm::vec2 range);

private:
    Deleter delGlfw;
    std::unique_ptr<Font> font;
    std::unique_ptr<Texture> gnuTex;
    glm::ivec2 gnuTexSize;

public:
    static Main* main;
    GLFWwindow* window;
    Instance instances[maxInstances];
    std::vector<Batch> batches;
    glm::ivec2 fbSize;
    float frametime;
    std::mutex mutex;

private:
    void run();
    static void errorCallback(int error, const char* description);
    static void keyCallback(GLFWwindow*, int key, int scancode, int action, int mods);
    static void characterCallback(GLFWwindow*, unsigned int codepoint);
    static void runRenderThread();
    void addBatch();

    std::string testStr;
};

// Font::texSize[0] must be specified
void loadFont(unsigned char* ttfBuffer, int pxSize, Font* font);

class Renderer
{
public:
    Renderer();

private:
    void run();
    Shader shader;
    Vao vao;
    Bo vboRect;
    Bo vboInstances;
};
