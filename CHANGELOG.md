# Change Log
All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](http://keepachangelog.com/)
and this project adheres to [Semantic Versioning](http://semver.org/).

## [unreleased]
### Changed
- IO methods in ioutils now take cancellation tokens
## [v1.5.0.0]
### Added
- #1214 Additional log entries

### Fixed
- #1205 Fixed the offline mode bug
- #1215 Windows permissions issues arising from premature logger initialization
- #1216 Would you like to retry dialog
- #1255 & #1256 lock file issues
- #1223 Fixed long waiting times

## [v1.4.0]
### Added
- Boost library linking
- lock file implementation using `boost::interprocess::file_lock`
- New patcher manifest file format processing.
- Launcher will use the api-cache address from the `PK_LAUNCHER_API_CACHE_URL` environment variable if available.
- The network-status symbol

### Changed
- Overhauled a significant portion of the codebase
- Error messages now describe what happened
- ContentSummary parsing - added fields size and uncompressed_size

### Fixed
- Launcher will now query api caches if the main api doesn't respond
- The way the Launcher is reading installation info
- A small issue which would cause the Launcher to lock itself from executing again if an error occured when trying to start the Patcher.
- (1134) The invalid chunk error introduced in rc5
- path handling bugs introduced in rc5
- handling of PK_LAUNCHER_API_URL and PK_LAUNCHER_API_CACHE_URL environment variables

## [v1.3.0]
### Added
- Creating the launcher_path file in the Patcher directory, the file contains a relative path to Launcher executable.
- Lockfile implementation using QLockFile

### Fixed
- The error on OSX caused by downloading the patcher into location without necessary permissions.
- The crash caused by invalid permissions on Windows

## [v1.2.0]
### Added
- .lock file creation to prevent multiple instances launching at the same time.

### Fixed
- Obtaining version number from git on Windows

### Changed
- Version number must now be wrtitten into the version.pri file

## [v1.1.0]
### Added
- Caller identification in API calls
- Logging current launcher version

### Changed
- Launcher will use API Gateway (api2.patchkit.net) from now on

### Fixed
- Launcher now only includes country code in appropriate api calls

## [v1.0.3]
### Added
- Additional api-cache server

## [v1.0.2]
### Added
- A few test cases regarding new changes

### Changed
- A significant part of codebase responsible for downloading

### Fixed
- The "Launcher is stuck on 'Waiting...'" bug
- Crashes caused by unmanaged signal calls

## [v1.0.2-rc2] - 2017-08-21
### Fixed
* OSX compilation

## [v1.0.2-rc1] - 2017-08-11
### Added
* BUILDING.md, CODE_STYLE.md and CHANGELOG.md
* Feature #536
* The binary_replace.rb script (part of feature #536)
* RUNNING_TESTS.md
* Geolocation
* Feature #538

### Changed
* Rehauled the logging system
* The README

### Fixed
* Bug #558 - the launcher would the user 2 times before going into offline mode
* Output from logs and tests no longer intermingles, and tests no longer produce log files.

## [v1.0.1-rc3] - 2017-05-15
### Changed
- Changed installation directories for OSX systems.
