# pamanager
### A PulseAudio Device Manager

Libpamanger provides an easy-to-use singleton to abstract the asynchronous API of PulseAudio.
It can be quickly added as a git submodule to any app in need of access to the PulseAudio demon.
Your app can then retrieve information about sound devices and control them.
Some parts of the project are modeled after [the PulseAudio C tutorial of Jan Newmarch](https://jan.newmarch.name/LinuxSound/Sampled/PulseAudio/#heading_toc_j_17), 
however, this project is written in C++. 

The device manager
 * provides a list of sound devices found at startup or added/removed at runtime
 * can switch between devices
 * can retrieve the active device
 * can get/set the volume
 * runs in a separate thread
 <br>
 Libpamanger allows to register callback functions to alert the end-user application about changes in the audio system.<br>
 <br>
 
### Dependencies
The project links against libpulse. premake5 is required. Ubuntu users can get premake5 from my Launchpad with <br>
"sudo add-apt-repository ppa:beauman/marley && sudo apt-get update"<br>
<br>
Ubuntu: sudo apt install premake5 git build-essential libpulse-dev<br>
<br>
### Generate and build project
premake5 gmake2<br>
<br>
Build release (silent operation): make config=release verbose=1 <br>
Build debug (verbose): make config=debug verbose=1 <br>
<br>
### Resources
If you're looking for more resources on libpulse / pulse audio, there is a similar project (only as command line tool and probably way more advanced) at https://github.com/cdemoulins/pamixer.
