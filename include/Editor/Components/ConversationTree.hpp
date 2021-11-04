#ifndef EDITOR_COMPONENTS_CONVERSATIONTREE_HPP
#define EDITOR_COMPONENTS_CONVERSATIONTREE_HPP

#include <BLIB/Interfaces/GUI.hpp>
#include <BLIB/Media/Graphics/VertexBuffer.hpp>
#include <Core/Files/Conversation.hpp>

namespace editor
{
namespace component
{
/**
 * @brief Special component that renders a clickable tree view of a conversation
 *
 * @ingroup Components
 *
 */
class ConversationTree : public bl::gui::Element {
public:
    using Ptr = std::shared_ptr<ConversationTree>;

    /// Called when a node is clicked
    using ClickCb = std::function<void(unsigned int)>;

    /**
     * @brief Construct a new Conversation Tree component
     *
     * @param clickCb Callback for when a node is clicked
     */
    static Ptr create(const ClickCb& clickCb);

    /**
     * @brief Updates the rendered tree with the new nodes
     *
     * @param nodes The new tree to render
     */
    void update(const std::vector<core::file::Conversation::Node>& nodes);

private:
    struct Node {
        sf::Text label;
        sf::Vector2f center;
        unsigned int vBegin;
        unsigned int vEnd;
        bool terminator;
        unsigned int index;

        Node();
        void setup(const sf::Vector2f& pos, unsigned int i,
                   const core::file::Conversation::Node& node, unsigned int vi, unsigned int ve,
                   bool terminator);
    };

    const ClickCb clickCb;
    mutable sf::View view;
    sf::RectangleShape background;
    bl::gfx::VertexBuffer vertexBuffer;
    std::vector<Node> renderNodes;
    unsigned int selected;
    float flashTime;

    ConversationTree(const ClickCb& clickCb);

    void updateBackground();
    void onDrag(const bl::gui::Event& dragEvent);
    virtual bool handleScroll(const bl::gui::Event& zoomEvent) override;
    void onClick(const bl::gui::Event& clickEvent);

    sf::Vector2f transformToTreeCoord(const sf::Vector2f& point) const;
    void setSelectedColor(const sf::Color& color);

    virtual void update(float dt) override;
    virtual sf::Vector2f minimumRequisition() const override;
    virtual void doRender(sf::RenderTarget& target, sf::RenderStates states,
                          const bl::gui::Renderer& renderer) const override;
};

} // namespace component
} // namespace editor

#endif
