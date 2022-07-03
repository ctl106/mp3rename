Mp3rename 0.6


This program is for easily renaming all those bad named mp3 files.
So that they can be recognised.

Why?

Well I got really tired of typing mv ... ... all the time.

Installation:

make
make install should do the trick.

Usage:

It is recommended to first set your default look of the mp3 files.
Help on this can be get by mp3rename -s help
Just type mp3rename <filename> or mp3rename *.mp3
And it should rename all the files according to their id3 tag.
If there is no id3 tag given, it gives  a message that no id3 tag
is present, if the option -v is given it will ask you for the artists
name and song title for songs without id3tags.


Disclaimer:

I do not take responsebility for any damage caused by this program.
It shouldn't do any damage, but if it does, its your problem.


Todo:
* Fix inconsistent whitespace
* Seperate functionality into its own function
    * `main` should only handle initializing the app
    * A new function should be created for reading in a file, if one is not provided via stdin
    * A new function should be created to contain the renaming functionality
* Change flag variables to use `bool` instead of `int`
* Move flags set from command line arguments to a struct
* Create constant variables to replace magic numbers
* Move parsing of IDE3 V1 tags to its own file
* Add capability for detecting different metadata tag types
* Create a more modular and extensible format character system

Done:


Known bugs:

-

Sander Janssen 		<janssen@rendo.dekooi.nl>

