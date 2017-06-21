# Running Tests
All that's needed to run the tests is to execute the `LauncherTests` executable, which is located in the build directory.

Automating the process is recomended:

## QT Creator
Add a custom process (**Add Build Step -> Custom Process Step**) step, in the build settings of the `patchkit-launcher-qt-app` project, with the following parameters:

**Command:** `./patchkit-launcher-qt-tests/LauncherTests` \
**Arguments:** ` ` \
**Working directory:** `%{buildDir}`

**NOTE: The build step has to be added separately for each configuration (Debug, Release, etc.)**

## Visual Studio
Coming soon...