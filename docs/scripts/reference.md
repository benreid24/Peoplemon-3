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
| getPlayer        | N/A                                                               | [Player](types.md#player) | Returns all player information                                                                                                                                                                           |
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

| Name         | Parameters                                                                        | Return    | Description                                                                                                  |
|--------------|-----------------------------------------------------------------------------------|-----------|--------------------------------------------------------------------------------------------------------------|
| getNpc       | `name`: String                                                                    | [Character](types.md#character) | Returns the NPC by name. If not found returns `false`                                  |
| getTrainer   | `name`: String                                                                    | [Character](types.md#character) | Returns the Trainer by name. If not found returns `false`                              |
| loadCharacter| `id`: Numeric                                                                     | [Character](types.md#character) | Loads the NPC or trainer info from the given id. Returns `false` if id is not valid    |
| spawnCharacter | `file`: String, `level`: Numeric, `x`: Numeric, `y`: Numeric, `direction`: String | Bool      | Spawns the NPC/Trainer from the file at the position. Returns true if file valid and Character was spawned |

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
| resetEntityLock  | `entity`: Numeric,                                                             | N/A    | Resets the given entity's lock state to what it was prior to `setEntityLock` being called                            |

### Game Time

| Name           | Parameters                     | Return | Description                                                                                                                                                                                                                                   |
|----------------|--------------------------------|--------|-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
| makeTime       | `hour`: Numeric, `minute`: Numeric              | [Time](types.md#time)   | Returns a constructed [Time](types.md#time) object from the given hour and minute                                                                                                                           |
| getClock       | N/A                                             | [Time](types.md#time)   | Returns the current game                                                                                                                                                                                    |
| waitUntilTime  | `time`: [Time](types.md#time), `exact`: Bool    | N/A    | Blocks until it is the given time of day. If exact is false this will return if it is already after the requested time. If exact is true then it will block until the time wraps around and becomes the exact requested time |
| runAtClockTime | `time`: [Time](types.md#time), `script`: String | N/A    | Runs the given script (can be file or code) at the given time of day exactly. Returns immediately                                                                                                                            |

### Game Saves

| Name            | Parameters                  | Return | Description                                                                            |
|-----------------|-----------------------------|--------|----------------------------------------------------------------------------------------|
| addSaveEntry    | `key`: String, `value`: Any | N/A    | Saves the given value under the given key. Saved values are persisted in the game save |
| getSaveEntry    | `key`: String               | Any    | Retrieves the saved value with the given key. Returns `false` if not found             |
| removeSaveEntry | `key`: String               | N/A    | Deletes the given key from the persistent data                                         |


## Maps

| Name            | Parameters                                                   | Return  | Description                                                                                                                                |
|-----------------|--------------------------------------------------------------|---------|--------------------------------------------------------------------------------------------------------------------------------------------|
| loadMap         | `file`: String, `spawn`: Numeric                             | N/A     | Switches to the new map and spawn. `"LastMap"` may be passed to return the player to the previous map and position                         |
| setAmbientLight | `low`: Numeric, `high`: Numeric, `sunlightAdjust`: Bool   | N/A     | Overrides the ambient light level. 0 is full dark, 255 is full light. Sunlight causes the ambient to move between the low and high based on time. If no sunlight the high level is used as a constant light level |
| createLight     | `x`: Numeric, `y`: Numeric, `radius`: Numeric                | Numeric | Creates a light at the given position and radius. All parameters are in pixels. Returns the unique id of the new light                     |
| updateLight     | `id`: Numeric, `x`: Numeric, `y`: Numeric, `radius`: Numeric | N/A     | Updates the light with the given `id` to the new position and radius.                                                                      |
| removeLight     | `id`: Numeric                                                | N/A     | Deletes the light with the given `id`                                                                                                      |

## Weather

| Name              | Parameters                       | Return | Description                                                |
|-------------------|----------------------------------|--------|------------------------------------------------------------|
| clearWeather      | N/A                              | N/A    | Clears the current weather                                 |
| makeRain          | `isLight`: Bool, `thunder`: Bool | N/A    | Creates light or hard rain with optional thunder           |
| makeSnow          | `isLight`: Bool, `thunder`: Bool | N/A    | Creates light or hard snow with optional thunder           |
| makeSunny         | N/A                              | N/A    | Creates bright sunny weather                               |
| makeSandstorm     | N/A                              | N/A    | Creates a raging sandstorm                                 |
| makeFog           | `thick`: Bool                    | N/A    | Creates thin or thick fog                                  |
| makeRandomRain    | N/A                              | N/A    | The game will cycle through different varieties of rain    |
| makeRandomSnow    | N/A                              | N/A    | The game will cycle through different varieties of snow    |
| makeRandomDesert  | N/A                              | N/A    | The game will cycle between sandstorm and sunny            |
| makeRandomWeather | N/A                              | N/A    | The game will cycle between all different kinds of weather |

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
