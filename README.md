[![Nile Engine logo](/logo.png)]()

## Nile Game Engine

[![Project Status: Active – The project has reached a stable, usable state and is being actively developed.](http://www.repostatus.org/badges/latest/active.svg)](http://www.repostatus.org/#active)


Nile engine is an experimental game engine, written in C++17 or later, developed as a hobby.
It is open source and distributed under the [MIT License](http://opensource.org/licenses/MIT).

Feedback, issues and pull requests are always welcome!

* Bugs and issues can be raised in [Issues on GitHub](https://github.com/stelro/Nile-engine/issues).
* Please feel free to ping me on [stelmach.ro@gmail.com](stelmach.ro@gmail.com) if you have any questions or feature requests.
* You can track feature requests and the progress of its features/issues on [Trello board](https://trello.com/b/62LkitLV/nile-game-engine). So feel free to add some comments or vote for your favorite feature requests.

## Supported platforms

Nile is available and supported on Linux at this moment. Feature builds will be available for Windows 10 and Mac OS x.

## How to build

1. Clone the repository
2. Make sure that submodules are checked out and up-to-date:

    ```shell
    git submodule update --init --recursive
    ```

3. Build and compile:

    ```shell
    mkdir build && cd build

    # Debug build
    cmake -DCMAKE_CXX_COMPILER=g++ -DCMAKE_BUILD_TYPE=Debug ..

    # Release build
    cmake -DCMAKE_CXX_COMPILER=g++ -DCMAKE_BUILD_TYPE=Debug ..

    make -j4

    ## Getting started

    To create a new project, checkout the example folder in the root directory.