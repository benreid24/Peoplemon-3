# Peoplemon 3

## Build Status
![Windows](https://github.com/benreid24/peoplemon-3/workflows/windows-verify/badge.svg?branch=master) 
![macOS](https://github.com/benreid24/peoplemon-3/workflows/macos-verify/badge.svg?branch=master) 
![Linux](https://github.com/benreid24/peoplemon-3/workflows/linux-verify/badge.svg?branch=master)

***

Development repository for the third iteration of Peoplemon! This is still the first Peoplemon game, just our third try. 

## Download

**TODO**: Build Peoplemon in CI and link to downloads here

## Development

For those working on Peoplemon, or anyone else wanting to try out some changes on their own, the below information provides some good starting
points for getting to work.

### Documentation

- [Editor Documentation](docs/editor/reference.md)
- [Game Debugging](docs/debugging.md)
- [Peoplemon **bScript** Reference](docs/scripts/reference.md)
- [C++ Source Documentation](https://benreid24.github.io/Peoplemon-3/modules.html)
- [BLIB Engine Source Documentation](https://benreid24.github.io/BLIB/modules.html)

### Editor Setup

The Peoplemon editor may be used with the release build of Peoplemon, or may be used with the cloned source as well. Those
wishing to make changes should follow the below instructions to [clone the repository](#clone-peoplemon). Editor binaries may be downloaded from the [releases page](https://github.com/benreid24/Peoplemon-3/releases/latest). Presently the binaries are only available for Windows. Once downloaded simply extract all the files and folders directly into the Peoplemon folder. More about the editor can be [read here](docs/editor/reference.md).

### Building From Source

Anyone making changes to the core Peoplemon code (or the BLIB engine) must build from source. This requires [CMake](https://cmake.org/download/) and a
C++17 compliant compiler installed and configured to be used with CMake.

#### Clone Peoplemon

Those not added as collaborators should first fork Peoplemon. If forked, replace the below URL's with your own fork. Peoplemon's dependencies are
bundled as submodules, so those must be initialized as well.

```
git clone git@github.com:benreid24/Peoplemon-3.git
cd Peoplemon-3
git submodule update --init --recursive
```

If on Windows you must also get the `openal32.dll` file from SFML:
```
cp lib/BLIB/lib/SFML/extlibs/bin/x64/openal32.dll ./
```

#### Build Peoplemon

Once cloned Peoplemon and its dependencies may now be built using CMake. The following build commands and more are available in the
[VS Code build tasks file](.vscode/tasks.json). Feel free to replace the generated build file type with your own system. Release builds may be used
as well.

```
cmake -DCMAKE_BUILD_TYPE=Debug -G "Unix Makefiles" -S . -B ./build-debug
cd build-debug && make
```

If you have properly completed the setup and build steps you should now have runnable binary Peoplemon files to start the game with. When you make changes
to any C++ code be sure to rerun the above build commands.
