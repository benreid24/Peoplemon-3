#ifndef EDITOR_COMPONENTS_TYPESELECTOR_HPP
#define EDITOR_COMPONENTS_TYPESELECTOR_HPP

#include <BLIB/Interfaces/GUI.hpp>
#include <Core/Peoplemon/Type.hpp>
#include <functional>

namespace editor
{
namespace component
{
/**
 * @brief Wrapper around ComboBox for Peoplemon types
 *
 * @ingroup Components
 *
 */
class TypeSelector : public bl::gui::ComboBox {
public:
    /// Pointer to a TypeSelector component
    using Ptr = std::shared_ptr<TypeSelector>;

    /**
     * @brief Create a new TypeSelector
     *
     */
    static Ptr create();

    /**
     * @brief Returns the type that is currently selected
     *
     */
    core::pplmn::Type currentType() const;

    /**
     * @brief Sets the currently selected type
     *
     * @param type The type to set
     */
    void setCurrentType(core::pplmn::Type type);

private:
    TypeSelector();
};

} // namespace component
} // namespace editor

#endif
