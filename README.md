# SC Hack Proof-of-Concept

Before going any further, you should know and accept that the code for this is bad. Not just what-the-cat-coughed-up bad. No... this is as if Cerberus himself reared back and spewed forth from the depths of hell an unholy rancid torrent of noxious bile thick and fast enough to flood the streets and sweep away a herd of wild horses. 

This is throw-away code. It was hacked together over the course of a few days simply as something to use while figuring out the [Nuklear GUI Toolkit](https://github.com/vurtun/nuklear). This project was never intended to be used for anything beyond that, and I'm fairly confident you don't want to use it for anything else either. You probably just want to download the appropriate binary for your system and run the demo. 

That said, it s licensed public domain, so if you're a masochist looking to loose a little faith in the future of developers, feel free...

## Overview
The purpose of this demo is to showcase one idea for a pseudo-feasible method of in-game hacking for the game [Star Citizen](https://robertsspaceindustries.com/). It is neither complete, nor a full game, or even demo. The general idea is that instead of playing some unrelated puzzle mini-game that has little to do with either hacking or the rest of the game, as happens so often in video games, the player needs to discover the appropriate frequency hopping pattern used by various ship's and component's radios. [Frequency-hopping](https://en.wikipedia.org/wiki/Frequency-hopping_spread_spectrum) is an actual method used to resist jamming, avoid interception, and reduce signal interference. This demo shows that with a little knowledge of what is being sent and the appropriate tools, decoding this pattern is not only possible, but lends itself to a number of other necessary game play elements either in or planned for Star Citizen. For a full write-up on the proposed hacking mechanic, see the files under the docs/ directory. 

An overly simplified use case for this tool would be as follows:
* Select the target (victim) ship firmware
* Start capturing packets
* Adjust the number of frequency-hops, number of bytes per hop values, and first-byte offset values.  
* Using the decoded output, the raw data, and the visual signal frequency chart, adjust the sliders for each frequency hop until the decoded message comes out clear and the packet data can be viewed. 
* Select an appropriate exploit and payload, and launch the attack. 

This demo only demonstrates this frequency-hopping pattern discovery. Other aspects of hacking game play can be found in the full write-up. The demo also will __always__ truncate the first packet which, while not necessarily realistic, demonstrates that getting a lock on the target radio is still possible in even the hardest scenarios.

For the purposes of this demo, the number of bytes per hop, number of hops, frequencies for each hop, and first packet offset are all randomly generated. If assistance is needed, these values are printed to the console, but I have yet to come across a combination that wasn't solvable. (Although there are certain combinations that can be quite tricky). The contents of the packets are hard-coded for each manufacturer.

## Known Issues 
Resizing the window on Windows systems causes the program to crash. As I am neither a game developer, or a Windows programmer, I choose to assume this is due to a problem in the Nuklear direct 3D implementation found in nuklear_d3d9.h. Resizing the window works as expected on Linux.

## Building
Developed primarily for 64-bit Linux running an X11 window manager. Development was done on Linux Mint 18.3 and XFCE with gcc 5.4.0. The windows builds were done on Windows 7 (64 bit).

### Linux
make

### Windows (cygwin)
This was an old version of cygwin, long in need of an update. I think the version used is 2.774. 

gcc main_win.c -I. -lgdi32 -l d3d9 -o bin/schack_cygwin.exe

### Windows (Visual Studio 2017 Community)
You may need to edit build.bat in case your installation is somewhere other than the default location or you're using a different version of VS. Once sorted, simply run:

build.bat

This links against directX 9, or d3d9.lib and user32.lib. 

## License
```
------------------------------------------------------------------------------
Public Domain (www.unlicense.org)
This is free and unencumbered software released into the public domain.
Anyone is free to copy, modify, publish, use, compile, sell, or distribute this
software, either in source code form or as a compiled binary, for any purpose,
commercial or non-commercial, and by any means.
In jurisdictions that recognize copyright laws, the author or authors of this
software dedicate any and all copyright interest in the software to the public
domain. We make this dedication for the benefit of the public at large and to
the detriment of our heirs and successors. We intend this dedication to be an
overt act of relinquishment in perpetuity of all present and future rights to
this software under copyright law.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
-----------------------------------------------------------------------------
```
