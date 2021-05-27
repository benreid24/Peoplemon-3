#ifndef TESTS_SYSTEMSHELPER_HPP
#define TESTS_SYSTEMSHELPER_HPP

#include <Core/Systems/Systems.hpp>

namespace tests
{
class SystemsHelper {
public:
    SystemsHelper()
    : engine(bl::engine::Settings().withCreateWindow(false))
    , sys(engine) {}

    core::system::Systems& systems() { return sys; }

private:
    bl::engine::Engine engine;
    core::system::Systems sys;
};

} // namespace tests

#endif
