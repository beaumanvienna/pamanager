# pa
pulse audio device manager

This project is modeled after [this pulsedaio C tutorial ](https://jan.newmarch.name/LinuxSound/Sampled/PulseAudio/#heading_toc_j_17), 
however, it is written in C++. The device manager provides a list a sound output devices aka sinks. 
It tracks devices found at startup or added/removed at runtime. It runs in a separate thread.
It links against libpulse and libpthread.
