# pa
a pulse audio device manager

Some parts of the project are modeled after [the pulseaudio C tutorial of Jan Newmarch](https://jan.newmarch.name/LinuxSound/Sampled/PulseAudio/#heading_toc_j_17), 
however, this project is written in C++. 

The device manager
 * provides a list of sound devices found at startup or added/removed at runtime
 * can switch between devices
 * can retrieve the active device
 * set the volume
 * runs in a separate thread
 <br>
 All files required for the static library of the device manager can be found under lib/src.<br>
 <br>
Dependencies: The project links against libpulse. premake5 is required. Ubuntu users can get premake5 from my Launchpad with <br>
"sudo add-apt-repository ppa:beauman/marley && sudo apt-get update"<br>
<br>
Ubuntu: sudo apt install premake5 git build-essential libpulse-dev<br>
<br>
Build: premake5 gmake2 && make config=release verbose=1 <br>
<br>
If you're looking for more resources on libpulse / pulse audio, there is a similar project (only as command line tool and probably way more advanced) at https://github.com/cdemoulins/pamixer.
