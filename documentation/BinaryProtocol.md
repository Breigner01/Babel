# Binary Protocol

Values     | byte | type |
---        | --- | --- |
magicValue | 4 | uint32 |
type       | 1 | uint8 |
id         | 4 | uint32 |
size       | 4 | uint32 |

## Defined Values

### Magic Value

Value that allows to recognize if we receive a good packet as this value is set to **0x42dead42**.

### Type

A simple value to know the type of message sent or received.

Type     | value | Description |
---        | --- | --- |
IP         | 0 | Send IP |
Song       | 1 | Send compress audio  |
Text       | 2 | Send text |
Call       | 3 | Ask for a call |
EndCall    | 4 | End a call |
Contacts   | 5 | Ask the contacts list |

### Id

A value that increments to know where you are in the packets.
When answering, the same id will be returned.

### Size

The payload size. For knowing the size of packet without the header.