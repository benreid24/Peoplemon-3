# Peoplemon Scripting

This is the centralized document outlining everything related to scripting in Peoplemon. Built-in
functions available in all the different Peoplemon script types are documented below, as are several
common return types. For resources on **bScript** itself or specific script types see the linked
external documentation.

## External Resources

- [**bScript** Language Reference](https://github.com/benreid24/BLIB/blob/master/docs/wiki/bscript_reference.md)
- [**bScript** built-in functions](https://github.com/benreid24/BLIB/blob/master/docs/wiki/bscript_functions.md)
- [Peoplemon Map Script Reference](maps.md)
- [Peoplemon Conversation Script Reference](conversation.md)
- [Function Return Types Reference](types.md)

## Functions

The following functions are available in each type of Peoplemon script, in addition to the built-in
**bScript** functions

### Player

| Name             | Parameters                                                        | Return | Description                                                                                                                                                                                              |
|------------------|-------------------------------------------------------------------|--------|----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
| getPlayer        | N/A                                                               | Player | Returns all player information                                                                                                                                                                           |
| giveItem         | `id`: Numeric, `qty`: Numeric, `showMessage`: bool, `block`: Bool | N/A    | Gives `qty` of the item to the player. Optionally prints the received item message to the hud. If `displayMessage` and `block` are both true then the function only returns when the output is complete  |
| giveMoney        | `qty`: Numeric, `showMessage`: Bool, `block`: Bool                | N/A    | Gives money to the player. Behaves the same as `giveItem`                                                                                                                                                |
| takeItem         | `id`: Numeric, `qty`: Numeric, `prompt`: Bool                     | Bool   | Tries to take `qty` of the item from the player. If `prompt` is true then it will ask before trying to take. Returns true if the items were successfully taken. Always blocks                            |
| takeMoney        | `qty`: Numeric, `prompt`: Bool                                    | Bool   | Tries to take `qty` money from the player. Behaves the same as `takeItem`                                                                                                                                |
| givePeoplemon    | `file`: String, `showMessage`: Bool                               | N/A    | Gives the Peoplemon in the given file to the player. If `showMessage` is true the function blocks until the output is complete                                                                           |
| takePeoplemon    | `id`: Numeric, `minLevel`: Numeric                                | Bool   | Tries to take the Peoplemon from the player. Always prompts and blocks on input. Specify `minLevel` as 0 to accept any level                                                                             |
| whiteout         | N/A                                                               | N/A    | Forces the player to respawn at the last PC Center they visited                                                                                                                                          |
| restorePeoplemon | N/A                                                               | N/A    | Restores the player's Peoplemon to full health and status                                                                                                                                                |
### Interface

| Name           | Parameters                                   | Return | Description                                                                         |
|----------------|----------------------------------------------|--------|-------------------------------------------------------------------------------------|
| displayMessage | `message`: String, `block`: Bool             | N/A    | Displays the message in the hud. Optionally blocks until message is complete        |
| promptPlayer   | `prompt`: String, `choices`: Array of String | String | Prompts the user with a set of choices and returns the chosen choice. Always blocks |
| rollCredits    | N/A                                          | N/A    | Plays the credits                                                                   |

### NPC's and Trainers

| Name         | Parameters                                                                        | Return    | Description                                                                                          |
|--------------|-----------------------------------------------------------------------------------|-----------|------------------------------------------------------------------------------------------------------|
| getNpc       | `name`: String                                                                    | Character | Returns the NPC by name. If not found returns `false`                                                |
| getTrainer   | `name`: String                                                                    | Character | Returns the Trainer by name. If not found returns `false`                                            |
| spawnNpc     | `file`: String, `level`: Numeric, `x`: Numeric, `y`: Numeric, `direction`: String | Bool      | Spawns the NPC from the file at the position. Returns true if file valid and NPC was spawned         |
| spawnTrainer | `file`: String, `level`: Numeric, `x`: Numeric, `y`: Numeric, `direction`: String | Bool      | Spawns the Trainer from the file at the position. Returns true if file valid and Trainer was spawned |

### Entity Manipulation

Includes NPC's, trainers, and the player.

| Name             | Parameters                                                                     | Return | Description                                                                                                          |
|------------------|--------------------------------------------------------------------------------|--------|----------------------------------------------------------------------------------------------------------------------|
| moveEntity       | `entity`: Numeric, `direction`: String, `block`: Bool                          | Bool   | Moves the entity in the direction. Optionally blocks until the move is complete. Returns true if the move had effect |
| rotateEntity     | `entity`: Numeric, `direction`: String                                         | N/A    | Makes the entity face the given direction                                                                            |
| removeEntity     | `entity`: Numeric                                                              | Bool   | Despawns the given entity. Returns true if an entity was despawned. Cannot despawn the player                        |
| entityToPosition | `entity`: Numeric, `level`: Numeric, `x`: Numeric, `y`: Numeric, `block`: Bool | Bool   | Locks and pathfinds the entity to the given position (tiles). Optionally blocks until the entity is in place         |
| entityInteract   | `entity`: Numeric                                                              | Bool   | Makes the entity perform an interaction. Returns true if an interaction occurred                                     |
| setEntityLock    | `entity`: Numeric, `locked`: Bool                                              | N/A    | Locks or unlocks the given entity. Locking prevents AI or the player from giving input                               |

### Game Time

| Name           | Parameters                     | Return | Description                                                                                                                                                                                                                  |
|----------------|--------------------------------|--------|------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
| getClock       | N/A                            | Time   | Returns the current game time                                                                                                                                                                                                |
| waitUntilTime  | `time`: Time, `exact`: Bool    | N/A    | Blocks until it is the given time of day. If exact is false this will return if it is already after the requested time. If exact is true then it will block until the time wraps around and becomes the exact requested time |
| runAtClockTime | `time`: Time, `script`: String | N/A    | Runs the given script (can be file or code) at the given time of day exactly. Returns immediately                                                                                                                            |
### Game Saves

| Name            | Parameters                  | Return | Description                                                                            |
|-----------------|-----------------------------|--------|----------------------------------------------------------------------------------------|
| addSaveEntry    | `key`: String, `value`: Any | N/A    | Saves the given value under the given key. Saved values are persisted in the game save |
| getSaveEntry    | `key`: String               | Any    | Retrieves the saved value with the given key. Returns `false` if not found             |
| removeSaveEntry | `key`: String               | N/A    | Deletes the given key from the persistent data                                         |

## Basic Example

A simple example to illustrate how the composite objects work:

```
// get player entity handle
player = getPlayer();
loginfo("The player's name is " + player.name);

// get npc entity handle
jim = getNpc("Jim");
if (not jim) {
    error("Failed to get NPC 'Jim'"); // error terminates the script
}

// jim walks over
entityToPosition(
    jim,
    player.position.level,
    player.position.tiles.x + 1, // one tile to the right
    player.position.tiles.y,
    true
);

// face each other
rotateEntity(player, "right");
rotateEntity(jim, "left");

// only the player or trainers may interact
// entityInteract(jim); would fail
entityInteract(player);
```
