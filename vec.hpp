#pragma once

struct ivec2;
struct vec2
{
    vec2() = default;
    vec2(const ivec2& rhs);
    vec2(float x, float y): x(x), y(y) {}
    vec2(float x): x(x), y(x) {}
    vec2& operator=(float) = delete;
    bool operator==(vec2 rhs) const {return x == rhs.x && y == rhs.y;}
    bool operator!=(vec2 rhs) const {return !(*this == rhs);}
    float x, y;
};

struct ivec2
{
    ivec2() = default;
    ivec2(vec2 rhs): x(rhs.x), y(rhs.y) {}
    ivec2(int x, int y): x(x), y(y) {}
    ivec2(int x): x(x), y(x) {}
    ivec2& operator=(int) = delete;
    bool operator==(ivec2 rhs) const {return x == rhs.x && y == rhs.y;}
    bool operator!=(ivec2 rhs) const {return !(*this == rhs);}
    int x, y;
};

vec2::vec2(const ivec2& rhs): x(rhs.x), y(rhs.y) {}

struct ivec4;
struct vec4
{
    vec4() = default;
    vec4(const ivec4& rhs);
    vec4(float x, float y, float z, float w): x(x), y(y), z(z), w(w) {}
    vec4(float x): x(x), y(x), z(x), w(z) {}
    vec4& operator=(float) = delete;
    bool operator==(vec4 rhs) const {return x == rhs.x && y == rhs.y && z == rhs.z && w == rhs.w;}
    bool operator!=(vec4 rhs) const {return !(*this == rhs);}
    float x, y, z, w;
};

struct ivec4
{
    ivec4() = default;
    ivec4(vec4 rhs): x(rhs.x), y(rhs.y), z(rhs.z), w(rhs.w) {}
    ivec4(int x, int y, int z, int w): x(x), y(y), z(z), w(w) {}
    ivec4(int x): x(x), y(x), z(x), w(z) {}
    ivec4& operator=(int) = delete;
    bool operator==(ivec4 rhs) const {return x == rhs.x && y == rhs.y && z == rhs.z && w == rhs.w;}
    bool operator!=(ivec4 rhs) const {return !(*this == rhs);}
    int x, y, z, w;
};

vec4::vec4(const ivec4& rhs): x(rhs.x), y(rhs.y), z(rhs.z), w(rhs.w) {}
