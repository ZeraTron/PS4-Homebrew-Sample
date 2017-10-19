# PS4 Homebrew Sample
1. **[Preview](#1-preview-)**
2. **[How to compile / run](#2-how-to-compile--run-)**
3. **[Credits](#3-thanks-to-)**
4. **[Contact](#4-contact-)**
5. **[Known bugs / to do](#5-known-bugs--to-do-)**
***
## Information :
This is still a WIP, and the code may be bugged or not working. I'll keep update this repo to provide the best code I can.
***
### __1. Preview :__
Showcase #01: https://www.youtube.com/watch?v=Fy1leri9apk

Showcase #02: https://www.youtube.com/watch?v=nw2FN0wzgzA

Latest Sneak Peak: https://twitter.com/ZeraTron_/status/921129847958790145
***
### __2. How to compile / run :__

#### Prequisite:

*  BigBoss fork [ps4sdk](https://github.com/psxdev/ps4sdk)
*  TheoryWrong fork [libOrbis](https://github.com/theorywrong/liborbis)


1. Install and build both *ps4sdk* & *libOrbis*
2. Put the *HomebrewLauncher* folder in your *libOrbis* folder
3. Build the payload, put it on a usb stick in a backup folder named **CUSAXXXXX** and plug in *usb0*
4. Send the *kernel_hooks* with the elf-loader
5. Launch Playroom, you should boot up in the homebrew

***
### __3. Thanks to :__
* ***BigBoss***, for the basic libOrbis, and all his work.
* ***TheoryWrong***, for his help improving libOrbis.
* ***Zecoxao***, for providing me the kernel_hooks and eboot_plugin source code.
* ***All the scene devs for their awesome works !***
***
### __4. Contact :__
* __E-mail:__ *zeratron@yahoo.com*
* __Twitter:__ *[@ZeraTron_](https://twitter.com/ZeraTron_)*
***
### __5. Known Bugs / To Do :__
  * __Known Bugs:__
    * A lot of stuffs are not working like expected, you get a lot of warnings when compiling etc...


  * __To Do:__
    * Fix and clean the code.
    * Add support for png decoding and rendering.
    * Add sound support.
