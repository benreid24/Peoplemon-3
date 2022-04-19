#include <Core/Peoplemon/Type.hpp>

namespace core
{
namespace pplmn
{
namespace
{
float compareSingleEffective(Type move, Type ppl) {
    switch (move) {
    case Type::None:
        return 1.f;

    case Type::Normal:
        if (ppl == Type::Awkward) return 0.f;
        break;

    case Type::Intelligent:
        if (ppl == Type::Funny) return 0.5f;
        if (ppl == Type::Athletic) return 2.f;
        break;

    case Type::Funny:
        switch (ppl) {
        case Type::Intelligent:
        case Type::PartyAnimal:
            return 2.f;
        case Type::Athletic:
        case Type::Awkward:
        case Type::Quiet:
            return 0.5f;
        default:
            return 1.f;
        }

    case Type::Athletic:
        switch (ppl) {
        case Type::Normal:
        case Type::Funny:
            return 2.f;
        case Type::Intelligent:
            return 0.5f;
        case Type::Awkward:
            return 0.f;
        default:
            return 1.f;
        }

    case Type::Quiet:
        if (ppl == Type::Awkward) return 2.f;
        if (ppl == Type::PartyAnimal) return 0.5f;
        break;

    case Type::Awkward:
        switch (ppl) {
        case Type::Normal:
        case Type::Athletic:
            return 0.f;
        case Type::Quiet:
            return 0.5f;
        case Type::Funny:
        case Type::Awkward:
            return 2.f;
        default:
            return 1.f;
        }

    case Type::PartyAnimal:
        switch (ppl) {
        case Type::Funny:
            return 0.5f;
        case Type::Athletic:
        case Type::Quiet:
            return 2.f;
        default:
            return 1.f;
        }

    default:
        return 1.f;
    }

    return 1.f;
}
} // namespace

float TypeUtil::getStab(Type move, Type ppl) { return isType(ppl, move) ? 1.5f : 1.f; }

float TypeUtil::getSuperMult(Type move, Type ppl) {
    float m = 1.f;
    for (unsigned int i = 0; i <= 6; ++i) {
        const Type type = static_cast<Type>(0x1 << i);
        if (isType(ppl, type)) { m *= compareSingleEffective(move, type); }
    }
    return m;
}

Type TypeUtil::legacyTypeToNew(unsigned int ogType) {
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
