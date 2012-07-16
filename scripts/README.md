Quick Inventory:
----------------

TypeWronger is the arduino script. It is completely undocumented
but you can probably figure out how to tweak the timings. I'd leave
the carriage return timings alone if possible. Don't remove too many
of the key delays; too fast and keys will start jamming.

Communicate with the typewriter over the serial port at 19200 bps.
The easiest way to test is with gnu screen:
screen /dev/ttyUSB0 19200

What you type should go directly to the typewriter.


