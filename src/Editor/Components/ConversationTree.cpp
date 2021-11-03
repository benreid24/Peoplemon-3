#include <Editor/Components/ConversationTree.hpp>

#include <Core/Properties.hpp>
#include <queue>

namespace editor
{
namespace component
{
namespace
{
const sf::Color NodeColor(30, 60, 230);
const sf::Color TerminatorColor(150, 70, 120);
const sf::Color SelectedNodeColor(90, 120, 240);

constexpr float NodeRadius                 = 30.f;
constexpr float NodePadding                = 80.f;
constexpr float Spacing                    = NodeRadius * 2.f + NodePadding;
constexpr unsigned int CircleTriangleCount = 300;
constexpr unsigned int CircleVertexCount   = CircleTriangleCount * 3;
constexpr unsigned int ArrowTriangleCount  = 3;
constexpr unsigned int ArrowVertexCount    = ArrowTriangleCount * 3;

void renderCircle(bl::gfx::VertexBuffer& buffer, unsigned int offset, const sf::Vector2f& pos,
                  bool terminator) {
    // TODO - render at pos
}

void renderArrow(bl::gfx::VertexBuffer& buffer, unsigned int offset, const sf::Vector2f& start,
                 const sf::Vector2f& end) {
    // TODO - render arrow
}

std::string nodeToString(const core::file::Conversation::Node& node) {
    // TODO
    return "node";
}

} // namespace

using namespace bl::gui;

ConversationTree::Ptr ConversationTree::create(const ClickCb& ccb) {
    return Ptr(new ConversationTree(ccb));
}

ConversationTree::ConversationTree(const ClickCb& clickCb)
: onClick(clickCb)
, vertexBuffer(sf::PrimitiveType::Triangles, sf::VertexBuffer::Usage::Static, 0)
, selected(0)
, flashTime(0.f) {
    // TODO - click, drag, scroll, move, acquisition event handlers
}

void ConversationTree::update(const std::vector<core::file::Conversation::Node>& nodes) {
    using T = core::file::Conversation::Node::Type;

    // Clear existing render data
    renderNodes.clear();
    vertexBuffer.resize(0);
    if (nodes.empty()) return;

    // Lots of data structures
    std::queue<unsigned int> toVisit;
    std::vector<bool> visited(nodes.size(), false);
    std::vector<std::pair<unsigned int, unsigned int>> edges;
    std::vector<unsigned int> levelCounts(nodes.size(), 0);
    std::vector<unsigned int> nodeLevels(nodes.size(), 0);
    std::vector<bool> terminators(nodes.size(), false);

    // Initial setup
    edges.reserve(nodes.size() * 2);
    toVisit.push(0);
    visited[0]     = true;
    levelCounts[0] = 1;

    // BFS through tree to populate data structures
    std::vector<unsigned int> next;
    next.reserve(8);
    while (!toVisit.empty()) {
        const unsigned i                           = toVisit.front();
        const unsigned int dist                    = nodeLevels[i];
        const unsigned int newDist                 = dist + 1;
        const core::file::Conversation::Node& node = nodes[i];
        toVisit.pop();

        next.clear();
        switch (node.getType()) {
        case T::Talk:
        case T::RunScript:
        case T::SetSaveFlag:
        case T::GiveMoney:
        case T::GiveItem:
            next.push_back(node.next());
            break;
        case T::CheckInteracted:
        case T::CheckSaveFlag:
        case T::TakeMoney:
        case T::TakeItem:
            next.push_back(node.nextOnPass());
            next.push_back(node.nextOnReject());
            break;
        case T::Prompt:
            for (const auto& c : node.choices()) { next.push_back(c.second); }
            break;
        default:
            BL_LOG_ERROR << "Unknown node type: " << node.getType();
            break;
        }

        for (unsigned int jump : next) {
            if (jump >= nodes.size()) {
                terminators[i] = true;
                continue;
            }
            if (visited[jump]) continue;

            edges.emplace_back(i, jump);
            visited[jump] = true;
            levelCounts[newDist] += 1;
            nodeLevels[jump] = newDist;
            toVisit.push(jump);
        }
    }

    // Allocate space for render data
    vertexBuffer.resize(edges.size() * ArrowVertexCount + nodes.size() * CircleVertexCount);
    renderNodes.resize(nodes.size());
    unsigned int vIndex = 0;

    // Get ready to position nodes
    std::vector<sf::Vector2f> nodePositions(nodes.size());
    std::vector<float> levelLefts(levelCounts.size());
    for (unsigned int i = 0; i < levelCounts.size(); ++i) {
        const float width = static_cast<float>(levelCounts[i]) * Spacing;
        levelLefts[i]     = -width * 0.5f;
    }
    const unsigned int maxLevelCount =
        *std::max_element(std::begin(levelCounts), std::end(levelCounts));
    const float maxwidth = static_cast<float>(maxLevelCount) * Spacing;
    const float maxLeft  = -maxwidth * 0.5f;

    // Position nodes
    sf::Vector2f unreachablePos(maxLeft, -Spacing);
    for (unsigned int i = 0; i < nodes.size(); ++i) {
        if (visited[i]) {
            nodePositions[i].x = levelLefts[nodeLevels[i]];
            nodePositions[i].y = static_cast<float>(nodeLevels[i]) * Spacing;
            levelLefts[nodeLevels[i]] += Spacing;
        }
        else {
            nodePositions[i] = unreachablePos;
            unreachablePos.x += NodePadding + NodeRadius * 2.f;
        }
    }

    // Render edges
    for (const auto& edge : edges) {
        renderArrow(vertexBuffer, vIndex, nodePositions[edge.first], nodePositions[edge.second]);
        vIndex += ArrowVertexCount;
    }

    // Render circles and text
    for (unsigned int i = 0; i < nodes.size(); ++i) {
        renderCircle(vertexBuffer, vIndex, nodePositions[i], terminators[i]);
        renderNodes[i].setup(
            nodePositions[i], nodes[i], vIndex, vIndex + CircleVertexCount, terminators[i]);
        vIndex += CircleVertexCount;
    }
}

void ConversationTree::update(float dt) {
    bl::gui::Element::update(dt);

    if (selected < renderNodes.size()) {
        std::optional<sf::Color> color;
        if (flashTime > 0.f) {
            flashTime += dt;
            if (flashTime > 2.f) {
                flashTime = -flashTime + 2.f;
                color     = renderNodes[selected].terminator ? TerminatorColor : NodeColor;
            }
        }
        else {
            flashTime -= dt;
            if (flashTime < -2.f) {
                flashTime = -flashTime - 2.f;
                color     = SelectedNodeColor;
            }
        }

        if (color.has_value()) {
            const unsigned int si = renderNodes[selected].vBegin;
            const unsigned int ei = renderNodes[selected].vEnd;
            for (unsigned int i = si; i < ei; ++i) { vertexBuffer[i].color = color.value(); }
            vertexBuffer.update(si, si - ei);
        }
    }
}

sf::Vector2f ConversationTree::minimumRequisition() const { return {500.f, 500.f}; }

void ConversationTree::doRender(sf::RenderTarget& target, sf::RenderStates states,
                                const bl::gui::Renderer&) const {
    target.draw(background, states);
    const sf::View oldView = target.getView();
    target.setView(view);
    target.draw(vertexBuffer, states);
    for (const Node& node : renderNodes) { target.draw(node.label, states); }
    target.setView(oldView);
}

ConversationTree::Node::Node() {
    label.setFillColor(sf::Color::Black);
    label.setCharacterSize(12.f);
    label.setFont(core::Properties::MenuFont());
}

void ConversationTree::Node::setup(const sf::Vector2f& pos,
                                   const core::file::Conversation::Node& node, unsigned int vi,
                                   unsigned int ve, bool t) {
    center = pos;
    label.setString(nodeToString(node));
    label.setOrigin(label.getLocalBounds().width * 0.5f, label.getLocalBounds().height * 0.5f);
    label.setPosition(pos);
    vBegin     = vi;
    vEnd       = ve;
    terminator = t;
}

} // namespace component
} // namespace editor
