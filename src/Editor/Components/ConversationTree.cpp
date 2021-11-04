#include <Editor/Components/ConversationTree.hpp>

#include <Core/Properties.hpp>
#include <cmath>
#include <queue>

namespace editor
{
namespace component
{
namespace
{
const sf::Color NodeColor(80, 90, 230);
const sf::Color TerminatorColor(230, 70, 120);
const sf::Color SelectedNodeColor(190, 190, 70);
const sf::Color DownArrowColor(30, 220, 65);
const sf::Color UpArrowColor(230, 90, 40);

constexpr float NodeRadius                 = 40.f;
constexpr float NodeRadiusSquared          = NodeRadius * NodeRadius;
constexpr float NodePadding                = 100.f;
constexpr float Spacing                    = NodeRadius * 2.f + NodePadding;
constexpr unsigned int CircleTriangleCount = 300;
constexpr unsigned int CircleVertexCount   = CircleTriangleCount * 3;
constexpr float RadsPerTriangle = 2.f * 3.1415926f / static_cast<float>(CircleTriangleCount);
constexpr unsigned int ArrowTriangleCount = 3;
constexpr unsigned int ArrowVertexCount   = ArrowTriangleCount * 3;
constexpr float ArrowWidth                = 5.f;
constexpr float ArrowDepth                = 18.f;
constexpr float FlashPeriod               = 1.f;

void renderCircle(bl::gfx::VertexBuffer& buffer, unsigned int offset, const sf::Vector2f& pos,
                  bool terminator) {
    const sf::Color col = terminator ? TerminatorColor : NodeColor;
    for (unsigned int j = 0; j < CircleTriangleCount; ++j) {
        unsigned int i = j * 3;
        const float sa = static_cast<float>(j) * RadsPerTriangle;
        const float ea = sa + RadsPerTriangle;

        buffer[offset + i].position = pos;
        buffer[offset + i].color    = col;

        buffer[offset + i + 1].position.x = std::cos(sa) * NodeRadius + pos.x;
        buffer[offset + i + 1].position.y = std::sin(sa) * NodeRadius + pos.y;
        buffer[offset + i + 1].color      = col;

        buffer[offset + i + 2].position.x = std::cos(ea) * NodeRadius + pos.x;
        buffer[offset + i + 2].position.y = std::sin(ea) * NodeRadius + pos.y;
        buffer[offset + i + 2].color      = col;
    }
}

void renderArrow(bl::gfx::VertexBuffer& buffer, unsigned int offset, const sf::Vector2f& start,
                 const sf::Vector2f& end) {
    const float forwardAngle = std::atan2(end.y - start.y, end.x - start.x);
    const float reverseAngle = std::atan2(start.y - end.y, start.x - end.x);
    const float perpAngle    = (forwardAngle + reverseAngle) * 0.5f;

    const sf::Vector2f from(start.x + NodeRadius * std::cos(forwardAngle),
                            start.y + NodeRadius * std::sin(forwardAngle));
    const sf::Vector2f to(end.x + (NodeRadius + ArrowDepth) * std::cos(reverseAngle),
                          end.y + (NodeRadius + ArrowDepth) * std::sin(reverseAngle));

    const sf::Vector2f sCorner1(from.x + ArrowWidth * std::cos(perpAngle),
                                from.y + ArrowWidth * std::sin(perpAngle));
    const sf::Vector2f sCorner2(from.x - ArrowWidth * std::cos(perpAngle),
                                from.y - ArrowWidth * std::sin(perpAngle));
    const sf::Vector2f eCorner1(to.x + ArrowWidth * std::cos(perpAngle),
                                to.y + ArrowWidth * std::sin(perpAngle));
    const sf::Vector2f eCorner2(to.x - ArrowWidth * std::cos(perpAngle),
                                to.y - ArrowWidth * std::sin(perpAngle));

    // Rect triangle 1
    buffer[offset].position     = sCorner1;
    buffer[offset + 1].position = sCorner2;
    buffer[offset + 2].position = eCorner2;

    // Rect triangle 2
    buffer[offset + 3].position = eCorner1;
    buffer[offset + 4].position = eCorner2;
    buffer[offset + 5].position = sCorner1;

    // Arrow
    buffer[offset + 6].position = {end.x + NodeRadius * std::cos(reverseAngle),
                                   end.y + NodeRadius * std::sin(reverseAngle)};
    buffer[offset + 7].position = {to.x + ArrowDepth * std::cos(perpAngle),
                                   to.y + ArrowDepth * std::sin(perpAngle)};
    buffer[offset + 8].position = {to.x - ArrowDepth * std::cos(perpAngle),
                                   to.y - ArrowDepth * std::sin(perpAngle)};

    // Colors
    const sf::Color color = end.y > start.y ? DownArrowColor : UpArrowColor;
    for (unsigned int i = 0; i < ArrowVertexCount; ++i) { buffer[offset + i].color = color; }
}

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
, vertexBuffer(sf::PrimitiveType::Triangles, sf::VertexBuffer::Usage::Static, 0)
, selected(0)
, flashTime(0.f) {
    getSignal(Event::AcquisitionChanged)
        .willAlwaysCall(std::bind(&ConversationTree::updateBackground, this));
    getSignal(Event::Moved).willAlwaysCall(std::bind(&ConversationTree::updateBackground, this));
    getSignal(Event::Dragged)
        .willAlwaysCall(std::bind(&ConversationTree::onDrag, this, std::placeholders::_1));
    getSignal(Event::LeftClicked)
        .willAlwaysCall(std::bind(&ConversationTree::onClick, this, std::placeholders::_1));

    view.setSize(800.f, 800.f);
    view.setCenter(0.f, 400.f);
    background.setFillColor(sf::Color::Cyan);
    background.setOutlineThickness(-2.f);
    background.setOutlineColor(sf::Color::Black);
}

void ConversationTree::updateBackground() {
    background.setPosition(getPosition());
    background.setSize({getAcquisition().width, getAcquisition().height});
}

bool ConversationTree::handleScroll(const bl::gui::Event& e) {
    if (getAcquisition().contains(e.mousePosition())) {
        const float d = e.scrollDelta() * 20.f;
        view.setSize(view.getSize() - sf::Vector2f{d, d});
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
    setSelectedColor(renderNodes[selected].terminator ? TerminatorColor : NodeColor);
    selected = i;
}

void ConversationTree::onDrag(const bl::gui::Event& e) {
    const sf::Vector2f dragStart = transformToTreeCoord(e.dragStart());
    const sf::Vector2f mpos      = transformToTreeCoord(e.mousePosition());
    view.move(dragStart - mpos);
}

sf::Vector2f ConversationTree::transformToTreeCoord(const sf::Vector2f& p) const {
    const float sx             = view.getSize().x / getAcquisition().width;
    const float sy             = view.getSize().y / getAcquisition().height;
    const sf::Vector2f shifted = p - getPosition();
    const sf::Vector2f corner(view.getCenter() - view.getSize() * 0.5f);
    const sf::Vector2f scaled(shifted.x * sx, shifted.y * sy);
    return corner + scaled;
}

void ConversationTree::update(const std::vector<core::file::Conversation::Node>& nodes) {
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
            nodePositions[i], i, nodes[i], vIndex, vIndex + CircleVertexCount, terminators[i]);
        vIndex += CircleVertexCount;
    }

    // Reset bounds
    sf::FloatRect bounds;
    for (unsigned int i = 0; i < vertexBuffer.size(); ++i) {
        bounds.left   = std::min(bounds.left, vertexBuffer[i].position.x);
        bounds.top    = std::min(bounds.top, vertexBuffer[i].position.y);
        bounds.width  = std::max(bounds.width, vertexBuffer[i].position.x);
        bounds.height = std::max(bounds.height, vertexBuffer[i].position.y);
    }
    const sf::Vector2f ds(bounds.width - bounds.left, bounds.height - bounds.top);
    const sf::Vector2f size(std::max(ds.x, ds.y), std::max(ds.x, ds.y));
    view.setCenter((bounds.left + bounds.width) * 0.5f, (bounds.top + bounds.height) * 0.5f);
    view.setSize(size);

    vertexBuffer.update();
}

void ConversationTree::update(float dt) {
    bl::gui::Element::update(dt);

    if (selected < renderNodes.size()) {
        std::optional<sf::Color> color;
        if (flashTime > 0.f) {
            flashTime += dt;
            if (flashTime > FlashPeriod) {
                flashTime = -flashTime + FlashPeriod;
                color     = renderNodes[selected].terminator ? TerminatorColor : NodeColor;
            }
        }
        else {
            flashTime -= dt;
            if (flashTime < -FlashPeriod) {
                flashTime = -flashTime - FlashPeriod;
                color     = SelectedNodeColor;
            }
        }

        if (color.has_value()) { setSelectedColor(color.value()); }
    }
}

void ConversationTree::setSelectedColor(const sf::Color& c) {
    const unsigned int si = renderNodes[selected].vBegin;
    const unsigned int ei = renderNodes[selected].vEnd;
    for (unsigned int i = si; i < ei; ++i) { vertexBuffer[i].color = c; }
    vertexBuffer.update(si, ei - si);
}

sf::Vector2f ConversationTree::minimumRequisition() const { return {500.f, 500.f}; }

void ConversationTree::doRender(sf::RenderTarget& target, sf::RenderStates states,
                                const bl::gui::Renderer&) const {
    if (renderNodes.empty()) return;

    const float w = static_cast<float>(target.getSize().x);
    const float h = static_cast<float>(target.getSize().y);
    view.setViewport({getPosition().x / w,
                      getPosition().y / h,
                      getAcquisition().width / w,
                      getAcquisition().height / h});

    target.draw(background, states);
    const sf::View oldView = target.getView();
    target.setView(view);
    target.draw(vertexBuffer, states);
    sf::CircleShape circle;
    circle.setFillColor(sf::Color(255, 0, 0, 40));
    circle.setRadius(NodeRadius);
    circle.setOrigin(NodeRadius, NodeRadius);
    for (const Node& node : renderNodes) {
        target.draw(node.label, states);
        circle.setPosition(node.center);
        target.draw(circle);
    }
    target.setView(oldView);
}

ConversationTree::Node::Node() {
    label.setFillColor(sf::Color::Black);
    label.setCharacterSize(18.f);
    label.setStyle(sf::Text::Style::Bold);
    label.setFont(core::Properties::MenuFont());
}

void ConversationTree::Node::setup(const sf::Vector2f& pos, unsigned int i,
                                   const core::file::Conversation::Node& node, unsigned int vi,
                                   unsigned int ve, bool t) {
    center = pos;
    label.setString(nodeToString(i, node));
    label.setOrigin(label.getLocalBounds().width * 0.5f, label.getLocalBounds().height * 0.5f);
    label.setPosition(pos);
    vBegin     = vi;
    vEnd       = ve;
    terminator = t;
    index      = i;
}

} // namespace component
} // namespace editor
