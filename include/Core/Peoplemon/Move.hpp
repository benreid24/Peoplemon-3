#ifndef CORE_PEOPLEMON_MOVE_HPP
#define CORE_PEOPLEMON_MOVE_HPP

#include <unordered_map>

#include <Core/Files/MoveDB.hpp>
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
     * @brief Returns the list of all valid move ids
     *
     * @return const std::vector<MoveId>& The list of all valid move ids
     */
    static const std::vector<MoveId>& validIds();

    /**
     * @brief Set the data source for each method
     *
     * @param source The data source. Must remain in scope
     */
    static void setDataSource(file::MoveDB& source);

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
    static Type type(MoveId move);

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

    /**
     * @brief Returns the path to the animation for when the local player uses the move
     *
     * @param move The move to get the animation for
     * @return std::string The full path to the move animation
     */
    static std::string playerAnimationBackground(MoveId move);

    /**
     * @brief Returns the path to the animation for when the local player uses the move
     *
     * @param move The move to get the animation for
     * @return std::string The full path to the move animation
     */
    static std::string playerAnimationForeground(MoveId move);

    /**
     * @brief Returns the path to the animation for when the opponent uses the move
     *
     * @param move The move to get the animation for
     * @return std::string The full path to the move animation
     */
    static std::string opponentAnimationBackground(MoveId move);

    /**
     * @brief Returns the path to the animation for when the opponent uses the move
     *
     * @param move The move to get the animation for
     * @return std::string The full path to the move animation
     */
    static std::string opponentAnimationForeground(MoveId move);

private:
    static std::unordered_map<MoveId, std::string>* names;
    static std::unordered_map<MoveId, std::string>* descriptions;
    static std::unordered_map<MoveId, std::string>* animationPaths;
    static std::unordered_map<MoveId, Type>* types;
    static std::unordered_map<MoveId, int>* damages;
    static std::unordered_map<MoveId, int>* accuracies;
    static std::unordered_map<MoveId, int>* priorities;
    static std::unordered_map<MoveId, unsigned int>* pps;
    static std::unordered_map<MoveId, bool>* contactors;
    static std::unordered_map<MoveId, bool>* specials;
    static std::unordered_map<MoveId, MoveEffect>* effects;
    static std::unordered_map<MoveId, int>* effectChances;
    static std::unordered_map<MoveId, int>* effectIntensities;
    static std::unordered_map<MoveId, bool>* effectSelves;
};

} // namespace pplmn
} // namespace core

#endif
