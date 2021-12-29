#ifndef CORE_SYSTEMS_PLAYER_HPP
#define CORE_SYSTEMS_PLAYER_HPP

#include <BLIB/Entities.hpp>
#include <BLIB/Events.hpp>
#include <Core/Components/Movable.hpp>
#include <Core/Components/Position.hpp>
#include <Core/Events/GameSave.hpp>
#include <Core/Peoplemon/OwnedPeoplemon.hpp>
#include <Core/Player/Bag.hpp>
#include <Core/Player/Gender.hpp>
#include <Core/Player/Input.hpp>

namespace core
{
namespace system
{
class Systems;

/**
 * @brief Primary system for managing the player and their data
 *
 * @ingroup Systems
 *
 */
class Player : public bl::event::Listener<event::GameSaving, event::GameLoading> {
public:
    /**
     * @brief Construct a new Player system
     *
     * @param owner The primary systems object
     */
    Player(Systems& owner);

    /**
     * @brief Spawns the player into the world
     *
     * @param position The position to spawn at
     * @return True if the player was spawned, false on error
     */
    bool spawnPlayer(const component::Position& position);

    /**
     * @brief Makes the given entity controlled by the player. Only one entity may be player
     *        controlled at a time
     *
     * @param entity The entity to receive player input
     * @return True if added, false on error
     */
    bool makePlayerControlled(bl::entity::Entity entity);

    /**
     * @brief Removes the PlayerControlled component from the given entity, if any
     *
     * @param entity The entity to remove player control from
     */
    void removePlayerControlled(bl::entity::Entity entity);

    /**
     * @brief Returns the id of the player entity
     *
     */
    bl::entity::Entity player() const;

    /**
     * @brief Returns the current position of the player
     *
     */
    const component::Position& position() const;

    /**
     * @brief Returns a reference to the player input system
     *
     */
    player::Input& inputSystem();

    /**
     * @brief Initializes all player data structures for a new game
     *
     * @param name The name of the player
     * @param gender The gender of the player
     */
    void newGame(const std::string& name, player::Gender gender);

    /**
     * @brief Returns the name of the player
     *
     */
    const std::string& name() const;

    /**
     * @brief Returns the peoplemon owned by the player
     *
     */
    const std::vector<pplmn::OwnedPeoplemon>& team() const;

    /**
     * @brief Returns the peoplemon owned by the player
     *
     */
    std::vector<pplmn::OwnedPeoplemon>& team();

    /**
     * @brief Returns the player's bag
     *
     */
    player::Bag& bag();

    /**
     * @brief Returns the player's bag
     *
     */
    const player::Bag& bag() const;

    /**
     * @brief Returns the amount of money the player has
     *
     */
    long money() const;

    /**
     * @brief Returns the amount of money the player has
     *
     */
    long& money();

    /**
     * @brief Subscribes the input system to the event bus
     *
     */
    void init();

    /**
     * @brief Updates the player system
     *
     */
    void update();

private:
    Systems& owner;
    bl::entity::Entity playerId;
    bl::entity::Registry::ComponentHandle<component::Position> _position;
    bl::entity::Registry::ComponentHandle<component::Movable> movable;

    player::Input input;
    std::string playerName;
    player::Gender gender;
    player::Bag inventory;
    long monei;
    std::vector<pplmn::OwnedPeoplemon> peoplemon;
    component::Position savePos;

    virtual void observe(const event::GameSaving& save) override;
    virtual void observe(const event::GameLoading& load) override;

    friend class bl::serial::json::SerializableObject<Player>;
};

} // namespace system
} // namespace core

namespace bl
{
namespace serial
{
namespace json
{
template<>
struct SerializableObject<core::system::Player> : public SerializableObjectBase {
    using Player = core::system::Player;

    SerializableField<Player, std::string> name;
    SerializableField<Player, core::player::Gender> gender;
    SerializableField<Player, core::player::Bag> bag;
    SerializableField<Player, std::vector<core::pplmn::OwnedPeoplemon>> peoplemon;
    SerializableField<Player, long> money;

    SerializableObject()
    : name("name", *this, &Player::playerName)
    , gender("gender", *this, &Player::gender)
    , bag("bag", *this, &Player::inventory)
    , peoplemon("peoplemon", *this, &Player::peoplemon)
    , money("money", *this, &Player::monei) {}
};

} // namespace json
} // namespace serial
} // namespace bl

#endif
