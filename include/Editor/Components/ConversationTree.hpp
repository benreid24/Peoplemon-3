#ifndef EDITOR_COMPONENTS_CONVERSATIONTREE_HPP
#define EDITOR_COMPONENTS_CONVERSATIONTREE_HPP

#include <BLIB/Graphics.hpp>
#include <BLIB/Interfaces/GUI.hpp>
#include <Core/Files/Conversation.hpp>
#include <Editor/Components/Render/ConversationTreeComponent.hpp>

namespace editor
{
namespace component
{
/**
 * @brief Special component that renders a clickable tree view of a conversation
 *
 * @ingroup UIComponents
 */
class ConversationTree : public bl::gui::Element {
public:
    using Ptr = std::shared_ptr<ConversationTree>;

    static constexpr float NodeRadius        = 60.f;
    static constexpr float NodeRadiusSquared = NodeRadius * NodeRadius;
    static constexpr float NodePadding       = 100.f;
    static constexpr float Spacing           = NodeRadius * 2.f + NodePadding;

    struct Node {
        glm::vec2 center;
        bool terminator;
        unsigned int index; // in source
        std::string label;

        Node() = default;
        void setup(const sf::Vector2f& pos, const core::file::Conversation::Node& src,
                   unsigned int i, bool terminator);

        friend class ConversationTree;
    };

    struct Edge {
        unsigned int from;
        unsigned int to;
    };

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

    /**
     * @brief Set the selected node index
     *
     * @param i Index of the selected node
     */
    void setSelected(unsigned int i);

    /**
     * @brief Used by the renderer component
     */
    const std::vector<Node>& getNodes() const { return renderNodes; }

    /**
     * @brief Used by the renderer component
     */
    const std::vector<Edge>& getEdges() const { return edges; }

    /**
     * @brief Used by the renderer component
     */
    unsigned int getSelectedNode() const { return selected; }

    /**
     * @brief Used by the renderer component
     */
    unsigned int getTreeVersion() const { return treeVersion; }

    /**
     * @brief Used by the renderer component
     */
    const glm::vec2& getCamCenter() const { return camCenter; }

    /**
     * @brief Used by the renderer component
     */
    float getCamZoom() const { return camZoom; }

    /**
     * @brief Used by the renderer component
     */
    bool shouldHighlightSelectedNode() const { return highlightSelected; }

private:
    const ClickCb clickCb;
    std::vector<Node> renderNodes;
    std::vector<Edge> edges;
    unsigned int selected;
    float flashTime;
    unsigned int treeVersion;
    glm::vec2 camCenter;
    float camZoom;
    bool highlightSelected;

    ConversationTree(const ClickCb& clickCb);

    void onDrag(const bl::gui::Event& dragEvent);
    virtual bool handleScroll(const bl::gui::Event& zoomEvent) override;
    void onClick(const bl::gui::Event& clickEvent);
    void centerView();
    sf::FloatRect getVirtualBounds() const;

    sf::Vector2f transformToTreeCoord(const sf::Vector2f& point) const;

    virtual void update(float dt) override;
    virtual sf::Vector2f minimumRequisition() const override;
    virtual bl::gui::rdr::Component* doPrepareRender(bl::gui::rdr::Renderer& renderer) override;
};

} // namespace component
} // namespace editor

#endif
