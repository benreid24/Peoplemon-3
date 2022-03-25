target_sources(Core PRIVATE
    Battle.cpp
    Battler.cpp
    BattleState.cpp
    BattleView.cpp
)

add_subdirectory(BattleControllers)
add_subdirectory(BattlerControllers)
add_subdirectory(Commands)
add_subdirectory(View)
