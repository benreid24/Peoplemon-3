#ifndef PROPERTIES_HPP
#define PROPERTIES_HPP

#include <BLIB/Util/NonCopyable.hpp>
#include <string>

/**
 * @brief Common core application properties here. We may want to load these from a file or they may
 *        simply be constants. Some properties may be loaded into the engine configuration store
 *
 */
class Properties : private bl::util::NonCopyable {
public:
    static const std::string SpritesheetPath;
    static const std::string MusicPath;

    static bool load();

private:
    Properties() = default;
};

#endif
