# pamanager
a pulse audio device manager

Libpamanger provides an easy-to-use singleton instance to abstract the asynchronous pulse audio API.
It can be quickly added as a git submodule.
Alternatively, the files from lib/src can directly be copied into your code.
Your audiophile app can then retrieve information about sound devices and control them. 
Some parts of the project are modeled after [the pulseaudio C tutorial of Jan Newmarch](https://jan.newmarch.name/LinuxSound/Sampled/PulseAudio/#heading_toc_j_17), 
however, this project is written in C++. 

The device manager
 * provides a list of sound devices found at startup or added/removed at runtime
 * can switch between devices
 * can retrieve the active device
 * can get/set the volume
 * runs in a separate thread
 <br>
 Libpamanger allows to register callback functions to alert the end-user application about events of the audio system.<br>
 All definitions and declarations of this library occupy a dedicated namespace.<br>
 <br>
Dependencies: The project links against libpulse. premake5 is required. Ubuntu users can get premake5 from my Launchpad with <br>
"sudo add-apt-repository ppa:beauman/marley && sudo apt-get update"<br>
<br>
Ubuntu: sudo apt install premake5 git build-essential libpulse-dev<br>
<br>
Generate project: premake5 gmake2<br>
<br>
Build release (silent operation): make config=release verbose=1 <br>
Build debug (verbose): make config=debug verbose=1 <br>
<br>
If you're looking for more resources on libpulse / pulse audio, there is a similar project (only as command line tool and probably way more advanced) at https://github.com/cdemoulins/pamixer.
