#pragma once

#include "glad.h"
#include <GLFW/glfw3.h>
#include "deleter.hpp"
#define maxInstances 10000
#include <mutex>
#include "globjects.hpp"
#include <memory>
#include <vector>
#include "vec.hpp"

// works for monospaced fonts
// (advance is the same for all glyphs)
struct Font
{
    struct Glyph
    {
        ivec2 offset;
        ivec4 texCoords;
    } glyphs[127];

    int newlineSpace;
    int ascent;
    int descent;
    Texture texture;
    ivec2 texSize;
    int advance;
};

struct Instance
{
    vec2 pos;
    vec2 size;
    vec4 color;
    ivec4 texCoords;
};

struct Batch
{
    int first;
    int numInstances;
    GLuint texId;
    ivec2 texSize;
    bool isFont;
    vec2 projStart;
    vec2 projRange;
};

class Main
{
public:
    void addText(const std::string& string, vec2 pos, const vec4& color);
    void addInstance(vec2 pos, vec2 size, const vec4& color, const ivec4& texCoords);
    // if id == 0 texture sampling will not be used
    void setTexture(GLuint id, ivec2 size = ivec2());
    void isFont(bool is);
    void setProjection(vec2 start, vec2 range);

    Main();
    static Main* main;
    Deleter delGlfw;
    GLFWwindow* window;
    Instance instances[maxInstances];
    std::vector<Batch> batches;
    ivec2 fbSize;
    float frametime;
    std::mutex mutex;
    std::unique_ptr<Font> font;
    std::unique_ptr<Texture> gnuTex;
    ivec2 gnuTexSize;

private:
    void run();
    static void errorCallback(int error, const char* description);
    static void keyCallback(GLFWwindow*, int key, int scancode, int action, int mods);
    static void characterCallback(GLFWwindow*, unsigned int codepoint);
    static void runRenderThread();
    std::string testStr;
    void addBatch();
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
