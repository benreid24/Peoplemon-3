target_sources(Peoplemon PUBLIC
    Battle.hpp
    Battler.hpp
    BattleState.hpp
)

add_subdirectory(BattleControllers)
add_subdirectory(BattlerControllers)
add_subdirectory(Commands)
