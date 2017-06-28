#pragma once

#include "nonmvcp.hpp"
#include <functional>

class Deleter: public NonMvcp
{
public:
    std::function<void()> fun = nullptr;
    ~Deleter() {if(fun) fun();}
};
