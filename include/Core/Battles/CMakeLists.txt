target_sources(Peoplemon PUBLIC
    Battle.hpp
    Battler.hpp
    BattleSkipper.hpp
    BattleState.hpp
    BattlerSubstate.hpp
    BattleView.hpp
    TurnAction.hpp
)

add_subdirectory(BattleControllers)
add_subdirectory(BattlerControllers)
add_subdirectory(Commands)
add_subdirectory(View)
