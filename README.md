# (Cursed) Lemonade Stand
*The Ludum Dare 40 entry of @polsys.*

Summer is at its best and lemonade is in huge demand. You have a week to sell as much lemonade as possible. The weather couldn't be nicer... but remember: *the more you have, the worse it is*...

The game is written in C++14 using [SFML](https://github.com/SFML/SFML). C++ is not my strongest language, and that's exactly why I'm using it here!

As a special surprise, `lemonade_stand.py` contains a rough command line version of the game! I used it to prototype the game rules. There is no parameter validation whatsoever and many things differ from the finished game.


# Building
## Windows
You'll need Visual Studio 2017 (VS2015 should work as well).
1. First, download/fork this repository.
2. You'll notice that the `Dependencies` folder is empty. Download [SFML for Visual Studio 2015 (32-bit)](https://www.sfml-dev.org/download/sfml/2.4.2/) and unpack the `SFML-2.4.2` folder there.
3. Open the solution in Visual Studio and build it. The project file includes some MSBuild magic to automatically copy binary dependencies and assets to the output directory when building.

## Linux
TBD.
