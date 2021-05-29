# Types

Some Peoplemon functions return objects with many properties. This reduces the amount of functions
required to query game information by bundling it together. The custom types are as follows:

## `Time`

Time of day in game

| Name    | Type    | Description                          |
|---------|---------|--------------------------------------|
| minutes | Numeric | Minute of the hour, in range [0, 59] |
| hours   | Numeric | Hour of the day, in range [0, 23]    |

## `Coordinate`

A two dimensional vector

| Name | Type    | Description      |
|------|---------|------------------|
| x    | Numeric | The x coordinate |
| y    | Numeric | The y coordinate |

## `Position`

The position of an entity in the world

| Name      | Type       | Description                                     |
|-----------|------------|-------------------------------------------------|
| tiles     | [Coordinate](#coordinate) | Position in tiles                               |
| pixels    | [Coordinate](#coordinate) | The position in pixels                          |
| level     | Numeric    | The level the entity is on                      |
| direction | String     | One of `"up"`, `"right"`, `"down"`, or `"left"` |

## `BagItem`

An item stack in the player's bag. Type is `Numeric` and is the item id. Also has the following properties.

| Name | Type    | Description             |
|------|---------|-------------------------|
| id   | Numeric | The id of the item      |
| name | String  | The name of the item    |
| qty  | Numeric | How many are in the bag |

## `Player`

Large object containing most of the information about the player and their entity. The type of the
object is `Numeric` and represents the player's in-game entity id. It has the following properties:

| Name     | Type             | Description                             |
|----------|------------------|-----------------------------------------|
| name     | String           | The player's name                       |
| gender   | String           | Either `"boy"` or `"girl"`              |
| bag      | Array of [BagItem](#bagitem) | The items the player has                |
| money    | Numeric          | How much money the player has           |
| position | [Position](#position) | The position of the player in the world |

## `Character`

Contains information for a trainer or NPC. The type is `Numeric` and is the character's entity id.
The following properties are also available.

| Name     | Type     | Description                                        |
|----------|----------|----------------------------------------------------|
| name     | String   | The character's name                               |
| talkedTo | Bool     | True if the player has talked to them              |
| defeated | Bool     | True if defeated in battle. Always false for NPC's |
| position | [Position](#position) | The position of the player in the world            |
