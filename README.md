Li-Ri is a fork of Ri-li (http://www.ri-li.org).

Li-Ri is a arcade game licensed under the GPL (General Public License).
You drive a toy wood engine in many levels and you must collect
all the coaches for win. You can download it and play indefinitely.
It's Free. You can play on 'Windows', 'Linux', 'Mac OsX', 'Android'.

Full-featured,
- 19 languages: Arabic, Breton, Chinese, English, Esperanto, French,
  German, Italian, Japanese, Korean, Portuguese, Russian, Slovak,
  Spanish, Swedish, Polish, Turkish, Hungarian, Dutch.
- Colorful animated wood engine.
- 50 levels in this first version
- 3 beautiful musics and many sound effects.

![Main menu](fastlane/metadata/android/en-US/images/sevenInchScreenshots/01.png)

![Gameplay](fastlane/metadata/android/en-US/images/sevenInchScreenshots/03.png)

This version updates the version to the SDL2 and updates the code to build and run on Android too.

[<img src="https://fdroid.gitlab.io/artwork/badge/get-it-on.png"
     alt="Get it on F-Droid"
     height="80">](https://f-droid.org/packages/org.liri.liri/)


Create the pot file:
xgettext --copyright-holder="This file is copyright:" --package-name=Li-ri --msgid-bugs-address=https://bugs.kde.org --from-code=UTF-8 -C src/*.cc src/*.h -o liri.pot
