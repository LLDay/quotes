# Quotes
Client-server system that allows to add, delete and view asset quotes.

# Protocol
Protocol's data structures are described using [Google Protobuf](https://developers.google.com/protocol-buffers/docs/overview) in [quotes.proto](quotes.proto).

There are three types of queries: `ADD`, `DELETE` and `GET`. The client
sends a query with the type `ADD` to add a new asset or/and new history
points for the asset. A `DELETE` query deletes an asset with its history.
A `GET` query allows to access to history points of an asset.

## Format

Packet = `SOURCE, TYPE, [ASSET(NAME, [HISTORY_POINT(TIME, VALUE), [...]]), [...]]`

Square brackets mean an optional field. Ellipsis means a variadic number of fields.
Parentheses mean arguments of a field.


## Add

There is no difference in request between creating a new asset and adding new points.
The server repeats all assets and all unique by time fields from a client's request.

Query (`time1 != time2`)

`SOURCE(CLIENT), TYPE(ADD), ASSET(name="ASSET", history=[HP(time1, value1), HP(time2, value2)])`

The server creates a new asset, if the asset didn't exist, and answer

`SOURCE(SERVER), TYPE(ADD), ASSET(name="ASSET", history=[HP(time1, value1), HP(time2, value2)])`

## Delete

All history points are ignored. Clients send a list of assets to delete. The server returns
a list of assets that wasn't deleted because of error name.

Query:

`SOURCE(CLIENT), TYPE(DELETE), ASSET(name="ASSET", history=NULL), ASSET(name="WRONG_NAME_ASSET", history=NULL)`

Answer:

`SOURCE(SERVER), TYPE(DELETE), ASSET(name="WRONG_NAME_ASSET", history=NULL)`

## Get

There are three ways to get asset's history
1. An asset without history points. The server returns all asset's history.
1. An asset with one history point `(time, value)`. The server returns the last `value` points of asset's history. If the points number
is less than `value`, the server returns all history points.
1. An asset with two history points `(time1, value1), (time2, value2)`. The server returns asset's history points with time in `[time1, time2]`.

Query:

`SOURCE(CLIENT), TYPE(GET), ASSET(name="ASSET", history=NULL)`

Answer:

`SOURCE(SERVER), TYPE(GET), ASSET(name="ASSET", history=[HP, ...])`


Query:

`SOURCE(CLIENT), TYPE(GET), ASSET(name="ASSET", history=HP(time=ANY, value=2)`

Answer:

`SOURCE(SERVER), TYPE(GET), ASSET(name="ASSET", history=[HP, HP])`


Query:

`SOURCE(CLIENT), TYPE(GET), ASSET(name="ASSET", history=[HP(time=from, value=ANY), HP(time=to, value=ANY)]`

Answer:

`SOURCE(SERVER), TYPE(GET), ASSET(name="ASSET", history=[HP, ...])`


## Asterisk
Client can use the asterisk sign "\*" as an asset name to add history points to all assets, to delete all assets or get all assets.

# Server
The server allows multiple clients to perform operations with assets.
It allows to save assets on close and load them back on execution.

## Build
There are two ways to build the server:
```bash
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
cmake --build -j4 .
```
or
```bash
docker-compose up
```

Executable is located in /build/bin/
## Options
```
-h [ --help ]         show help message
-i [ --ip ] arg       specify ip address (default 127.0.0.1)
-t [ --threads ] arg  number of threads (default 4)
-p [ --port ] arg     specify port number (default 1100)
-d [ --data ] arg     path to file with saved data (default "assets.data")
-n [ --no-data ]      suppress loading/storing data
```
## Stopping
To stop the server, send it SIGINT using `^C` or `kill`.

# Client

## Options
```
-h [ --help ]         show help message
-i [ --ip ] str       ip adress for connection (default = 127.0.0.1)
-p [ --port ] int     port adress for connection (default = 1100)
```
## Stopping
To stop the server, send it SIGINT using `^C`.
