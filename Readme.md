LD33 TD
=======

Built on the theme "You Are the Monster"


Goal
----

You have to defend your corporation from waves of angry customers, as levels progress you will be awarded more and more lethal weaponry to do this.

Go out and Be the Monster!


Tech
----

Built on C++11 with CMake, SFML, and my own [Kunlaboro](https://github.com/ace13/Kunlaboro) library.


Compiling
---------

Clone the library, use CMake to generate solutions for your preferred platform, then build the LD33_Game executable.

On Linux this might look like:
```
$ mkdir Build
$ cd Build
$ cmake ..
$ cd ..
$ make -C Build -j4 LD33_Game
$ Build/bin/LD33_Game
```