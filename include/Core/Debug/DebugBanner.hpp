#ifndef CORE_DEBUG_DEBUGBANNER_HPP
#define CORE_DEBUG_DEBUGBANNER_HPP

#include <BLIB/Engine/System.hpp>
#include <BLIB/Graphics.hpp>

#ifdef PEOPLEMON_DEBUG

namespace core
{
namespace debug
{
/**
 * @brief Text banner that can be displayed. Shows text for a few seconds
 *
 * @ingroup Debug
 */
class DebugBanner : public bl::engine::System {
public:
    /**
     * @brief Creates a debug banner
     */
    DebugBanner();

    /**
     * @brief Displays the given message
     *
     * @param message The message to display
     * @param timeout How long to show it for
     */
    static void display(const std::string& message, float timeout = 2.f);

private:
    bl::engine::Engine* engine;
    bl::gfx::Text text;
    bool showing;
    float displayTime;
    float timeout;

    virtual void init(bl::engine::Engine& engine) override;
    virtual void update(std::mutex& stageMutex, float dt, float realDt, float residual,
                        float realResidual) override;
    virtual void cleanup() override;

    static DebugBanner* banner;
};

} // namespace debug
} // namespace core

#endif
#endif
