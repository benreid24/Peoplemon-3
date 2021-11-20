#ifndef CORE_PEOPLEMON_MOVE_HPP
#define CORE_PEOPLEMON_MOVE_HPP

#include <unordered_map>

#include <Core/Peoplemon/MoveEffect.hpp>
#include <Core/Peoplemon/MoveId.hpp>
#include <Core/Peoplemon/Type.hpp>

namespace core
{
namespace pplmn
{
/**
 * @brief Encapsulates the move datastore
 *
 * @ingroup Peoplemon
 *
 */
class Move {
public:
    /**
     * @brief Converts the given raw id to a move id, or Unknown if invalid
     *
     * @param id The id to convert
     * @return MoveId The converted id
     */
    static MoveId cast(unsigned int id);

    /**
     * @brief Returns the name of the given move
     *
     */
    static const std::string& name(MoveId move);

    /**
     * @brief Returns the description of the given move
     *
     */
    static const std::string& description(MoveId move);

    /**
     * @brief Returns the type of the given move
     *
     */
    static Type type(Move move);

    /**
     * @brief Returns the damage of the given move
     *
     */
    static int damage(MoveId move);

    /**
     * @brief Returns the accuracy of the given move
     *
     */
    static int accuracy(MoveId move);

    /**
     * @brief Returns the priority of the given move
     *
     */
    static int priority(MoveId move);

    /**
     * @brief Returns the max pp of the given move
     *
     */
    static unsigned int pp(MoveId move);

    /**
     * @brief Returns whether or not the given move makes physical contact when used
     *
     */
    static bool makesContact(MoveId move);

    /**
     * @brief Returns whether or not the given move is special
     *
     */
    static bool isSpecial(MoveId move);

    /**
     * @brief Returns the effect of the given move
     *
     */
    static MoveEffect effect(MoveId move);

    /**
     * @brief Returns the effect chance of the given move
     *
     */
    static int effectChance(MoveId move);

    /**
     * @brief Returns the effect intensity of the given move
     *
     */
    static int effectIntensity(MoveId move);

    /**
     * @brief Returns whether or not the given move affects the user or the opponent
     *
     */
    static bool effectsUser(MoveId move);
    // TODO - maybe refactor to allow arbitrary target? (double battles!)

private:
    std::unordered_map<MoveId, std::string> names;
    std::unordered_map<MoveId, std::string> descriptions;
    std::unordered_map<MoveId, Type> types;
    std::unordered_map<MoveId, int> damages;
    std::unordered_map<MoveId, int> accuracies;
    std::unordered_map<MoveId, int> priorities;
    std::unordered_map<MoveId, unsigned int> pps;
    std::unordered_map<MoveId, bool> contactors;
    std::unordered_map<MoveId, bool> specials;
    std::unordered_map<MoveId, MoveEffect> effects;
    std::unordered_map<MoveId, int> effectChances;
    std::unordered_map<MoveId, int> effectIntensities;
    std::unordered_map<MoveId, bool> effectSelves;
};

} // namespace pplmn
} // namespace core

#endif
