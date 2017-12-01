# The Ludum Dare 40 entry by @polsys

At least, there should be one someday. At the present, this is just preparation.

The game is written in C++14 using [SFML](https://github.com/SFML/SFML). C++ is not my strongest language, and that's exactly why I'm using it here!


# Building
## Windows
You'll need Visual Studio 2017 (VS2015 should work as well).
1. First, download/fork this repository.
2. You'll notice that the `Dependencies` folder is empty. Download [SFML for Visual Studio 2015 (32-bit)](https://www.sfml-dev.org/download/sfml/2.4.2/) and unpack the `SFML-2.4.2` folder there.
3. Open the solution in Visual Studio and build it. The project file includes some MSBuild magic to automatically copy binary dependencies and assets to the output directory when building.

## Linux
TBD.
