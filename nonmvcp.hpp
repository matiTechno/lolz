#pragma once

class NonMvcp
{
public:
    NonMvcp() = default;
    NonMvcp(const NonMvcp&) = delete;
    const NonMvcp& operator=(const NonMvcp&) = delete;
    NonMvcp(NonMvcp&&) = delete;
    NonMvcp& operator=(NonMvcp&&) = delete;
};
