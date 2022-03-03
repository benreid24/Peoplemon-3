target_sources(Peoplemon PRIVATE
    Battle.cpp
    Battler.cpp
    BattleState.cpp
    BattleView.cpp
)

add_subdirectory(BattleControllers)
add_subdirectory(BattlerControllers)
add_subdirectory(Commands)
