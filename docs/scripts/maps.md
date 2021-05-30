# Map Scripts

The following types of scripts are ran in the context of maps:

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
