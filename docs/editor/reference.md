# Peoplemon Editor Reference

While the editor aspires to be mostly intuitive this document is here to explain some of the higher level concepts
of the Peoplemon editing process.

## Maps

The game world of Peoplemon is comprised of a collection of tile maps. The main map is `Worldmap`, which is loaded in the editor by default. Each map has several intrinsic properties such as:
- Name
- Default playlist
- Default weather
- Enter/Exit scripts
- Ambient light settings

### Levels & Layers
Maps are made up of a stack of levels, which can be thought of as elevation. Each level itself is a stack of layers, and each layer is a two-dimensional grid of tiles, as well as some special layers. There are five layer types:
- **Bottom tiles**: These tiles are rendered underneath all other tiles and entities on the same level
- **y-sorted tiles**: These tiles are rendered inter-woven with entities and each other. Think walking in-front of a house then behind it.
- **Top tiles**: These tiles are rendered on top of all other tiles and entities
- **Collisions**: Collision tiles are not rendered, but they dictate whether or not an entity is allowed to move onto a tile or not
- **Catchables**: These tiles indicate if wild Peoplemon appear on the tile or not. Catch tiles are associated with a catch region, which is a collection of wild Peoplemon templates and their relative frequencies.

### Towns

In addition to the levels and layers, maps also have a single extra layer of town tiles. Town tiles associate a tile position with a town. A town has its own music, local PC center, and weather. "Town" may refer to routes and other regions as well, and is not actually specific to towns or cities. If a tile is not in a town then the map's default music and weather are used.

### Lighting

Peoplemon has a day/night cycle which lasts for approximately 20 minutes. Maps may have their ambient lighting adjust based on this cycle, or may instead have a fixed level of ambient lighting. Fixed lighting is ideal for inside buildings or caves, while outside maps should generally allow the engine to cycle the light level based on time of day. 

In addition to the ambient lighting, light sources may also be placed on the map. Light sources are rendered on top of everything and have a radius in pixels. Light sources are only rendered when the ambient light level drops below a certain point. In the editor the time of day may be manually set to test lighting, but this setting has no bearing on the game or map itself.

### Entities

Maps also store information about the entities which spawn by default in them. Items, NPC's, and trainers may all be placed from the editor. The currently selected level is used to determine the spawn position, so care must be taken if a map has more than one level.

### Spawns

Spawns are for when the player enters a map. When entering a map the player may either spawn at a designated spawn, or may spawn at the position they were last at (such as coming back out from a house). Player spawns are level-specific like entity spawns so the same care must be taken.

### Scripts & Events

When a map is entered or exited scripts may be ran. See the [scripting reference](../scripts/reference.md) for more on how to write scripts. These scripts are ran on the main game thread, so they must be fast and not block on time or user input. In addition to the enter and exit scripts maps also contain events. Events are rectangular regions that run a script based on their trigger. Event triggers are:
- **OnEnter**: Runs exactly once when an entity first enters the event region
- **OnExit**: Runs exactly once when an entity exits the event region
- **OnEnterOrExit**: Runs once when the entity enters and again when the entity leaveas
- **WhileIn**: Runs once per frame while an entity is within the region
- **OnInteract**: Runs if the entity performs an interaction while within the region

Event scripts are ran on the main game thread and must not block on time or user input. Event regions are *not* level-specific.

## Entities

### NPC's

NPC's in Peoplemon have a small set of properties that give them life:
- **Name**: May be displayed by the game and may be used by scripts to reference the entity
- **Animation**: A move animation which is a folder comprised of 4 animations (`up.anim`, `right.anim`, `down.anim`, `left.anim`)
- **Conversation**: What the NPC says. More on that below
- **Behavior**: Dictates how the NPC moves around. More on that below

### Trainers

Trainers are the same as NPC's, but have the addition of a team of Peoplemon and some extra properties:
- **Vision range**: How far away the trainer will see the player from, in tiles
- **Before battle conversation**: What the trainer says before they initiate battle
- **Lose line**: Single line of text that the trainer says on the battle screen when they lose
- **After battle conversation**: What the trainer says in the map view after they lose. This is also used if the player talks to them again
- **Items**: List of items the trainer may use in battle
- **Peoplemon**: The team of Peoplemon the trainer uses in battle

### Behaviors

NPC's and trainers are controlled by one of 4 behaviors:
- **Standstill**: Does not move
- **Spin**: Stands in place and looks in different directions
- **Path**: Follows a pre-defined path
- **Wander**: Randomly walks within a given tile radius

### Conversations

Conversations are implemented as a graph of nodes. This graph will generally resemble a tree for most conversations. The game engine will traverse the conversation graph in a relatively intuitive manner, following the node transitions as indicated. Conversations may display dialog, ask questions, give or take money, give or take items, and run scripts. The flow of conversations may be dicted by the answers to questions, result of taking an item or money, the status of whether or not the given entity was already talked to, and even by custom flags which may be set in conversations or from scripts. This all allows for a flexible and robust interaction system.

## Testing

### From The Editor

TODO: Document this when implemented

### In Game

From within Peoplemon the game may be tested with several of the following tools:
- Press `F1` to switch to a free camera which may be used to explore the current map
- Press `~` to enable the entry of arbitrary scripts which may be ran to query or modify the game state

The above are only available in the build of Peoplemon that is bundled with the editor.
