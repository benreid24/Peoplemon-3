#ifndef EDITOR_COMPONENTS_RENDER_EDITMAPCOMPONENT_HPP
#define EDITOR_COMPONENTS_RENDER_EDITMAPCOMPONENT_HPP

#include <BLIB/Graphics/Sprite.hpp>
#include <BLIB/Interfaces/GUI/Renderer/Component.hpp>
#include <BLIB/Render.hpp>

namespace editor
{
namespace component
{
namespace rdr
{
class EditMapComponent : public bl::gui::rdr::Component {
public:
    EditMapComponent();

    virtual ~EditMapComponent() = default;

private:
    bl::engine::Engine* enginePtr;
    bl::rc::vk::RenderTexture::Handle renderTexture;
    bl::gfx::Sprite sprite;

    virtual void setVisible(bool visible) override;
    virtual void onElementUpdated() override;
    virtual void onRenderSettingChange() override;
    virtual bl::ecs::Entity getEntity() const override;
    virtual void doCreate(bl::engine::Engine& engine, bl::gui::rdr::Renderer& renderer) override;
    virtual void doSceneAdd(bl::rc::Overlay* overlay) override;
    virtual void doSceneRemove() override;
    virtual void handleAcquisition() override;
    virtual void handleMove() override;
};

} // namespace rdr
} // namespace component
} // namespace editor

#endif
