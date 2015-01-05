jm2cv : Jack Midi to Control Voltage
====================================

This repo is a mirror of http://git.fuzzle.org/jm2cv.git

Intro
-----
This tool allows converting JACK MIDI signals into JACK audio signals.

This tool was created for use with 
[non-mixer](http://non.tuxfamily.org/wiki/Non%20Mixer), to allow MIDI  controller mapping to non-mixer controls. The [non-mixer manual](http://non.tuxfamily.org/mixer/doc/MANUAL.html) has the details, specifically the section on [control voltages](http://non.tuxfamily.org/mixer/doc/MANUAL.html#n:1.2.3.1.4.1.).

Build & Usage
-------------
To get jm2cv set up on you machine, the following steps are neccessary.
```
git clone https://github.com/harryhaaren/jm2cv jm2cv
cd jm2cv
mkdir build
cd build
cmake ..
make
make install
```

Copyright / Authors
-------------------
jm2cv is copyright (c) 2010 Peter Nelson.

This software is licensed under the GNU General Public License version 2.
Please see the included file GPL-2 for full license terms.

I (Harry van Haaren) updated the build system to allow installing, and created
this README for easier build / usage instructions. I am not the author of any
of the code of this project.

Contact
-------
As no email of Peter Nelson is available (to my knowledge: do you have it? Get in
contact with me please!), any issues can be directed to this github reposity,
[https://github.com/harryhaaren/jm2cv/issues/new](https://github.com/harryhaaren/jm2cv/issues/new).

Cheers, -Harry
