# pa
a pulse audio device manager

This project is modeled after [the pulseaudio C tutorial of Jan Newmarch](https://jan.newmarch.name/LinuxSound/Sampled/PulseAudio/#heading_toc_j_17), 
however, it is written in C++. The device manager provides a list a sound output devices and can switch between them.
It tracks devices found at startup or added/removed at runtime. It runs in a separate thread.
The project links against libpulse and libpthread.
