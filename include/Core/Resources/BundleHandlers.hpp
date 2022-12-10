#ifndef CORE_RESOURCES_BUNDLEHANDLERS_HPP
#define CORE_RESOURCES_BUNDLEHANDLERS_HPP

#include <BLIB/Logging.hpp>
#include <BLIB/Resources.hpp>

namespace core
{
namespace res
{
/**
 * @brief Generic handler for peoplemon files when being bundled. Performs format conversion and
 *        registers dependency files. Requires dev loaders to be installed
 *
 * @tparam T The type of Peoplemon file to bundle
 * @ingroup CoreResources
 */
template<typename T>
struct PeoplemonBundleHandler : public bl::resource::bundle::FileHandler {
    virtual bool processFile(const std::string& path, std::ostream& output,
                             bl::resource::bundle::FileHandlerContext& context) override {
        T obj;
        if (!bl::resource::ResourceManager<T>::initializeExisting(path, obj)) {
            BL_LOG_ERROR << "Failed to process: " << path;
            return false;
        }
        bl::serial::StreamOutputBuffer wrapper(output);
        bl::serial::binary::OutputStream os(wrapper);
        if (!obj.saveBundle(os, context)) {
            BL_LOG_ERROR << "Failed to serialize '" << path << "' into bundle";
            return false;
        }
        return true;
    }
};

} // namespace res
} // namespace core

#endif
