#include <Core/Resources/RunWalkAnimations.hpp>

#include <BLIB/Util/FileUtil.hpp>

namespace core
{
namespace res
{
bool RunWalkAnimations::loadFromDirectory(const std::string& path) {
    using AnimationManager = bl::resource::ResourceManager<bl::gfx::a2d::AnimationData>;
    using FileUtil         = bl::util::FileUtil;

    if (!AnimationManager::initializeExisting(FileUtil::joinPath(path, "Walk/up.anim"), *this)) {
        return false;
    }

    prepareForStates(8);
    bl::gfx::a2d::AnimationData temp;
    if (!AnimationManager::initializeExisting(FileUtil::joinPath(path, "Walk/right.anim"), temp)) {
        return false;
    }
    addState(1, temp);

    if (!AnimationManager::initializeExisting(FileUtil::joinPath(path, "Walk/down.anim"), temp)) {
        return false;
    }
    addState(2, temp);

    if (!AnimationManager::initializeExisting(FileUtil::joinPath(path, "Walk/left.anim"), temp)) {
        return false;
    }
    addState(3, temp);

    if (!AnimationManager::initializeExisting(FileUtil::joinPath(path, "Run/up.anim"), temp)) {
        return false;
    }
    addState(4, temp);

    if (!AnimationManager::initializeExisting(FileUtil::joinPath(path, "Run/right.anim"), temp)) {
        return false;
    }
    addState(5, temp);

    if (!AnimationManager::initializeExisting(FileUtil::joinPath(path, "Run/down.anim"), temp)) {
        return false;
    }
    addState(6, temp);

    if (!AnimationManager::initializeExisting(FileUtil::joinPath(path, "Run/left.anim"), temp)) {
        return false;
    }
    addState(7, temp);

    return true;
}

bool RunWalkAnimationLoader::load(const std::string& path, const char*, std::size_t, std::istream&,
                                  RunWalkAnimations& result) {
    return result.loadFromDirectory(path);
}

} // namespace res
} // namespace core
