# Building

## Boost

Launcher requires the Boost library to compile.

For Windows make sure to download boost with the binaries, it can be downloaded from here:
https://sourceforge.net/projects/boost/files/boost-binaries/

Set the `PK_LAUNCHER_BOOST_INCLUDEDIR` and `PK_LAUNCHER_BOOST_LIBDIR` environment variable (from command line or using QT Creator project settings) to an appropriate path.

For example, if you're using [PDK](https://github.com/patchkit-net/patchkit-development-kit/) the path should look something like `/path/to/patchkit-development-kit/src/boost`.

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

## Troubleshooting

### Linker: Boost library name is different than we require

Just find the file and rename it to match the names.

### Running: Throw TLS errors on HTTPS connections

You need to put openssl and libcrypto dlls next to the launcher exe.

1. Find out what version to use with this function:
`qDebug() << QSslSocket::supportsSsl() << QSslSocket::sslLibraryBuildVersionString() << QSslSocket::sslLibraryVersionString();`
2. Download the binaries from here: https://wiki.openssl.org/index.php/Binaries
3. Put it next to the Launcher.exe

### Running: Raises errors of missing Qt libraries

You need to add bin path of your Qt installation to PATH. For instance:

`C:\Qt\5.12.5\msvc2017_64\bin`
