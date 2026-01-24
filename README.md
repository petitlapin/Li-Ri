# Li-Ri is a G.P.L. arcade game about toy wood engine
## Description
Li-Ri is a fork of Ri-li (http://www.ri-li.org).

Li-Ri is a arcade game licensed under the G.P.L. (General Public License).
You drive a toy wood engine in many levels and you must collect all the wagons on map for win. This game is free, and you can download it on 'Windows', 'Linux', 'Mac OsX', 'Android'.

## Features
Full-featured,
- 19 languages: Arabic, Breton, Chinese, English, Esperanto, French,
  German, Italian, Japanese, Korean, Portuguese, Russian, Slovak,
  Spanish, Swedish, Polish, Turkish, Hungarian, Dutch.
- Colorful animated wood engine.
- 50 levels in this first version
- 3 beautiful musics and many sound effects.

This version updates the version to the SDL2, adds cmake build system and updates the code to build and run on Android too.

## Screenshots
![Main menu](fastlane/metadata/android/en-US/images/sevenInchScreenshots/01.png)
Main menu

![Gameplay](fastlane/metadata/android/en-US/images/sevenInchScreenshots/03.png)
Gameplay

## Building
To build this game you need to clone repository using `git clone https://github.com/petitlapin/Li-Ri.git`, install SDL 2, and execute:
```
cd Li-Ri
mkdir build
cd build
cmake ..
make
```

Done :)

-------------

Copyright (c) 2023
Johnny Jazeix <jazeix@gmail.com>: port to SDL2 + android + cmake

Copyright (c) 2006
Dominique Roux-Serret: roux-serret@ifrance.com : design & programming & graphics & website.
Maf464 : site= http://maf464.free.fr : musics

[<img src="https://fdroid.gitlab.io/artwork/badge/get-it-on.png"
     alt="Get it on F-Droid"
     height="80">](https://f-droid.org/packages/org.liri.liri/)
