# Conversation Scripts

Conversation scripts are the type of script invoked from a character conversation. The following additional functions are available in conversation
scripts and any scripts spawned from conversation scripts

## Built-in Functions

| Name                 | Parameters | Return  | Description                                                           |
|----------------------|------------|---------|-----------------------------------------------------------------------|
| talkingEntity        | N/A        | Numeric | Returns the id of the NPC or trainer being talked to                  |
| conversationOver     | N/A        | Bool    | Returns true if the conversation is complete, false if still going on |
| waitConversationOver | N/A        | N/A     | Blocks until the conversation that started this script is complete    |
