#include <Core/Systems/Interaction.hpp>

#include <Core/Components/Item.hpp>
#include <Core/Events/Item.hpp>
#include <Core/Items/Item.hpp>
#include <Core/Systems/Systems.hpp>

namespace core
{
namespace system
{
Interaction::Interaction(Systems& owner)
: owner(owner) {}

bool Interaction::interact(bl::entity::Entity interactor) {
    const component::Position* pos =
        owner.engine().entities().getComponent<component::Position>(interactor);
    if (!pos) return false;

    const bl::entity::Entity interacted = owner.position().search(*pos, pos->direction, 1);
    if (interacted == bl::entity::InvalidEntity) return false;

    // TODO - npc conversations

    // Check for item if player
    if (interactor == owner.player().player()) {
        const component::Item* ic =
            owner.engine().entities().getComponent<component::Item>(interacted);
        if (ic) {
            BL_LOG_INFO << "Player picked up: " << static_cast<unsigned int>(ic->id()) << "("
                        << item::Item::getName(ic->id()) << ")";
            owner.player().bag().addItem(ic->id(), 1);
            owner.engine().eventBus().dispatch<event::ItemPickedUp>({ic->id()});
            owner.engine().entities().destroyEntity(interacted);
            // TODO - print message to hud
            return true;
        }
    }

    return false;
}

} // namespace system
} // namespace core
