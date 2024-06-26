#ifndef CORE_RESOURCES_WALKANIMATIONS_HPP
#define CORE_RESOURCES_WALKANIMATIONS_HPP

#include <BLIB/Graphics/Animation2D/AnimationData.hpp>
#include <BLIB/Resources.hpp>
#include <BLIB/Tilemap/Direction.hpp>

namespace core
{
namespace res
{
/**
 * @brief Extension of animation data that loads and combines the animations for each movement
 *        direction into a single animation with different states
 *
 * @ingroup Resources
 */
class WalkAnimations : public bl::gfx::a2d::AnimationData {
public:
    /**
     * @brief Loads the walking animations from the given directory
     *
     * @param path The full path to the directory to load from
     * @return True if the animations were all loaded successfully, false on error
     */
    bool loadFromDirectory(const std::string& path);

    /**
     * @brief Maps the movement direction to the state index
     *
     * @param dir The direction to map to
     * @return The state index to use for the given walking direction
     */
    static std::size_t getStateFromDirection(bl::tmap::Direction dir) {
        return static_cast<std::size_t>(dir);
    }

    /**
     * @brief Returns the maximum length of any substate animation in seconds
     */
    float getMaxStateLength() const { return maxLen; }

private:
    float maxLen;
};

/**
 * @brief Specialized loader for walking animations
 *
 * @ingroup Resources
 */
struct WalkAnimationLoader : public bl::resource::LoaderBase<WalkAnimations> {
    /**
     * @brief Loads the walking animations from the given path
     * @param path The path to the set of walking animations
     * @param  Unused
     * @param  Unused
     * @param  Unused
     * @param result The animation object to populate
     * @return True on success, false on error
     */
    virtual bool load(const std::string& path, const char*, std::size_t, std::istream&,
                      WalkAnimations& result) override;
};

} // namespace res
} // namespace core

#endif
