target_sources(Core PUBLIC
    Battle.hpp
    Battler.hpp
    BattleState.hpp
    BattlerSubstate.hpp
    BattleView.hpp
    Result.hpp
    TurnAction.hpp
)

add_subdirectory(BattleControllers)
add_subdirectory(BattlerControllers)
add_subdirectory(Commands)
add_subdirectory(View)
