#ifndef REGISTRY_HPP
#define REGISTRY_HPP

#include "strategy.hpp"
#include <string>
#include <array>

const std::array<std::string, 2> stratNames = {
    MovingAvgStrategy::name,
    WeinsteinStrategy::name,
};

#endif // REGISTRY_HPP