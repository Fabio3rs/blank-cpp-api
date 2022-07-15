#include "Instances.hpp"

auto Instances::singleton() -> Instances &
{
    static Instances inst;
    return inst;
}

Instances::Instances(/* args */) = default;

Instances::~Instances() = default;
