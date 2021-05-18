#include <Core/Menu/GhostWriter.hpp>

#include <Core/Properties.hpp>
#include <cmath>

namespace core
{
namespace menu
{
GhostWriter::GhostWriter()
: showing(0)
, residual(0.f) {}

GhostWriter::GhostWriter(const std::string& content)
: content(content)
, showing(0)
, residual(0.f) {}

void GhostWriter::setContent(const std::string& c) {
    content  = c;
    showing  = 0;
    residual = 0.f;
}

const std::string& GhostWriter::getContent() const { return content; }

std::string_view GhostWriter::getVisible() const { return {content.c_str(), showing}; }

void GhostWriter::update(float dt) {
    if (finished()) return;

    residual += dt;
    const unsigned int a = std::floor(residual / Properties::GhostWriterSpeed());
    residual -= static_cast<float>(a) * Properties::GhostWriterSpeed();

    for (unsigned int i = 0; i < a; ++i) {
        do { ++showing; } while (showing < content.size() && std::isspace(content[showing]));
        if (finished()) break;
    }
}

bool GhostWriter::finished() const { return showing >= content.size(); }

} // namespace menu
} // namespace core
