# patchkit-launcher-qt
The PatchKit Launcher project.

The Launcher is a simple GUI application written in C++ with QT. It's purpose is to bootstrap the patching process. During a typical execution flow the Launcher is the first application to be started, it should verify that the newest version of the Patcher is installed. If that is not the case it should download a newer version. Afterwards it should start the Patcher and provide it with some execution context (installation directory, etc.). As such the Launcher should be kept as simple as possible.

## Usage
This section details how one should use the launcher without getting into the implementation or technical details. It explains the locations used by the Launcher and the files used and produced by it.

Using the Launcher is very simple. Given a binary simply execute it. It should open a simple window displaying progress value and information. On it's finished executing it should either terminate with an error message or start the Patcher.

Launcher uses a concept of working directory during it's runtime. The location of this directory platform dependent:
1. Windows: next to the executable
2. Linux: next to the executable
3. OSX: `~/Library/Application Support/PatchKit/Apps/<first 8 characters of app secret>`

The working directory will **always** contain the following files:
1. `.locations` the file describing where the patcher and the app are installed
2. `.lock` the lock file preventing another launcher (or Patcher) from being executed at the same time

The patcher and application will be located in the directories pointed to by the `.locations` file. On Windows and Linux these locations will be in the working directory, in the `patcher` and `app` directories. On OSX the user will be asked where to install the game with the default location being the working directory.

The log file `launcher-log.txt` will be located next to the executable on Windows and Linux platforms. On OSX it will be located in the ` ~/Library/Application Support/PatchKit/Apps` directory.

The data file `launcher.dat` is required on most platforms for proper functioning of the launcher. It's expected to be next to the executable. On Windows the same data can be provided through the use of an executable resource.

### Environment variables
`PK_LAUNCHER_API_URL` Overrides the main api url \
`PK_LAUNCHER_API_CACHE_URL` Overrides **all** the cache api urls with a single cache url \
`PK_LAUNCHER_SELECT_INSTALLATION_LOCATION` If set to 1, the launcher will ask the user for the installation location. Otherwise the launcher will assume platform default behaviour \
`PK_LAUNCHER_FORCE_PATCHER_SECRET` Overrides the used patcher secret \
`PK_LAUNCHER_FORCE_PATCHER_VERSION_ID` Overrides the used patcher version id

## Development
### Requirements
* [Qt Framework](https://www.qt.io/download/)
* [Boost](https://www.boost.org/)
* Microsoft Visual Studio 2015 or the Microsoft Build Tools with support for VS2015 (only for Windows)

The Launcher is a C++/QT project. It's recommended to use the QT Creator for development. But it is possible to use other IDEs. A guide on configuring Visual Studio for development is placed at the end of this section.

### Building for development
Launcher depends on Boost and QT. Qt dependencies will be handled automatically by the QT Creator (or other properly configured IDE). But Boost dependencies have to be supplied manually. Make sure your Boost libraries are built (using the `./b2` command). If you're using QT Creator navigate to the `Projects` tab and under build settings for your configuration, under `Build Environment` add `PK_LAUNCHER_BOOST_INCLUDEDIR` with a value pointing to `path/to/boost/boost_x_y_z` (where `x`, `y` and `z` are version numbers). In the same way set `PK_LAUNCHER_BOOST_LIBDIR` to `path/to/boost/boost_x_y_z/stage/lib`. Now you'll be able to build the project in the debug configuration.

### Building for release
In order to build for release you'll need to build and statically link a version of QT. [PatchKit Development Kit](https://github.com/patchkit-net/patchkit-development-kit) is a repository containing all the dependencies and instructions on how to achieve that.

If you're using QT Creator, simply navigate to the Manage Kits menu and add a new configuration with your statically compiled QT. Then build the project.

For command line instructions look [here](./BUILDING.md).

### Using Visual Studio as editor
Install [Qt Visual Studio Add-in](https://visualstudiogallery.msdn.microsoft.com/c89ff880-8509-47a4-a262-e4fa07168408).

Then you will be able to open Qt project (`patchkit-launcher-qt.pro`) with Visual Studio by entering *Qt5* menu from toolbar and selecting **Open Qt Project File (.pro)** option. Project will be automaticaly generated and opened within a solution. Note that Visual Studio project (`patchkit-launcher-qt.vcxproj`) shouldn't be added to the repository. If you want make some modifications to project setup you need to modify `patchkit-launcher-qt.pro` file manually and regenerate Visual Studio project.

### Project structure
The `core` subdirectory is meant to contain the core functionality of the Launcher - updating, configuration, reading & writing files, etc.

The `app` subdirectory uses the `core` project ot build a GUI application.

The `tests` subdirectory is used for tests. To run the tests simply build and run the `tests` project.

The `cli` subdirectory is an experimental CLI version of the Launcher intended for integration tests.

### [Changelog](/CHANGELOG.md)
The changelog is used to keep track of the changes made to the project. For a detailed guid refer to [keepachangelog.com](https://keepachangelog.com/). Additionally we've adopted a rule to add issue/ticket numbers to respective entries.

### [Code style guide](/CODE_STYLE.md)

### Cancellation
Launcher logic is run on a different thread than the GUI. We want to give the user the ability to cancel the Launcher. As such it's necessary to be able to stop the program at any given point - for instance while a download loop is running. This functionality is provided by the `CancellationToken` and `CancellationTokenSource` classes. Every operation that can be cancelled should accept a cancellation token by copy.

```cpp
void cancellableOperation(CancellationToken cancellationToken)
{
    while (true)
    {
        cancellationToken.throwIfCancelled();
        anotherCancellableOperation(cancellationToken);
    }
}

void anotherCancellableOperation(CancellationToken cancellationToken)
{
    for (int i = 0; i < 10000; i++)
    {
        cancellationToken.throwIfCancelled();
    }
}
```

When a `CancellationTokenSource` is cancelled it propagates through all the copies of the `CancellationToken` and will eventually throw a `CancelledException`. A `CancelledException` should be handled without displaying any error messages, intead the program should quit as soon as possible.

### User interaction
Any time a form of use interaction is required it should be carried out through the `ILauncherInterface` class. Launcher methods and classes can take a reference to an implementation of this interface. An implementation of this interface should be provided by the "final stage" project - like the `app` gui project. In the `app` subdirectory, the interface is implemented by connecting QT signals to respective methods in the `MainWindow` class. When a method on the interface is called it blocks and waits until the interaction is finished.

### Custom exceptions
The `customexceptions.h` file contains helpful macros used to define custom exception types. All exceptions used in the Launcher should derive from the `std::runtime_error` class and the `std::runtime_error` should be caught at the top level of Launcher execution flow.

### Launcher data file
Launcher data file is storing information about patcher and application secret. Both secrets are encrypted and written in raw format.
Data file location depends on the platform:

* Windows - launcher data file is stored in launcher executable resources under `3151` id as resource of type `RT_RCDATA` (type id - `10`). [Read more](https://msdn.microsoft.com/pl-pl/library/windows/desktop/ms648009(v=vs.85).aspx). If the resource is missing or corrupted, the launcher is searching for data file next to launcher location (the same method as Linux does).
* Mac OSX - it's placed in the Resources directory of bundle.
* Linux - it's placed next to the launcher executable.


