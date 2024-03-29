#ifndef CORE_SCRIPTS_LEGACYWARN_HPP
#define CORE_SCRIPTS_LEGACYWARN_HPP

#include <BLIB/Logging.hpp>
#include <BLIB/Util/FileUtil.hpp>
#include <Core/Properties.hpp>
#include <string>

namespace core
{
namespace script
{
/**
 * @brief Helper struct to warn when legacy scripts are attempted to load
 *
 */
struct LegacyWarn {
    /**
     * @brief In debug mode logs a warning if the given script is detected to be legacy
     *
     * @param script The script to check. Only works on files
     */
    static void warn(const std::string& script);
};

inline void LegacyWarn::warn(const std::string& script) {
#ifdef PEOPLEMON_DEBUG
    if (bl::util::FileUtil::getExtension(script) == "psc") {
        BL_LOG_WARN << "Tried to load legacy script: "
                    << bl::util::FileUtil::joinPath(Properties::ScriptPath(), script);
    }
#else
    (void)script;
#endif
}

} // namespace script
} // namespace core

#endif
