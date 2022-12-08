#include <Core/Files/PeoplemonDB.hpp>

#include <Core/Peoplemon/Move.hpp>
#include <Core/Peoplemon/Peoplemon.hpp>
#include <Core/Properties.hpp>
#include <Core/Resources.hpp>

namespace core
{
namespace file
{
using namespace bl::serial::binary;

struct PeoplemonDBLoader : public SerializerVersion<PeoplemonDB> {
    using Serial = Serializer<PeoplemonDB>;

    virtual bool read(PeoplemonDB& db, InputStream& input) const override {
        return Serial::deserialize(input, db);
    }

    virtual bool write(const PeoplemonDB& db, OutputStream& output) const override {
        return Serial::serialize(output, db);
    }
};

namespace
{
struct LegacyDBLoader : public SerializerVersion<PeoplemonDB> {
    virtual bool read(PeoplemonDB& db, InputStream& input) const override {
        std::uint16_t n = 0;
        if (!input.read<std::uint16_t>(n)) return false;
        for (std::uint16_t i = 0; i < n; ++i) {
            std::uint16_t u16;
            if (!input.read<std::uint16_t>(u16)) return false;
            const pplmn::Id id = static_cast<pplmn::Id>(u16);

            std::string str;
            if (!input.read(str)) return false;
            db.names[id] = str;
            if (!input.read(str)) return false;
            db.descriptions[id] = str;

            std::uint8_t u8;
            if (!input.read<std::uint8_t>(u8)) return false;
            db.types[id] = pplmn::TypeUtil::legacyTypeToNew(u8);
            if (!input.read<std::uint8_t>(u8)) return false;
            db.abilities[id] = static_cast<pplmn::SpecialAbility>(u8);

            Stats stats;
            if (!input.read<std::uint16_t>(u16)) return false;
            stats.hp = u16;
            if (!input.read<std::uint16_t>(u16)) return false;
            stats.atk = u16;
            if (!input.read<std::uint16_t>(u16)) return false;
            stats.def = u16;
            if (!input.read<std::uint16_t>(u16)) return false; // acc
            if (!input.read<std::uint16_t>(u16)) return false; // evd
            if (!input.read<std::uint16_t>(u16)) return false;
            stats.spd = u16;
            if (!input.read<std::uint16_t>(u16)) return false; // crit
            if (!input.read<std::uint16_t>(u16)) return false;
            stats.spatk = u16;
            if (!input.read<std::uint16_t>(u16)) return false;
            stats.spdef  = u16;
            db.stats[id] = stats;

            if (!input.read<std::uint16_t>(u16)) return false;
            db.validMoves[id].reserve(u16);
            for (std::uint16_t j = 0; j < u16; ++j) {
                std::uint16_t mi;
                if (!input.read<std::uint16_t>(mi)) return false;
                const pplmn::MoveId m = pplmn::Move::cast(mi);
                if (m == pplmn::MoveId::Unknown) {
                    BL_LOG_ERROR << "Invalid learn move id " << mi << " for peoplemon "
                                 << static_cast<unsigned int>(id) << "(" << db.names[id] << ")";
                }
                else { db.validMoves[id].insert(m); }
            }

            if (!input.read<std::uint16_t>(u16)) return false;
            for (std::uint16_t j = 0; j < u16; ++j) {
                std::uint8_t lvl;
                std::uint16_t mi;
                if (!input.read<std::uint8_t>(lvl)) return false;
                if (!input.read<std::uint16_t>(mi)) return false;
                const pplmn::MoveId m = pplmn::Move::cast(mi);
                if (m == pplmn::MoveId::Unknown) {
                    BL_LOG_ERROR << "Invalid learn move id " << mi << " for peoplemon "
                                 << static_cast<unsigned int>(id) << "(" << db.names[id] << ")";
                }
                else {
                    db.learnedMoves[id][lvl] = m;
                    db.validMoves[id].insert(m);
                }
            }

            if (!input.read<std::uint8_t>(u8)) return false;
            db.evolveLevels[id] = u8;
            if (!input.read<std::uint8_t>(u8)) return false;
            pplmn::Id tid = pplmn::Peoplemon::cast(u8);
            if (tid == pplmn::Id::Unknown && u8 != 0) {
                BL_LOG_ERROR << "Invalid evolution id " << static_cast<unsigned int>(u8)
                             << " for peoplemon " << static_cast<unsigned int>(id) << "("
                             << db.names[id] << ")";
            }
            db.evolveIds[id] = tid;

            if (!input.read<std::uint16_t>(u16)) return false;
            db.xpMults[id] = u16;
            if (!input.read<std::uint8_t>(u8)) return false;
            db.xpGroups[id] = u8;

            if (!input.read<std::uint16_t>(u16)) return false;
            stats.hp = u16;
            if (!input.read<std::uint16_t>(u16)) return false;
            stats.atk = u16;
            if (!input.read<std::uint16_t>(u16)) return false;
            stats.def = u16;
            if (!input.read<std::uint16_t>(u16)) return false; // acc
            if (!input.read<std::uint16_t>(u16)) return false; // evd
            if (!input.read<std::uint16_t>(u16)) return false;
            stats.spd = u16;
            if (!input.read<std::uint16_t>(u16)) return false; // crit
            if (!input.read<std::uint16_t>(u16)) return false;
            stats.spatk = u16;
            if (!input.read<std::uint16_t>(u16)) return false;
            stats.spdef     = u16;
            db.evAwards[id] = stats;
        }
        return true;
    }

    virtual bool write(const PeoplemonDB&, OutputStream&) const override { return false; }
};

using VersionedLoader =
    bl::serial::binary::VersionedSerializer<PeoplemonDB, LegacyDBLoader, PeoplemonDBLoader>;

} // namespace

bool PeoplemonDB::load() {
    return PeoplemonDbManager::initializeExisting(Properties::PeoplemonDBFile(), *this);
}

bool PeoplemonDB::loadDev(std::istream& input) {
    return bl::serial::json::Serializer<PeoplemonDB>::deserializeStream(input, *this);
}

bool PeoplemonDB::loadProd(bl::serial::binary::InputStream& input) {
    return VersionedLoader::read(input, *this);
}

bool PeoplemonDB::save() const {
    std::ofstream output(Properties::PeoplemonDBFile().c_str());
    return bl::serial::json::Serializer<PeoplemonDB>::serializeStream(output, *this, 4, 0);
}

} // namespace file
} // namespace core
