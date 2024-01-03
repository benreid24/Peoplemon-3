#include <Core/Resources/WalkAnimations.hpp>

#include <BLIB/Util/FileUtil.hpp>

namespace core
{
namespace res
{
bool WalkAnimations::loadFromDirectory(const std::string& path) {
    using AnimationManager = bl::resource::ResourceManager<bl::gfx::a2d::AnimationData>;
    using FileUtil         = bl::util::FileUtil;

    if (!AnimationManager::initializeExisting(FileUtil::joinPath(path, "up.anim"), *this)) {
        return false;
    }
    maxLen = getLength();

    prepareForStates(4);
    bl::gfx::a2d::AnimationData temp;
    if (!AnimationManager::initializeExisting(FileUtil::joinPath(path, "right.anim"), temp)) {
        return false;
    }
    maxLen = std::max(maxLen, temp.getLength());
    addState(1, temp);

    if (!AnimationManager::initializeExisting(FileUtil::joinPath(path, "down.anim"), temp)) {
        return false;
    }
    maxLen = std::max(maxLen, temp.getLength());
    addState(2, temp);

    if (!AnimationManager::initializeExisting(FileUtil::joinPath(path, "left.anim"), temp)) {
        return false;
    }
    maxLen = std::max(maxLen, temp.getLength());
    addState(3, temp);

    return true;
}

bool WalkAnimationLoader::load(const std::string& path, const char*, std::size_t, std::istream&,
                               WalkAnimations& result) {
    return result.loadFromDirectory(path);
}

} // namespace res
} // namespace core
