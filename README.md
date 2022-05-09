# Disco.c, a Discord C bot library

### File explanations

- libs/web/**websocket**: Contains the required methods to abstract the background websocket connection.
- libs/web/**gateway**: Handles the websocket connection to the Discord Gateway. Making sure the heartbeat is regularly sent and the incoming responses are all properly handled.
- libs/web/**request**: Helper file that abstracts the HTTP GET and POST requests.
- libs/utils/**cJSON**: External library linked below that is used to work with JSON formats.


required libs:
- https://libwebsockets.org/
- https://curl.se/libcurl/c/
- https://github.com/DaveGamble/cJSON/