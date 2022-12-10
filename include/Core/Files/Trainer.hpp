#ifndef CORE_FILES_TRAINER_HPP
#define CORE_FILES_TRAINER_HPP

#include <BLIB/Resources.hpp>
#include <BLIB/Serialization.hpp>
#include <Core/Files/Behavior.hpp>
#include <Core/Items/Id.hpp>
#include <Core/Peoplemon/OwnedPeoplemon.hpp>

namespace core
{
namespace file
{
/**
 * @brief Static data for trainers in the world
 *
 * @ingroup Files
 *
 */
struct Trainer {
public:
    /**
     * @brief Gives sane defaults
     *
     */
    Trainer();

    /**
     * @brief Saves the trainer data to the given file
     *
     * @param file The file to save to
     * @return True on save, false on error
     */
    bool save(const std::string& file) const;

    /**
     * @brief Saves the data from this object to the given bundle and registers depency files if any
     *
     * @param output Stream to output to
     * @param ctx Context to register dependencies with
     * @return True if serialization succeeded, false otherwise
     */
    bool saveBundle(bl::serial::binary::OutputStream& output,
                    bl::resource::bundle::FileHandlerContext& ctx) const;

    /**
     * @brief Loads the trainer data from the given file
     *
     * @param file The file to load from
     * @param spawnDir The direction to put in the standing behavior on load
     * @return True on load, false on error
     */
    bool load(const std::string& file, component::Direction spawnDir = component::Direction::Down);

    /**
     * @brief Loads the trainer from the json stream
     *
     * @param input JSON input stream
     * @return True if the trainer could be loaded, false on error
     */
    bool loadDev(std::istream& input);

    /**
     * @brief Loads the trainer from it's binary format
     *
     * @param input The input stream to load from
     * @return True if the trainer could be loaded, false otherwise
     */
    bool loadProd(bl::serial::binary::InputStream& input);

    std::string name;
    std::string animation;
    std::string prebattleConversation;
    std::string postBattleConversation;
    std::string lostBattleLine;
    std::uint8_t visionRange;
    Behavior behavior;
    std::vector<pplmn::OwnedPeoplemon> peoplemon;
    std::vector<item::Id> items;
    std::uint8_t payout;
    // TODO - determine how to store battle music/background

    std::string sourceFile;
};

} // namespace file
} // namespace core

namespace bl
{
namespace serial
{
template<>
struct SerializableObject<core::file::Trainer> : public SerializableObjectBase {
    using T   = core::file::Trainer;
    using Ppl = core::pplmn::OwnedPeoplemon;

    SerializableField<1, T, std::string> name;
    SerializableField<2, T, std::string> anim;
    SerializableField<3, T, std::string> preBattle;
    SerializableField<4, T, std::string> postBattle;
    SerializableField<5, T, std::string> loseBattleLine;
    SerializableField<6, T, std::uint8_t> range;
    SerializableField<7, T, core::file::Behavior> behavior;
    SerializableField<8, T, std::vector<Ppl>> peoplemon;
    SerializableField<9, T, std::vector<core::item::Id>> items;
    SerializableField<10, T, std::uint8_t> payout;

    SerializableObject()
    : SerializableObjectBase("Trainer")
    , name("name", *this, &T::name, SerializableFieldBase::Required{})
    , anim("anim", *this, &T::animation, SerializableFieldBase::Required{})
    , preBattle("preBattle", *this, &T::prebattleConversation, SerializableFieldBase::Required{})
    , postBattle("postBattle", *this, &T::postBattleConversation, SerializableFieldBase::Required{})
    , loseBattleLine("loseLine", *this, &T::lostBattleLine, SerializableFieldBase::Required{})
    , range("range", *this, &T::visionRange, SerializableFieldBase::Required{})
    , behavior("behavior", *this, &T::behavior, SerializableFieldBase::Required{})
    , peoplemon("peoplemon", *this, &T::peoplemon, SerializableFieldBase::Required{})
    , items("items", *this, &T::items, SerializableFieldBase::Required{})
    , payout("payout", *this, &T::payout, SerializableFieldBase::Optional{}) {
        payout.setDefault(40);
    }
};

} // namespace serial
} // namespace bl

#endif
