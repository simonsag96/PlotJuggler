# Foxglove Protocol Notes

This plugin targets the current Foxglove WebSocket protocol used by the Foxglove SDK and `foxglove_bridge`.

## Scope

- ROS 2 only
- message payload encoding: `cdr`
- schema encoding: `ros2msg`
- schema is required

## Handshake

- WebSocket subprotocol: `foxglove.sdk.v1`
- Qt5 client implementation uses `QWebSocket::open(const QNetworkRequest&)`
- The request must include:
  - `Sec-WebSocket-Protocol: foxglove.sdk.v1`

## Server To Client Text Messages

JSON messages are identified by the `op` field.

Relevant ops for this plugin:

- `serverInfo`
- `advertise`
- `unadvertise`
- `status`
- `removeStatus` is ignored in the current implementation

### `advertise`

Expected channel fields:

- `id`
- `topic`
- `encoding`
- `schemaName`
- `schema`
- `schemaEncoding`

This plugin accepts only channels where:

- `encoding == "cdr"`
- `schemaEncoding == "ros2msg"`
- `schema` is non-empty

For `ros2msg`, the schema is handled as UTF-8 text and forwarded to PlotJuggler's existing ROS 2 parser factory.

### `unadvertise`

Expected payload:

- `channelIds`

These channel IDs are removed from the available channel list. Active subscriptions tied to removed channels are dropped locally.

If all active subscriptions are removed, the plugin shuts the stream down instead of staying connected with no parsers.

## Client To Server Text Messages

### `subscribe`

Format:

```json
{
  "op": "subscribe",
  "subscriptions": [
    { "id": 1, "channelId": 10 }
  ]
}
```

- `id` is the client-generated subscription ID
- `channelId` is the server-advertised Foxglove channel ID

### `unsubscribe`

Format:

```json
{
  "op": "unsubscribe",
  "subscriptionIds": [1]
}
```

## Binary Message Frames

The plugin expects Foxglove `MessageData` frames:

- byte `0`: opcode
- bytes `1-4`: `subscriptionId` little-endian `uint32`
- bytes `5-12`: `log_time` little-endian `uint64` in nanoseconds
- bytes `13+`: serialized ROS 2 CDR payload

For the current implementation:

- only opcode `0x01` is handled
- `subscriptionId` is used to look up the parser created for the selected channel
- the remaining bytes are passed directly to the PlotJuggler `ros2msg` parser

## PlotJuggler Mapping

- Foxglove channel filtering happens before the dialog shows selectable entries
- one PlotJuggler parser is created per active subscription
- the parser factory key used is `ros2msg`
- incoming frame timestamps are used as the initial timestamp, and the parser can still override them with embedded ROS header timestamps if enabled
- `dataReceived()` emissions are coalesced on a short timer to avoid one UI update per packet

## Status Messages

Foxglove `status` messages use a numeric level enum:

- `0`: info
- `1`: warning
- `2`: error

This plugin surfaces warning and error messages to the user.

## Current Limitations

- no support for non-ROS 2 channels
- no support for `omgidl` / `ros2idl`
- pause is client-side only
- the implementation assumes Foxglove channel IDs are within the practical JSON integer range used by the bridge
