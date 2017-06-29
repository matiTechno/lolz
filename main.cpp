#include "main.hpp"
#include <future>
#include <stdio.h>
#include <assert.h>
#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_truetype.h"
#include "stb_image.h"
#include "Inconsolata-Regular.h"
#include "gnu.h"

Main* Main::main;
GLFWwindow* Main::window;
std::mutex Main::mutex;
Main::Instance Main::instances[maxInstances];
std::vector<Main::Batch> Main::batches;
glm::ivec2 Main::fbSize;
float Main::frametime;

int main()
{
    try
    {
        Main main;
        (void)main;
    }
    catch(const std::exception& e)
    {
        printf("%s\n", e.what());
    }
    return 0;
}

Main::Main()
{
    main = this;
    glfwSetErrorCallback(errorCallback);
    if(!glfwInit())
        throw std::exception();
    delGlfw.fun = [](){glfwTerminate();};

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_MAXIMIZED, GLFW_TRUE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(640, 480, "lolz", NULL, NULL);
    if(!window)
        throw std::exception();

    batches.reserve(10);

    glfwSetCharCallback(window, Client::characterCallback);
    glfwSetKeyCallback(window, Client::keyCallback);

    glfwMakeContextCurrent(window);
    gladLoadGLLoader(GLADloadproc(glfwGetProcAddress));
    glfwSwapInterval(1);
    glfwGetFramebufferSize(window, &fbSize.x, &fbSize.y);
    glClearColor(0.f, 0.f, 0.f, 1.f);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    {
        batches.emplace_back();
        auto& batch = batches.back();
        batch.first = 0;
        batch.numInstances = 0;
        batch.texId = 0;
        batch.isFont = false;
    }
    font = std::make_unique<Font>();
    font->texSize.x = 500;
    loadFont(Inconsolata_Regular_ttf, 18, &*font);
    gnuTex = std::make_unique<Texture>();

    unsigned char* data = stbi_load_from_memory(gnu_png, gnu_png_len, &gnuTexSize.x, &gnuTexSize.y, nullptr, 4);
    if(!data)
    {
        printf("stbi_load_from_memory failed\n");
        throw std::exception();
    }
    glBindTexture(GL_TEXTURE_2D, gnuTex->id);
    glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, gnuTexSize.x, gnuTexSize.y);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, gnuTexSize.x, gnuTexSize.y, GL_RGBA, GL_UNSIGNED_BYTE, data);
    stbi_image_free(data);

    run();
}

void Main::run()
{
    glfwMakeContextCurrent(0);
    auto future = std::async(std::launch::async, runRenderThread);
    auto time1 = glfwGetTime();
    while(!glfwWindowShouldClose(window))
    {
        glfwWaitEventsTimeout(0.01666);
        {
            std::unique_lock<std::mutex> lock(mutex);
            (void)lock;

            auto time2 = glfwGetTime();
            frametime = time2 - time1;
            time1 = time2;
            {
                auto& batch = batches.front();
                batch = batches.back();
                batch.first = 0;
                batch.numInstances = 0;
                batches.erase(batches.begin() + 1, batches.end());
            }
            glfwGetFramebufferSize(window, &fbSize.x, &fbSize.y);
            {
                setProjection({0.f, 0.f}, fbSize);
                setTexture(gnuTex->id, gnuTexSize);

                glm::vec4 texCoords(0.f, 0.f, gnuTexSize);
                float fbAspect = float(fbSize.x) / fbSize.y;
                float texAspect = float(gnuTexSize.x) / gnuTexSize.y;
                if(fbAspect > texAspect)
                {
                    texCoords.w = gnuTexSize.x / fbAspect;
                    texCoords.y = (gnuTexSize.y - texCoords.w) / 2;
                }
                else if(fbAspect < texAspect)
                {
                    texCoords.z = gnuTexSize.y * fbAspect;
                    texCoords.x = (gnuTexSize.x - texCoords.z) / 2;
                }
                addInstance(glm::vec2(0.f), fbSize, {1.f, 1.f, 1.f, 0.15f}, texCoords);
                client.render();
            }
        } // unlock
    }
    future.get();
    glfwMakeContextCurrent(window);
}

void Main::runRenderThread()
{
    glfwMakeContextCurrent(window);
    {
        try
        {
            Renderer renderer;
            (void)renderer;
        }
        catch(const std::exception& e)
        {
            printf("%s\n", e.what());
            glfwSetWindowShouldClose(window, 1);
        }
    }
    glfwMakeContextCurrent(0);
}

void Main::addBatch()
{
    const auto& batch = batches.back();
    batches.emplace_back();
    auto& newBatch = batches.back();
    newBatch = batch;
    newBatch.first = batch.first + batch.numInstances;
    newBatch.numInstances = 0;
}

void Main::addInstance(glm::vec2 pos, glm::vec2 size, const glm::vec4& color, const glm::ivec4& texCoords)
{
    auto& batch = batches.back();
    batch.numInstances += 1;
    assert(batch.first + batch.numInstances <= maxInstances);
    auto& instance = instances[batch.first + batch.numInstances - 1];
    instance.pos = pos;
    instance.size = size;
    instance.color = color;
    instance.texCoords = texCoords;
}

void Main::addBorder(glm::vec2 pos, glm::vec2 size, const glm::vec4& color, float width)
{
    glm::vec4 dum;
    addInstance(pos, {size.x, width}, color, dum);
    addInstance({pos.x, pos.y + size.y - width}, {size.x, width}, color, dum);
    addInstance({pos.x, pos.y + width}, {width, size.y - 2.f * width}, color, dum);
    addInstance({pos.x + size.x - width, pos.y + width}, {width, size.y - 2.f * width}, color, dum);
}

void Main::setTexture(GLuint id, glm::ivec2 size)
{
    auto& batch = batches.back();
    if(batch.numInstances && batch.texId != id)
        addBatch();

    auto& newBatch = batches.back();
    newBatch.texId = id;
    newBatch.texSize = size;
    newBatch.isFont = false;
}

void Main::setFontMode()
{
    const auto& batch = batches.back();
    const auto& font = *main->font;
    if(batch.numInstances && batch.isFont == false)
        addBatch();

    auto& newBatch = batches.back();
    newBatch.isFont = true;
    newBatch.texId = font.texture.id;
    newBatch.texSize = font.texSize;
}

void Main::setProjection(glm::vec2 start, glm::vec2 range)
{
    const auto& batch = batches.back();
    if(batch.numInstances && (batch.projStart != start || batch.projRange != range))
        addBatch();

    auto& newBatch = batches.back();
    newBatch.projStart = start;
    newBatch.projRange = range;
}

void Main::errorCallback(int error, const char* description)
{
    (void)error;
    printf("Error: %s\n", description);
}

static const char* vertexSource =
        "#version 330\n"
        "layout(location = 0) in vec2 vertex;"
        "layout(location = 1) in vec2 pos;"
        "layout(location = 2) in vec2 size;"
        "layout(location = 3) in vec4 color;"
        "layout(location = 4) in ivec4 texCoords;"
        "uniform vec2 projStart;"
        "uniform vec2 projRange;"
        "uniform ivec2 texSize;"
        "out vec2 vTexCoord;"
        "out vec4 vColor;"
        "void main()"
        "{"
        "vTexCoord = vertex * texCoords.zw / vec2(texSize) + texCoords.xy / vec2(texSize);"
        "vColor = color;"
        "gl_Position = vec4("
        "((vec2(vertex.x, -vertex.y) * size + vec2(pos.x - projStart.x, projStart.y + projRange.y - pos.y)))"
        "* vec2(2) / projRange + vec2(-1)"
        ", 0, 1);"
        "}";

static const char* fragmentSource =
        "#version 330\n"
        "out vec4 color;"
        "uniform sampler2D sampl;"
        "uniform int isFont;"
        "uniform int useTexture;"
        "in vec2 vTexCoord;"
        "in vec4 vColor;"
        "void main()"
        "{"
        "if(isFont == 1)\n"
        "color = vec4(1, 1, 1, texture(sampl, vTexCoord).r) * vColor;"
        "else if(useTexture == 1)\n"
        "color = texture(sampl, vTexCoord) * vColor;"
        "else\n"
        "color = vColor;"
        "}";

Main::Renderer::Renderer():
    shader(vertexSource, fragmentSource, "Renderer")
{
    shader.bind();

    float rect[] = {0.f, 0.f, 1.f, 0.f, 1.f, 1.f, 1.f, 1.f, 0.f, 1.f, 0.f, 0.f};

    glBindBuffer(GL_ARRAY_BUFFER, vboVert.id);
    glBufferData(GL_ARRAY_BUFFER, sizeof(rect), rect, GL_STATIC_DRAW);

    glBindVertexArray(vao.id);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);

    glBindBuffer(GL_ARRAY_BUFFER, vboInstances.id);

    glEnableVertexAttribArray(1);
    glVertexAttribDivisor(1, 1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Instance), reinterpret_cast<const void*>(offsetof(Instance, pos)));
    glEnableVertexAttribArray(2);
    glVertexAttribDivisor(2, 1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Instance), reinterpret_cast<const void*>(offsetof(Instance, size)));
    glEnableVertexAttribArray(3);
    glVertexAttribDivisor(3, 1);
    glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(Instance), reinterpret_cast<const void*>(offsetof(Instance, color)));
    glEnableVertexAttribArray(4);
    glVertexAttribDivisor(4, 1);
    glVertexAttribIPointer(4, 4, GL_INT, sizeof(Instance), reinterpret_cast<const void*>(offsetof(Instance, texCoords)));

    run();
}

void Main::Renderer::run()
{
    while(!glfwWindowShouldClose(window))
    {
        glClear(GL_COLOR_BUFFER_BIT);
        {
            std::unique_lock<std::mutex> lock(mutex);
            (void)lock;
            glViewport(0, 0, fbSize.x, fbSize.y);

            for(auto& batch: batches)
            {
                glBufferData(GL_ARRAY_BUFFER, sizeof(Instance) * batch.numInstances,
                             instances + batch.first, GL_STREAM_DRAW);

                if(batch.texId)
                {
                    glBindTexture(GL_TEXTURE_2D, batch.texId);
                    glUniform1i(shader.getLocation("useTexture"), 1);
                    glUniform2iv(shader.getLocation("texSize"), 1, &batch.texSize.x);
                }
                else
                    glUniform1i(shader.getLocation("useTexture"), 0);

                if(batch.isFont)
                    glUniform1i(shader.getLocation("isFont"), 1);
                else
                    glUniform1i(shader.getLocation("isFont"), 0);

                glUniform2fv(shader.getLocation("projStart"), 1, &batch.projStart.x);
                glUniform2fv(shader.getLocation("projRange"), 1, &batch.projRange.x);

                glDrawArraysInstanced(GL_TRIANGLES, 0, 6, batch.numInstances);
            }
        } // unlock
        mutex.unlock();
        glfwSwapBuffers(window);
    }
}

void Main::addText(const std::string& string, glm::vec2 pos, const glm::vec4& color)
{
    const auto& font = *main->font;
    auto penPos = pos;
    for(auto c: string)
    {
        if(c == '\n')
        {
            penPos.x = pos.x;
            penPos.y += font.metrics.newlineSpace;
            continue;
        }
        if(c < 32 || c > 126)
            c = '?';
        const auto& glyph = font.glyphs[int(c)];
        addInstance(penPos + glm::vec2(glyph.offset), {glyph.texCoords.z, glyph.texCoords.w}, color, glyph.texCoords);
        penPos.x += font.metrics.advance;
    }
}

void loadFont(unsigned char* ttfBuffer, int pxSize, Font* font)
{
    stbtt_fontinfo fontInfo;
    if(stbtt_InitFont(&fontInfo, ttfBuffer, 0) == 0)
    {
        printf("stbtt_InitFont failed\n");
        throw std::exception();
    }

    int ascent, descent, lineGap;
    stbtt_GetFontVMetrics(&fontInfo, &ascent, &descent, &lineGap);
    float scale = stbtt_ScaleForMappingEmToPixels(&fontInfo, pxSize);
    font->metrics.ascent = scale * ascent;
    font->metrics.descent = scale * descent;
    font->metrics.newlineSpace = scale * (ascent - descent + lineGap);
    int advance, dum;
    stbtt_GetCodepointHMetrics(&fontInfo, 32, &advance, &dum);
    font->metrics.advance = scale * advance;
    
    unsigned char* bitmaps[127];
    int x = 0, y = 0, maxHeight = 0;
    for(int i = 32; i < 127; ++i)
    {
        int id = stbtt_FindGlyphIndex(&fontInfo, i);
        Font::Glyph& glyph = font->glyphs[i];
        int dum1, dum2;
        stbtt_GetGlyphHMetrics(&fontInfo, id, &dum1, &dum2);

        bitmaps[i] = stbtt_GetGlyphBitmap(&fontInfo, scale, scale, id, &glyph.texCoords.z, &glyph.texCoords.w,
                                          &glyph.offset.x, &glyph.offset.y);

        if(x + glyph.texCoords.z > font->texSize.x)
        {
            x = 0;
            y += maxHeight + 1;
            maxHeight = 0;
        }
        glyph.texCoords.x = x;
        glyph.texCoords.y = y;

        x += glyph.texCoords.z + 1;
        if(glyph.texCoords.w > maxHeight) maxHeight = glyph.texCoords.w;
    }
    font->texSize.y = y + maxHeight;

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glBindTexture(GL_TEXTURE_2D, font->texture.id);
    glTexStorage2D(GL_TEXTURE_2D, 1, GL_R8, font->texSize.x, font->texSize.y);
    std::vector<char> clear(font->texSize.x * font->texSize.y, 0);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, font->texSize.x, font->texSize.y, GL_RED, GL_BYTE, clear.data());
    for(int i = 32; i < 127; ++i)
    {
        Font::Glyph& glyph = font->glyphs[i];
        glTexSubImage2D(GL_TEXTURE_2D, 0, glyph.texCoords.x, glyph.texCoords.y, glyph.texCoords.z, glyph.texCoords.w,
                        GL_RED, GL_UNSIGNED_BYTE, bitmaps[i]);
        stbtt_FreeBitmap(bitmaps[i], nullptr);
    }
    glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
}
