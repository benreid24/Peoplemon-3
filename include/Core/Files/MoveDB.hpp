#ifndef CORE_FILE_MOVEDB_HPP
#define CORE_FILE_MOVEDB_HPP

#include <BLIB/Serialization.hpp>
#include <BLIB/Util/NonCopyable.hpp>
#include <Core/Peoplemon/MoveEffect.hpp>
#include <Core/Peoplemon/MoveId.hpp>
#include <Core/Peoplemon/Type.hpp>
#include <cstdint>

namespace core
{
namespace file
{
struct MoveDBLoader;

/**
 * @brief Stores the metadata of all peoplemon moves
 *
 * @ingroup Files
 *
 */
struct MoveDB : private bl::util::NonCopyable {
    /**
     * @brief Loads the moves from the data file
     *
     * @return True on success, false on error
     */
    bool load();

    /**
     * @brief Loads the database from its json format
     *
     * @param input The input stream to load from
     * @return True if the data could be loaded, false otherwise
     */
    bool loadDev(std::istream& input);

    /**
     * @brief Loads the database from its json format
     *
     * @param input The input stream to load from
     * @return True if the data could be loaded, false otherwise
     */
    bool loadProd(bl::serial::binary::InputStream& input);

    /**
     * @brief Saves the moves to the data file
     *
     * @return True on success, false on error
     */
    bool save() const;

    std::unordered_map<pplmn::MoveId, std::string> names;
    std::unordered_map<pplmn::MoveId, std::string> descriptions;
    std::unordered_map<pplmn::MoveId, pplmn::Type> types;
    std::unordered_map<pplmn::MoveId, std::int32_t> damages;
    std::unordered_map<pplmn::MoveId, std::int32_t> accuracies;
    std::unordered_map<pplmn::MoveId, std::int32_t> priorities;
    std::unordered_map<pplmn::MoveId, std::uint32_t> pps;
    std::unordered_map<pplmn::MoveId, bool> contactors;
    std::unordered_map<pplmn::MoveId, bool> specials;
    std::unordered_map<pplmn::MoveId, pplmn::MoveEffect> effects;
    std::unordered_map<pplmn::MoveId, std::int32_t> effectChances;
    std::unordered_map<pplmn::MoveId, std::int32_t> effectIntensities;
    std::unordered_map<pplmn::MoveId, bool> effectSelves;

    friend struct MoveDBLoader;
    friend struct bl::serial::SerializableObject<MoveDB>;
};

} // namespace file
} // namespace core

namespace bl
{
namespace serial
{
template<>
struct SerializableObject<core::file::MoveDB> : public SerializableObjectBase {
    using Id = core::pplmn::MoveId;
    using DB = core::file::MoveDB;

    SerializableField<1, DB, std::unordered_map<Id, std::string>> names;
    SerializableField<2, DB, std::unordered_map<Id, std::string>> descriptions;
    SerializableField<4, DB, std::unordered_map<Id, core::pplmn::Type>> types;
    SerializableField<5, DB, std::unordered_map<Id, std::int32_t>> damages;
    SerializableField<6, DB, std::unordered_map<Id, std::int32_t>> accuracies;
    SerializableField<7, DB, std::unordered_map<Id, std::int32_t>> priorities;
    SerializableField<8, DB, std::unordered_map<Id, std::uint32_t>> pps;
    SerializableField<9, DB, std::unordered_map<Id, bool>> contactors;
    SerializableField<10, DB, std::unordered_map<Id, bool>> specials;
    SerializableField<11, DB, std::unordered_map<Id, core::pplmn::MoveEffect>> effects;
    SerializableField<12, DB, std::unordered_map<Id, std::int32_t>> effectChances;
    SerializableField<13, DB, std::unordered_map<Id, std::int32_t>> effectIntensities;
    SerializableField<14, DB, std::unordered_map<Id, bool>> effectSelves;

    SerializableObject()
    : names("names", *this, &DB::names, SerializableFieldBase::Required{})
    , descriptions("descriptions", *this, &DB::descriptions, SerializableFieldBase::Required{})
    , types("types", *this, &DB::types, SerializableFieldBase::Required{})
    , damages("damages", *this, &DB::damages, SerializableFieldBase::Required{})
    , accuracies("accs", *this, &DB::accuracies, SerializableFieldBase::Required{})
    , priorities("prioties", *this, &DB::priorities, SerializableFieldBase::Required{})
    , pps("pps", *this, &DB::pps, SerializableFieldBase::Required{})
    , contactors("contactors", *this, &DB::contactors, SerializableFieldBase::Required{})
    , specials("specials", *this, &DB::specials, SerializableFieldBase::Required{})
    , effects("effects", *this, &DB::effects, SerializableFieldBase::Required{})
    , effectChances("effectChances", *this, &DB::effectChances, SerializableFieldBase::Required{})
    , effectIntensities("effectIntensities", *this, &DB::effectIntensities,
                        SerializableFieldBase::Required{})
    , effectSelves("effectSelves", *this, &DB::effectSelves, SerializableFieldBase::Required{}) {}
};

} // namespace serial
} // namespace bl

#endif
