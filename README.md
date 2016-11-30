# overlay64 - video overlay module

*This project is currently in early beta status.*

The overlay64 module allows superimposing predefined texts onto an
existing composite signal or onto the luminance portion of an s-video
signal depending on the states of up to sixteen digital input
lines. An LM1881 video sync separator IC is used to detect the
vertical and horizontal sync pulses of the video signal. An Atmega1284
uses these signals to synchronize with the video signal and to
superimpose text on the video display by bit-banging the corresponding
characters over SPI.

The user writes a simple configuration file that determines which
texts are to be displayed for which input line states. The
configuration file is converted to a binary format using the supplied
commandline tool before flashing it to the Atmegas's eeprom memory via
the USB interface.

Possible uses include displaying the state of control lines for
additional hardware installed in a homecomputer such as the Commodore
64, for example to display the currently selected kernal based on the
state of the control lines for a multi-kernal adapter.

An early working prototype can be seen in [this
video](https://www.youtube.com/watch?v=0exp2yM0WHM), where it displays
the runtime configuration of the MixSID Stereo SID board.

This repository includes the complete sourcecode of the client
software, the firmware and all design files in KiCAD format. The
project is licensed under the GPLv3.
