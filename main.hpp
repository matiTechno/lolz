#pragma once

#include "glad.h"
#include <GLFW/glfw3.h>
#include "deleter.hpp"
#include <mutex>
#include "globjects.hpp"
#include <memory>
#include <vector>
#define GLM_FORCE_NO_CTOR_INIT
#include <glm/vec2.hpp>
#include <glm/vec4.hpp>
#define maxInstances 10000
#include "client.hpp"

// works for monospaced fonts
// (advance is the same for all glyphs)
struct Font
{
    struct Glyph
    {
        glm::ivec2 offset;
        glm::ivec4 texCoords;
    } glyphs[127];

    Texture texture;
    glm::ivec2 texSize;

    struct Metrics
    {
        int newlineSpace;
        int ascent;
        int descent;
        int advance;
    } metrics;
};

class Main
{
public:
    Main();
    // set proper batch states before using these functions
    // ***
    static void addText(const std::string& string, glm::vec2 pos, const glm::vec4& color);

    // Instance means rect or sprite
    static void addInstance(glm::vec2 pos, glm::vec2 size, const glm::vec4& color, const glm::ivec4& texCoords);

    // border is inside specified rect
    static void addBorder(glm::vec2 pos, glm::vec2 size, const glm::vec4& color, float width);
    // ***

    // if id == 0 texture sampling will not be used
    // disables font mode
    static void setTexture(GLuint id, glm::ivec2 size = glm::ivec2());

    static void setFontMode();

    static void setProjection(glm::vec2 start, glm::vec2 range);

    static glm::ivec2 getFbSize() {return fbSize;}

    static float getFrametime() {return frametime;}

    static Font::Metrics getFontMetrics() {return main->font->metrics;}

private:
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
        bool isFont;  // default: false
        GLuint texId; // default: 0
    };

    static Main* main;
    static GLFWwindow* window;
    static std::mutex mutex;
    static Instance instances[maxInstances];
    static std::vector<Batch> batches;
    static glm::ivec2 fbSize;
    static float frametime;

    Deleter delGlfw;
    std::unique_ptr<Font> font;
    std::unique_ptr<Texture> gnuTex;
    glm::ivec2 gnuTexSize;
    Client client;

    void run();
    static void addBatch();
    static void errorCallback(int error, const char* description);
    static void runRenderThread();

    class Renderer
    {
    public:
        Renderer();
    private:
        Shader shader;
        Vao vao;
        Bo vboVert;
        Bo vboInstances;
        void run();
    };
};

// Font::texSize[0] must be specified
void loadFont(unsigned char* ttfBuffer, int pxSize, Font* font);
