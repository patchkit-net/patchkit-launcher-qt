# Building

## Boost
Launcher requires the Boost library to compile.

Make sure to set the `PK_LAUNCHER_BOOST_INCLUDEDIR` environment variable (from command line or using QT Creator project settings) to an appropriate path.

For example, ff you're using [PDK](https://github.com/patchkit-net/patchkit-development-kit/) the path should look something like `/path/to/patchkit-development-kit/src/boost`.

## Building from Qt Creator (recommended)

Open the `patchkit-launcher-qt.pro` project in Qt Creator and compile.

## Building from command line

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

## Building statically linked Qt

Refer to [PatchKit Development Kit](https://github.com/patchkit-net/patchkit-development-kit).