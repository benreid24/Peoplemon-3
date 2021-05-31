# Map Scripts

Map scripts come in two flavors: *event* scripts and *load* scripts. The former are triggered by event zones in the maps themselves, while the
latter are triggered on map enter or exit. Some map built-ins are available in both flavors while others are only in one flavor of script.

## Common Map Functions

| Name            | Parameters                                                   | Return  | Description                                                                                                                                |
|-----------------|--------------------------------------------------------------|---------|--------------------------------------------------------------------------------------------------------------------------------------------|
| loadMap         | `file`: String, `spawn`: Numeric                             | N/A     | Switches to the new map and spawn. `"LastMap"` may be passed to return the player to the previous map and position                         |
| setAmbientLight | `lightLevel`: Numeric                                        | N/A     | Overrides the ambient light level. 0 is full light, 255 is full dark. Pass -1 to allow time of day to determine light level                |
| createLight     | `x`: Numeric, `y`: Numeric, `radius`: Numeric                | Numeric | Creates a light at the given position and radius. All parameters are in pixels. Returns the unique id of the new light                     |
| updateLight     | `id`: Numeric, `x`: Numeric, `y`: Numeric, `radius`: Numeric | Bool    | Updates the light with the given `id` to the new position and radius. Returns true if the light was modified, false if `id` does not exist |
| removeLight     | `id`: Numeric                                                | Bool    | Deletes the light with the given `id`. Returns true if the light was deleted, false if `id` was not found                                  |

## Weather

All map scripts may manipulate the weather with the below functions:

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

## Entity Tile Events

These are ran when an entity triggers an event by stepping into the map event's region. Both the player and NPC's may trigger events so it is important 
to check that `triggeringEntity() == getPlayer()` if the event should only have effect for the player

| Name             | Parameters | Return                             | Description                                                                                |
|------------------|------------|------------------------------------|--------------------------------------------------------------------------------------------|
| triggeringEntity | N/A        | Numeric                            | Returns the id of the NPC, trainer, or player that triggered the event                     |
| triggerPosition  | N/A        | [Position](types.md#position)      | Returns the position of the entity that triggered the event                                |
| eventType        | N/A        | String                             | Returns one of: `"OnEnter"`, `"OnExit"`, `"OnEnterOrExit"`, `"WhileIn"`, or `"OnInteract"` |
| eventPosition    | N/A        | [Coordinate](types.md#coordninate) | Returns the position of the top right corner of the event region                           |
| eventSize        | N/A        | [Coordinate](types.md#coordinate)  | Returns the size of the event in tiles                                                     |

## Map Load/Unload Events

These are the scripts that run when a map is entered or exited. Scripts ran on map enter are ran after all entities are spawned (including the player), 
and scripts that are ran on map exit are ran before any entities are removed.

| Name        | Parameters | Return  | Description                                           |
|-------------|------------|---------|-------------------------------------------------------|
| mapName     | N/A        | String  | Returns the name of the map being entered             |
| spawnId     | N/A        | Numeric | Returns the spawn id the player is being spawned at   |
| previousMap | N/A        | String  | Returns the name of the map the player is coming from |
