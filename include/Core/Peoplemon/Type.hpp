#ifndef CORE_PEOPLEMON_TYPE_HPP
#define CORE_PEOPLEMON_TYPE_HPP

#include <cstdint>

namespace core
{
namespace pplmn
{
/**
 * @brief Represents a type that a move or Peoplemon can be. Types may be combined
 *
 * @ingroup Peoplemon
 *
 */
enum struct Type : std::uint8_t {
    None        = 0,
    Normal      = 0x1 << 0,
    Intelligent = 0x1 << 1,
    Funny       = 0x1 << 2,
    Athletic    = 0x1 << 3,
    Quiet       = 0x1 << 4,
    Awkward     = 0x1 << 5,
    PartyAnimal = 0x1 << 6
};

/**
 * @brief Binary AND for Type values
 *
 * @param l Left type
 * @param r Right type
 * @return Type Combined type
 */
inline Type operator&(Type l, Type r) {
    return static_cast<Type>(static_cast<std::uint8_t>(l) & static_cast<std::uint8_t>(r));
}

/**
 * @brief Binary OR for Type values
 *
 * @param l Left type
 * @param r Right type
 * @return Type Combined type
 */
inline Type operator|(Type l, Type r) {
    return static_cast<Type>(static_cast<std::uint8_t>(l) | static_cast<std::uint8_t>(r));
}

/**
 * @brief Checks whether the given type contains the type to check
 *
 * @param type The type being searched
 * @param check The type to check for
 * @return True if type contains the types in check, false otherwise
 */
inline bool isType(Type type, Type check) { return (type & check) == check; }

/**
 * @brief Helper method to convert old type ids to the new
 *
 * @param ogType The old type id
 * @return Type The corresponding new type value
 */
inline Type legacyTypeToNew(unsigned int ogType) {
    switch (ogType) {
    case 8:
        return Type::Funny | Type::PartyAnimal;
    case 9:
        return Type::Intelligent | Type::Athletic;
    case 10:
        return Type::Intelligent | Type::Normal;
    case 11:
        return Type::Normal | Type::Quiet;
    case 12:
        return Type::Awkward | Type::Funny;
    case 13:
        return Type::Intelligent | Type::Funny;
    case 14:
        return Type::Athletic | Type::Normal;
    case 15:
        return Type::Funny | Type::Normal;
    case 16:
        return Type::Normal | Type::Awkward;
    case 17:
        return Type::Quiet | Type::Athletic;
    case 18:
        return Type::Intelligent | Type::Awkward;
    default:
        if (ogType < 8) return static_cast<Type>(0x1 << ogType);
        return Type::None;
    }
}

} // namespace pplmn
} // namespace core

#endif
