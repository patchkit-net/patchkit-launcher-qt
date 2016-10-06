# patchkit-launcher-qt
PatchKit launcher made in Qt.

## Requirements
* [Qt Framework](https://www.qt.io/download/)
* Microsoft Visual Studio (only for Windows)

### Building statically linked Qt

Following guide shows how to build statically linked Qt (version 5.7).

#### Windows (using Visual Studio envirnonment)

* Download [Qt Installer](https://www.qt.io/download-open-source/) and install Qt Creator
* Download [Qt Source](http://download.qt.io/official_releases/qt/5.7/5.7.0/single/) and unzip the package
* Modify and save envinonment preparation scripts (listed in next paragraph)
* Open `cmd.exe`
* `env_x86` for 32 bit
* `env_x64` for 64 bit
* `jom`
* `jom install`

**env_x86.bat**

``` batch
REM Configuration
SET _VS_ROOT="C:\Program Files (x86)\Microsoft Visual Studio 14.0"
SET _QT_SRC_ROOT=C:\Qt\5.7\msvc2015_static_32_src
SET _QT_OUTPUT_INSTALL_ROOT=C:\Qt\5.7\msvc2015_static_32
SET _QT_CREATOR_ROOT=C:\Qt\Tools\QtCreator

CALL %_VS_ROOT%\VC\vcvarsall.bat

SET PATH=%_QT_SRC_ROOT%\qtbase\bin;%_QT_SRC_ROOT%\gnuwin32\bin;%_QT_CREATOR_ROOT%\bin;%PATH%
SET QMAKESPEC=win32-msvc2015

configure -prefix %_QT_OUTPUT_INSTALL_ROOT% -release -opensource -static -static-runtime -nomake tests -nomake examples

SET _VS_ROOT=
SET _QT_SRC_ROOT=
SET _QT_OUTPUT_INSTALL_ROOT=
SET _QT_CREATOR_ROOT=
```

**env_x64.bat**

``` batch
REM Configuration
SET _VS_ROOT="C:\Program Files (x86)\Microsoft Visual Studio 14.0"
SET _QT_SRC_ROOT=C:\Qt\5.7\msvc2015_static_64_src
SET _QT_OUTPUT_INSTALL_ROOT=C:\Qt\5.7\msvc2015_static_64
SET _QT_CREATOR_ROOT=C:\Qt\Tools\QtCreator

CALL %_VS_ROOT%\VC\vcvarsall.bat x64

SET PATH=%_QT_SRC_ROOT%\qtbase\bin;%_QT_SRC_ROOT%\gnuwin32\bin;%_QT_CREATOR_ROOT%\bin;%PATH%
SET QMAKESPEC=win32-msvc2015

configure -prefix %_QT_OUTPUT_INSTALL_ROOT% -release -opensource -static -static-runtime -nomake tests -nomake examples

SET _VS_ROOT=
SET _QT_SRC_ROOT=
SET _QT_OUTPUT_INSTALL_ROOT=
SET _QT_CREATOR_ROOT=
```

#### Linux (Ubuntu)

Download following script and save it in `~/Qt` as `get_qt.sh`

``` sh
#!/bin/bash

sudo apt-get install build-essential perl python git "^libxcb.*" libx11-xcb-dev libglu1-mesa-dev libxrender-dev  libasound2-dev libgstreamer0.10-dev libgstreamer-plugins-base0.10-dev libfontconfig1-dev

VER="5.7.0"


VER2="${VER%.*}"
WSRC="http://download.qt-project.org/official_releases/qt/$VER2/$VER/single/qt-everywhere-opensource-src-$VER.tar.xz"


B=$(pwd)
Q="$B/qt"
SRC="$Q/src/$VER"
O="$Q/build/$VER"
XZ="$Q/xz/qt-$VER.tar.xz"
J=$(grep -c ^processor /proc/cpuinfo)
LOG="$O/log.txt"

OPTS=""
OPTS+=" -debug-and-release"
OPTS+=" -opensource"
OPTS+=" -static" 
OPTS+=" -confirm-license"
OPTS+=" -largefile" 
OPTS+=" -silent"
OPTS+=" -qpa xcb"
OPTS+=" -opengl"
OPTS+=" -fontconfig"
OPTS+=" -qt-zlib"
OPTS+=" -qt-libpng"
OPTS+=" -qt-libjpeg"
OPTS+=" -qt-freetype"
OPTS+=" -qt-harfbuzz"
OPTS+=" -qt-pcre"
OPTS+=" -qt-xcb"
OPTS+=" -qt-xkbcommon"
OPTS+=" -no-sql-db2" 
OPTS+=" -no-sql-ibase" 
OPTS+=" -no-sql-mysql" 
OPTS+=" -no-sql-oci" 
OPTS+=" -no-sql-odbc" 
OPTS+=" -no-sql-psql" 
OPTS+=" -no-sql-sqlite" 
OPTS+=" -no-sql-sqlite2" 
OPTS+=" -no-sql-tds"
OPTS+=" -no-gif"
OPTS+=" -no-nis"
OPTS+=" -no-cups" 
OPTS+=" -no-iconv"
OPTS+=" -no-dbus"
OPTS+=" -no-eglfs"
OPTS+=" -no-directfb" 
OPTS+=" -no-linuxfb"
OPTS+=" -no-glib"
OPTS+=" -no-kms"
OPTS+=" -nomake examples" 
OPTS+=" -nomake tests"

MODS="qtx11extras qtimageformats qtscript qtquick1 qtdeclarative qtquickcontrols qtsvg qtwebengine qtmultimedia"

mkdir -p "$Q"
mkdir -p "$Q/xz"
mkdir -p "$SRC"
mkdir -p "$O"

date > $LOG
[ ! -f $XZ ] && wget "$WSRC" -c -O "$XZ"
[ ! -x $SRC/configure ] && tar pxf "$XZ" --strip=1 -C "$SRC" "qt-everywhere-opensource-src-$VER" 
cd "$O"
MAKEFLAGS=-j$J "$SRC/configure" $OPTS

make -j$J >> $LOG

for M in $MODS
do
    echo "----------------------------------------- MODULE: $M" >> $LOG
    D=$O/$M
    mkdir -p $D
    cd $D
    $O/qtbase/bin/qmake $SRC/$M/
    make -j$J >> $LOG
done

echo "DONE"
```

Open Terminal and write following instructions

``` sh
cd ~/Qt
sudo bash get_qt.sh
```

#### Mac OSX

* Download [Qt Source](http://download.qt.io/official_releases/qt/5.7/5.7.0/single/) and unzip the package
* Open Terminal and write following instructions

``` sh
./configure -nomake examples -nomake tests -debug-and-release -static
make
make install
```

## Building

### Building from Qt Creator (recommended)

Open launcher-qt.pro project in Qt Creator and compile.

### Building from command line

#### Windows

First of all you need to setup the envirnonment.

**For x86 envirnonment**
``` Batch
[X86_QT_INSTALLATION_ROOT]/bin/qtenv2.bat
[VS_ROOT]/VC/bin/vcvarsall.bat
```

**For x64 envirnonment**
``` Batch
[X64_QT_INSTALLATION_ROOT]/bin/qtenv2.bat
[VS_ROOT]/VC/bin/vcvarsall.bat x64
```

Now generate Makefile with qmake.
``` Batch
qmake -r -spec win32-msvc2015
```

Compile.

**Release**
``` Batch
nmake -f Makefile.Release
```

**Debug**
``` Batch
nmake -f Makefile.Debug
```

## Launcher data file

Launcher data file is storing information about patcher and application secret. Both secrets are encrypted and written in raw format.
Data file location depends on the platform:

* Windows - launcher data file is stored in launcher executable resources under `3151` id as resource of type `RT_RCDATA` (type id - `10`). [Read more](https://msdn.microsoft.com/pl-pl/library/windows/desktop/ms648009(v=vs.85).aspx). If the resource is missing or corrupted, the launcher is searching for data file next to launcher location (the same method as Linux does).
* Mac OSX - it's placed in the Resources directory of bundle.
* Linux - it's placed next to the launcher executable .

## Log file

Launcher produces a log file during it's execution named `Launcher-log.txt`. It's location depends on the platform

* Windows - log file is located next to executable.
* Mac OSX - log file is located in bundle `Resources` directory.
* Linux - log file is located next to executable.

## Using Visual Studio as editor

Install [Qt Visual Studio Add-in](https://visualstudiogallery.msdn.microsoft.com/c89ff880-8509-47a4-a262-e4fa07168408).

Then you will be able to open Qt project (*patchkit-launcher-qt.pro*) with Visual Studio by entering *Qt5* menu from toolbar and selecting *Open Qt Project File (.pro)* option. Project will be automaticaly generated and opened within a solution. Note that Visual Studio project (*patchkit-launcher-qt.vcxproj*) shouldn't be added to the repository. If you want make some modifications to project setup you need to modify *patchkit-launcher-qt.pro* file manually and regenerate Visual Studio project. 

## Code style

### Files

* Header files - `headerfile.h`
* Source files - `sourcefile.cpp`

### Classes

* Classes - `class UpperCamelCase`
* Constructors - `ClassName(int t_lowerCamelCase)`
* Methods - `void lowerCamelCase(int t_lowerCamelCase)`
* Public field - `int lowerCamelCase;`
* Private field - `int m_lowerCamelCase;`
* Consts - `const int lowerCamelCase = 0;`

### Other
* Variable `int lowerCamelCase = 0;`
* Qt Signals - `void lowerCamelCase()`
* Qt Slots - `void lowerCamelCase()`

Every file must begin with license header
``` C++
/*
* Copyright (C) Upsoft 2016
* License: https://github.com/patchkit-net/patchkit-launcher-qt/blob/master/LICENSE
*/
```

Every header file must contain guard
``` C++
#ifndef HEADERFILENAME_H
#define HEADERFILENAME_H
...
#endif // HEADERFILENAME_H
```

## Errors

Launcher code is using exceptions to handle errors.
If something in launcher fails it should throw an exception. Exception and it's context will be saved to log file and will make debugging process easier.

## Logging

### Functions

There are five functions that can be used for creating log:

* `logDebug` - very deep informations, mostly contains raw data
* `logInfo` - describing launcher operations
* `logWarning` - describing failures from which launcher can be rescued
* `logCritical` - describing failures that makes launcher unable to work

Every function can format string. For example
``` C++
logInfo("Formatted log %1 with some data %2", .arg(QString("someData"), QString::number(5)));
```

### Rules

* Every important operation should start with log describing it's purpose but it shouldn't inform about it's result. For example:
``` C++
bool loadDataFile()
{
	logInfo("Loading data file from %1", .args(m_dataFilePath));
	...
}
```
* Log can display result of function. For example:
``` C++
if(loadDataFile())
{
	logInfo("Loaded data file.");
	...
}
else
{
	logWarning("Failed to load data file.");
	...
}
```
* Use `logCritical` to display exception description. For example:
``` C++
try
{
	...
}
catch(LauncherException &exception)
{
	logCritical(exception.what());
	...
}
```
