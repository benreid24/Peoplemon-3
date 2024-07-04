#include <Editor/Components/ConversationTree.hpp>

#include <Core/Properties.hpp>
#include <Editor/Components/Render/ConversationTreeComponent.hpp>
#include <cmath>
#include <queue>

namespace editor
{
namespace component
{
namespace
{
constexpr float FlashPeriod = 0.7f;

// void renderCircle(bl::gfx::VertexBuffer& buffer, unsigned int offset, const sf::Vector2f& pos,
//                   bool terminator) {
//     const sf::Color col = terminator ? TerminatorColor : NodeColor;
//     for (unsigned int j = 0; j < CircleTriangleCount; ++j) {
//         unsigned int i = j * 3;
//         const float sa = static_cast<float>(j) * RadsPerTriangle;
//         const float ea = sa + RadsPerTriangle;
//
//         buffer[offset + i].position = pos;
//         buffer[offset + i].color    = col;
//
//         buffer[offset + i + 1].position.x = std::cos(sa) * NodeRadius + pos.x;
//         buffer[offset + i + 1].position.y = std::sin(sa) * NodeRadius + pos.y;
//         buffer[offset + i + 1].color      = col;
//
//         buffer[offset + i + 2].position.x = std::cos(ea) * NodeRadius + pos.x;
//         buffer[offset + i + 2].position.y = std::sin(ea) * NodeRadius + pos.y;
//         buffer[offset + i + 2].color      = col;
//     }
// }
//
// void renderArrow(bl::gfx::VertexBuffer& buffer, unsigned int offset, const sf::Vector2f& start,
//                  const sf::Vector2f& end) {
//     const float forwardAngle = std::atan2(end.y - start.y, end.x - start.x);
//     const float reverseAngle = std::atan2(start.y - end.y, start.x - end.x);
//     const float perpAngle    = (forwardAngle + reverseAngle) * 0.5f;
//
//     const sf::Vector2f from(start.x + NodeRadius * std::cos(forwardAngle),
//                             start.y + NodeRadius * std::sin(forwardAngle));
//     const sf::Vector2f to(end.x + (NodeRadius + ArrowDepth) * std::cos(reverseAngle),
//                           end.y + (NodeRadius + ArrowDepth) * std::sin(reverseAngle));
//
//     const sf::Vector2f sCorner1(from.x + ArrowWidth * std::cos(perpAngle),
//                                 from.y + ArrowWidth * std::sin(perpAngle));
//     const sf::Vector2f sCorner2(from.x - ArrowWidth * std::cos(perpAngle),
//                                 from.y - ArrowWidth * std::sin(perpAngle));
//     const sf::Vector2f eCorner1(to.x + ArrowWidth * std::cos(perpAngle),
//                                 to.y + ArrowWidth * std::sin(perpAngle));
//     const sf::Vector2f eCorner2(to.x - ArrowWidth * std::cos(perpAngle),
//                                 to.y - ArrowWidth * std::sin(perpAngle));
//
//     // Rect triangle 1
//     buffer[offset].position     = sCorner1;
//     buffer[offset + 1].position = sCorner2;
//     buffer[offset + 2].position = eCorner2;
//
//     // Rect triangle 2
//     buffer[offset + 3].position = eCorner1;
//     buffer[offset + 4].position = eCorner2;
//     buffer[offset + 5].position = sCorner1;
//
//     // Arrow
//     buffer[offset + 6].position = {end.x + NodeRadius * std::cos(reverseAngle),
//                                    end.y + NodeRadius * std::sin(reverseAngle)};
//     buffer[offset + 7].position = {to.x + ArrowDepth * std::cos(perpAngle),
//                                    to.y + ArrowDepth * std::sin(perpAngle)};
//     buffer[offset + 8].position = {to.x - ArrowDepth * std::cos(perpAngle),
//                                    to.y - ArrowDepth * std::sin(perpAngle)};
//
//     // Colors
//     const sf::Color color = end.y > start.y ? DownArrowColor : UpArrowColor;
//     for (unsigned int i = 0; i < ArrowVertexCount; ++i) { buffer[offset + i].color = color; }
// }

std::string nodeToString(unsigned int i, const core::file::Conversation::Node& node) {
    std::stringstream ss;
    const std::string n   = "#" + std::to_string(i);
    const std::string t   = core::file::Conversation::Node::typeToString(node.getType());
    const unsigned int sc = t.size() > n.size() ? (t.size() - n.size()) / 2 : 0;
    for (unsigned int i = 0; i < sc; ++i) { ss << " "; }
    ss << n << "\n" << t;
    return ss.str();
}

} // namespace

using namespace bl::gui;

ConversationTree::Ptr ConversationTree::create(const ClickCb& ccb) {
    return Ptr(new ConversationTree(ccb));
}

ConversationTree::ConversationTree(const ClickCb& ccb)
: clickCb(ccb)
, selected(0)
, flashTime(0.f)
, treeVersion(0) {
    setTooltip(
        "Click and drag to move. Scroll to zoom. Click node to edit. Right click to center view.");
    getSignal(Event::Dragged)
        .willAlwaysCall(std::bind(&ConversationTree::onDrag, this, std::placeholders::_1));
    getSignal(Event::LeftClicked)
        .willAlwaysCall(std::bind(&ConversationTree::onClick, this, std::placeholders::_1));
    getSignal(Event::RightClicked).willAlwaysCall(std::bind(&ConversationTree::centerView, this));
}

bool ConversationTree::handleScroll(const bl::gui::Event& e) {
    if (getAcquisition().contains(e.mousePosition())) {
        camZoom *= 1.f + e.scrollDelta() * -0.1f; // TODO - reciprocal?
        if (getComponent()) { getComponent()->onElementUpdated(); }
        return true;
    }
    return false;
}

void ConversationTree::onClick(const bl::gui::Event& e) {
    const sf::Vector2f mpos = transformToTreeCoord(e.mousePosition());
    for (unsigned int i = 0; i < renderNodes.size(); ++i) {
        const Node& n     = renderNodes[i];
        const float dx    = n.center.x - mpos.x;
        const float dy    = n.center.y - mpos.y;
        const float dsqrd = dx * dx + dy * dy;
        if (dsqrd <= NodeRadiusSquared) {
            setSelected(i);
            clickCb(n.index);
            break;
        }
    }
}

void ConversationTree::setSelected(unsigned int i) {
    selected          = i;
    camCenter         = renderNodes[i].center;
    flashTime         = 0.f;
    highlightSelected = true;
    if (getComponent()) { getComponent()->onElementUpdated(); }
}

void ConversationTree::onDrag(const bl::gui::Event& e) {
    const sf::Vector2f dragStart = transformToTreeCoord(e.dragStart());
    const sf::Vector2f mpos      = transformToTreeCoord(e.mousePosition());
    const sf::Vector2f diff(dragStart - mpos);
    camCenter += glm::vec2(diff.x, diff.y);
    if (getComponent()) { getComponent()->onElementUpdated(); }
}

sf::FloatRect ConversationTree::getVirtualBounds() const {
    sf::FloatRect bounds;
    for (const auto& node : renderNodes) {
        bounds.left   = std::min(bounds.left, node.center.x - NodeRadius);
        bounds.top    = std::min(bounds.top, node.center.y - NodeRadius);
        bounds.width  = std::max(bounds.width, node.center.x + NodeRadius);
        bounds.height = std::max(bounds.height, node.center.y + NodeRadius);
    }
    return bounds;
}

void ConversationTree::centerView() {
    const sf::FloatRect bounds = getVirtualBounds();
    camCenter = glm::vec2((bounds.left + bounds.width) * 0.5f, (bounds.top + bounds.height) * 0.5f);
    if (getComponent()) { getComponent()->onElementUpdated(); }
}

sf::Vector2f ConversationTree::transformToTreeCoord(const sf::Vector2f& p) const {
    // TODO - correct?
    sf::Transform transform;
    transform.translate(-getPosition() -
                        sf::Vector2f(getAcquisition().width, getAcquisition().height) * 0.5f);
    transform.scale(1.f / camZoom, 1.f / camZoom);
    transform.translate(camCenter.x, camCenter.y);

    return transform.transformPoint(p);
}

void ConversationTree::update(const std::vector<core::file::Conversation::Node>& nodes) {
    // Clear existing render data
    renderNodes.clear();
    // vertexBuffer.resize(0);
    if (nodes.empty()) return;

    // Lots of data structures
    std::queue<unsigned int> toVisit;
    std::vector<bool> visited(nodes.size(), false);
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

        core::file::Conversation::getNextJumps(node, next);

        for (unsigned int jump : next) {
            if (jump >= nodes.size()) {
                terminators[i] = true;
                continue;
            }
            edges.emplace_back(i, jump);
            if (visited[jump]) continue;

            visited[jump] = true;
            levelCounts[newDist] += 1;
            nodeLevels[jump] = newDist;
            toVisit.push(jump);
        }
    }

    // Allocate space for render data
    renderNodes.resize(nodes.size());

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

        renderNodes[i].setup(nodePositions[i], nodes[i], i, terminators[i]);
    }

    // Reset bounds
    centerView();

    // Update render component
    ++treeVersion;
    if (getComponent()) { getComponent()->onElementUpdated(); }
}

void ConversationTree::update(float dt) {
    bl::gui::Element::update(dt);

    if (selected < renderNodes.size()) {
        std::optional<sf::Color> color;
        if (flashTime > 0.f) {
            flashTime += dt;
            if (flashTime > FlashPeriod) {
                flashTime         = -flashTime + FlashPeriod;
                highlightSelected = false;
                if (getComponent()) { getComponent()->onElementUpdated(); }
            }
        }
        else {
            flashTime -= dt;
            if (flashTime < -FlashPeriod) {
                flashTime         = -flashTime - FlashPeriod;
                highlightSelected = true;
                if (getComponent()) { getComponent()->onElementUpdated(); }
            }
        }
    }
}

sf::Vector2f ConversationTree::minimumRequisition() const { return {500.f, 500.f}; }

bl::gui::rdr::Component* ConversationTree::doPrepareRender(bl::gui::rdr::Renderer& renderer) {
    return renderer.createComponent<ConversationTree>(*this);
}

// void ConversationTree::doRender(sf::RenderTarget& target, sf::RenderStates states,
//                                 const bl::gui::Renderer&) const {
//     if (renderNodes.empty()) return;
//
//     const float w = static_cast<float>(target.getSize().x);
//     const float h = static_cast<float>(target.getSize().y);
//     view.setViewport({getPosition().x / w,
//                       getPosition().y / h,
//                       getAcquisition().width / w,
//                       getAcquisition().height / h});
//
//     target.draw(background, states);
//     const sf::View oldView = target.getView();
//     target.setView(view);
//     target.draw(vertexBuffer, states);
//     sf::CircleShape circle;
//     circle.setFillColor(sf::Color(255, 0, 0, 40));
//     circle.setRadius(NodeRadius);
//     circle.setOrigin(NodeRadius, NodeRadius);
//     for (const Node& node : renderNodes) {
//         target.draw(node.label, states);
//         circle.setPosition(node.center);
//         target.draw(circle);
//     }
//     target.setView(oldView);
// }

void ConversationTree::Node::setup(const sf::Vector2f& pos,
                                   const core::file::Conversation::Node& src, unsigned int i,
                                   bool t) {
    center     = {pos.x, pos.y};
    terminator = t;
    index      = i;
    label      = nodeToString(i, src);
}

} // namespace component
} // namespace editor
