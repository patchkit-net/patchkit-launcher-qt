# Building

## Boost
Set up all the requirements.
For example, if you're using [PDK](https://github.com/patchkit-net/patchkit-development-kit/) you can use the bundled Boost installation.

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
