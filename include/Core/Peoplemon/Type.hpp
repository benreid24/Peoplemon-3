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
 * @brief Contains a collection of helper methods for types
 *
 * @ingroup Peoplemon
 *
 */
struct TypeUtil {
    /**
     * @brief Returns the multiplier for if/when a move is super effective
     *
     * @param moveType The type of move being used
     * @param defenderType The type the defender is
     * @return float The multiplier for if the move is super effective
     */
    static float getSuperMult(Type moveType, Type defenderType);

    /**
     * @brief Returns the STAB multiplier for a move
     *
     # @param moveType The type of move being used
     * @param defenderType The type the attacker is
     * @return float The multiplier for STAB
     */
    static float getStab(Type moveType, Type userType);

    /**
     * @brief Helper method to convert old type ids to the new
     *
     * @param ogType The old type id
     * @return Type The corresponding new type value
     */
    static Type legacyTypeToNew(unsigned int ogType);

    /**
     * @brief Checks whether the given type contains the type to check
     *
     * @param type The type being searched
     * @param check The type to check for
     * @return True if type contains the types in check, false otherwise
     */
    static inline bool isType(Type type, Type check) { return (type & check) == check; }
};

} // namespace pplmn
} // namespace core

#endif
