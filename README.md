FreeSerf Studio
===============

A resource browser and exporter for [Freeserf](https://github.com/freeserf/freeserf),
a remake of the original Settlers game. It reads the data files of the original
game (DOS or Amiga version) and Freeserf custom data, and shows sprites,
animations and sounds side by side for every source. Resources can be saved or
copied one by one, or exported in bulk to the Freeserf custom data format.

Build status
------------
[![Windows Build Status](https://github.com/freeserf/fsstudio/actions/workflows/windows.yml/badge.svg)](https://github.com/freeserf/fsstudio/actions/workflows/windows.yml)
[![Linux Build Status](https://github.com/freeserf/fsstudio/actions/workflows/linux.yml/badge.svg)](https://github.com/freeserf/fsstudio/actions/workflows/linux.yml)
[![macOS Build Status](https://github.com/freeserf/fsstudio/actions/workflows/macos.yml/badge.svg)](https://github.com/freeserf/fsstudio/actions/workflows/macos.yml)

Last build binaries
-------------------
Packages for every build are attached as artifacts to the workflow runs
(a GitHub login is required to download them):

* [Windows x64](https://github.com/freeserf/fsstudio/actions/workflows/windows.yml?query=branch%3Amaster) – NSIS installer
* [macOS](https://github.com/freeserf/fsstudio/actions/workflows/macos.yml?query=branch%3Amaster) – DMG
* [Linux](https://github.com/freeserf/fsstudio/actions/workflows/linux.yml?query=branch%3Amaster) – tarball

Game data
---------
By default FSStudio looks for the game data in `~/.local/share/freeserf`:

* DOS data file `spae.pa` (or `SPAD.PA`, `SPAF.PA`, `SPAU.PA` depending on the
  language of the game);
* Amiga files `gfxheader`, `gfxfast`, `gfxchip`, `gfxpics`, `sounds`, `music`;
* Freeserf custom data in the `custom` subdirectory.

Other locations can be chosen in *File → Sources*. Sources that are found are
shown side by side; each one can be hidden in the *View* menu.

Building
--------
FSStudio is built with CMake and requires Qt 6 (Core, Gui, Widgets and
Multimedia) and a C++17 compiler. Freeserf sources are used as a git
submodule:

```
git clone --recursive https://github.com/freeserf/fsstudio.git
cd fsstudio
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

If CMake can't find Qt, pass its location, e.g.
`-DCMAKE_PREFIX_PATH=/opt/homebrew/opt/qt` for Homebrew on macOS or
`-DCMAKE_PREFIX_PATH=C:/Qt/6.8.3/msvc2022_64` on Windows.

To build a redistributable package (NSIS installer on Windows, DMG on macOS,
tarball on Linux):

```
cd build
cpack
```

On Windows and macOS the Qt libraries and plugins are bundled with the
application at this step (`windeployqt` / `macdeployqt`). Building the Windows
installer requires [NSIS](https://nsis.sourceforge.io).

Known limitations
-----------------
* MIDI music can't be played: Qt Multimedia does not decode MIDI. Sound
  effects play normally.
