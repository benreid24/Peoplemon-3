# Debugging Peoplemon

To properly debug Peoplemon while performing development work it is important to use the executable from the editor download as it has extra features built-in to aid debugging, as well as generates more detailed logs. 

## Logs

The Peoplemon game, as well as the Peoplemon editor both generate detailed logs of what is happening as well as any problems they encounter. When working on the game it is important to pay attention to `WARNING` and `ERROR` level messages being output by the game, as these can inform you of improper or incorrect data. The first thing to check if a crash occurs is the logs, which may tell you exactly what happened. If the console window is closed the logs are also outputted to files. The game outputs to `verbose.log` and the editor outputs to `editor.log`. Both files are in the same directory as the executables.

## Scripts

Freehand scripts may be ran from the debug version of the game. To run a script press the tilde (`~`) key with the game window focused. This will open a prompt in the console window where a script may be entered. A file may also be entered. Scripts ran from the console have all of the same methods available as most other game scripts ([reference](./scripts/reference.md)). 

## Utilities

The following utilities are available in the debug build of Peoplemon:

1. **Map explorer**: Press `F1` to control a free camera and pan around the map. The camera controls are the same as the map editor. Press `ESC` to return to your character.
2. **Battle skipping**: Battles may be skipped by pressing `F2`, which will toggle the setting. Press this before battles. When battles are off wild Peoplemon will not attack. Trainers will still enter battle, but the battle will skip ahead to the trainer being defeated.
3. **Peoplemon catching**: All peopleballs can be given a catch rate of 100% by pressing `F3`. With this setting on Peoplemon will always be caught when a ball is used.
4. **Run away**: Press `F4` to be able to always escape battle with wild Peoplemon
5. **Evolution**: Press `F5` to instantly level up your current lead Peoplemon to the level it evolves at and trigger an evolution. Moves are learned along the way if spots are open. XP is not reset. EVs are not modified.
6. **Fly map**: Press `F6` to mark all towns as visited in order to fly to them
7. **Wild Peoplemon**: Press `F7` to immediately trigger a battle with a wild Peoplemon

## Game Saves

Game saves are stored in `Documents/My Games/Peoplemon/saves` and are simple json files. If the save editor utility is insufficient for your need you may edit the save by hand instead.
