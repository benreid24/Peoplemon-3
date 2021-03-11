# BLIB Project Template
This a template repository meant to fast track the creation of projects based on [BLIB](https://github.com/benreid24/BLIB).
It contains a proper directory structure, clang format, a basic gitignore, Doxygen config, CMake read to go, gtest setup, necessary submodules, and some example code to show how it all ties together.

## Setup
Some nominal changes are necessary to convert this template into a project of your own. Namely:

1. Clone all submodules with `git submodule update --init --recursive`
2. Search and replace `ProjectName` with your own project name. It appears often in CMake files
3. Update the path to your compiler and any compiler settings that need changing in [c_cpp_properties.json](.vscode/c_cpp_properties.json)
4. Tweak the [Doxyfile](Doxyfile) and [.clang-format](.clang-format) files to your liking
5. Add your resources and any required DLL's to the project directory
