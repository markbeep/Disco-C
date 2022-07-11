# Disco.c, an Async Discord C Bot Library
Disco-C is a part-time project to practice writing bigger projects in C and getting more experience in multiple directions (requests, websockets, APIs, etc.). It is also a nice way to learn about how Discord really works.

The library is built to work async, which means that every received event is handled in its own thread. The idea behind this is, that in a single function you want to work sequentially, but you also don't want functions like sending a message (which does a HTTP request that takes around ~200ms) block the whole bot from receiving anything anymore in that time. Another option would have been to make the HTTP request itself be async, but this then doesn't give you the guarantee that a message has been sent once the message-send function is done.

## Table of Contents
1. [Installing](#installing)
    1. [Copy Paste Command](#copy-paste-command)
    2. [Required Libraries](#required-libraries)
2. [File Explanations](#file-explanations)
3. [`Disco` vs `Discord`](#why-are-some-things-called-disco_-while-others-are-called-discord_)
5. [Progress](#progress)

## Installing
### Copy Paste Command
Use `git clone --recurse-submodules git@github.com:markbeep/Disco-C.git` to clone the submodules as well.

### Required Libraries:
- https://libwebsockets.org/
  - `sudo apt install libwebsockets-dev`
- https://curl.se/libcurl/c/
  - `sudo apt install libcurl4-gnutls-dev`
- https://github.com/DaveGamble/cJSON/
- https://github.com/sheredom/hashmap.h/

## File Explanations
- src/web/**websocket**: Contains the required methods to abstract the background websocket connection.
- src/web/**gateway**: Handles the websocket connection to the Discord Gateway. Making sure the heartbeat is regularly sent and the incoming responses are all properly handled.
- src/web/**request**: Helper file that abstracts the HTTP GET/POST/PATCH/etc. requests.
- src/utils
  - **cache**: The cache uses the `hashmap.h` library to create a linked hashmap for messages, channels and guilds.
  - **disco_logging**: Personalized logging library which can be used to log messages to the console. The file path is additionally added to each message.
  - **prio_queue**: O(n) lazy priority queue implementation. Currently there's no use for this anymore.
  - **t_pool**: Thread pool library to manage the multiple threads which will then be distributed for each fired event.
- src/discord/
  - **disco**: The main functions of Disco-C. It contains the highest level of abstraction for running the bot.
  - **event**: Handles received Discord events.
  - **structures/**: All of the Discord structure definitions.
  - Structure specific functions are all defined in their corresponding source files.
- **example**/: Example implementations of how the bot syntax looks like.
- **lib**/: External libraries not downloadable with the APT package manager (basically just Github libraries). Will be renamed to `include` in the future to maintain the Pitchfork standard.

## Why are some things called `disco_...` while others are called `discord_...`?
*(In the future everything will be called `discord_` for consistency)*

All of the structures that are taken straight from the [Discord documentation](https://discord.com/developers/docs/) are all named with a leading `discord_`. At the structure definition there should also be a link leading to the exact place in the documentation that the definition was taken from.

On the other hand, everything starting with `disco_` is a public library method that can be used at the top level to control the bot.

## Progress
- Interactions
  - [x] Application Command Object
  - [x] Application Command Option Object
  - [x] Register Commands
    - [x] Global
    - [x] Guild
- API Calls
  - [x] Send messages
    - [x] Content
    - [x] Embeds
    - [ ] Components (untested)
  - ... (no complete list of methods yet)
- Events ![](https://progress-bar.dev/14/?title=8/57)
  - [x] READY
  - [ ] RESUMED
  - [ ] RECONNECT
  - [ ] INVALID_SESSION
  - [ ] APPLICATION_COMMAND_PERMISSIONS_UPDATE
  - [x] CHANNEL_CREATE
  - [x] CHANNEL_UPDATE
  - [x] CHANNEL_DELETE
  - [ ] CHANNEL_PINS_UPDATE
  - [ ] THREAD_CREATE
  - [ ] THREAD_UPDATE
  - [ ] THREAD_DELETE
  - [ ] THREAD_LIST_SYNC
  - [ ] THREAD_MEMBER_UPDATE
  - [ ] THREAD_MEMBERS_UPDATE
  - [ ] GUILD_CREATE
  - [ ] GUILD_UPDATE
  - [ ] GUILD_DELETE
  - [ ] GUILD_BAN_ADD
  - [ ] GUILD_BAN_REMOVE
  - [ ] GUILD_EMOJIS_UPDATE
  - [ ] GUILD_STICKERS_UPDATE
  - [ ] GUILD_INTEGRATIONS_UPDATE
  - [ ] GUILD_MEMBER_ADD
  - [ ] GUILD_MEMBER_REMOVE
  - [ ] GUILD_MEMBER_UPDATE
  - [ ] GUILD_MEMBERS_CHUNK
  - [ ] GUILD_ROLE_CREATE
  - [ ] GUILD_ROLE_UPDATE
  - [ ] GUILD_ROLE_DELETE
  - [ ] GUILD_SCHEDULED_EVENT_CREATE
  - [ ] GUILD_SCHEDULED_EVENT_UPDATE
  - [ ] GUILD_SCHEDULED_EVENT_DELETE
  - [ ] GUILD_SCHEDULED_EVENT_USER_ADD
  - [ ] GUILD_SCHEDULED_EVENT_USER_REMOVE
  - [ ] INTEGRATION_CREATE
  - [ ] INTEGRATION_UPDATE
  - [ ] INTEGRATION_CREATE
  - [ ] INVITE_CREATE
  - [ ] INVITE_DELETE
  - [x] MESSAGE_CREATE
  - [x] MESSAGE_UPDATE
  - [x] MESSAGE_DELETE
  - [ ] MESSAGE_DELETE_BULK
  - [ ] MESSAGE_REACTION_ADD
  - [ ] MESSAGE_REACTION_REMOVE
  - [ ] MESSAGE_REACTION_REMOVE_ALL
  - [ ] MESSAGE_REACTION_REMOVE_EMOJI
  - [ ] PRESENCE_UPDATE
  - [ ] STAGE_INSTANCE_CREATE
  - [ ] STAGE_INSTANCE_DELETE
  - [ ] STAGE_INSTANCE_UPDATE
  - [ ] TYPING_START
  - [ ] USER_UPDATE
  - [ ] VOICE_STATE_UPDATE
  - [ ] VOICE_SERVER_UPDATE
  - [ ] WEBHOOKS_UPDATE
  - [x] INTERACTION_CREATE
  
