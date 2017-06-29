#pragma once

#include "nonmvcp.hpp"

class Deleter: public NonMvcp
{
public:
    void(*fun)(void) = nullptr;
    ~Deleter() {if(fun) fun();}
};
