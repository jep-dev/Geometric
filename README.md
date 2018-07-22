# Geometric
A combination of the RAII approaches from Glaii and new abstract/geometric algebra constructs.

## Setup
First, clone the repository: `git clone https://github.com/jep-dev/geometric`

On Linux, you will need the following libraries:
* glbinding: `sudo apt-get install libglbinding-dev`
* SOIL: `sudo apt-get install libsoil-dev`
* DevIL: `sudo apt-get install libdevil-dev`
* SDL2: `sudo apt-get install libsdl2{,-gfx,-image,-mixer,-net,-ttf}-dev`

Altogether, you can install the requirements using the following:
`sudo apt-get install libglbinding-dev libsoil-dev libdevil-dev libsdl2{,-gfx,-image,-mixer,-net,-ttf}-dev`

You will also probably need to install drivers for your CPU/GPU; check for available drivers in the "Additional Drivers" section of your settings.

Finally, if you'd like to build the documentation, you'll need:
* Doxygen: `sudo apt-get install doxygen{,-latex}`
* Graphviz: `sudo apt-get install graphviz`
The documentation is written in Doxygen format, so other documentation tools that can parse Doxygen
comments will work as well.

## Building
Simply run `make` to build all of the libraries and executables. To build a specific executable, e.g. `bin/view`, run `make bin/view`. This project can be built in parallel using `make -jN` for N threads, or just `make -j` to let Make decide the number of threads.

## Installation
Currently, there is no mechanism to install any of the executables or libraries; however, installation is not necessary to run the executables or use the libraries. See 'Usage' for more information.

## Usage
Most executables ignore command-line arguments; run from the project's root directory, as in `bin/view` to run the executable corresponding to `app/view.cpp`. Without static linkage or additional steps, none of the executables are expected to work from another directory, including their own, as in `./view` from `bin/`. Static linkage is not currently implemented, but will be by the first release.
