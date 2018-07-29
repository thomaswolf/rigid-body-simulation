### About
A header only constraint based rigid body physics engine developed during the Physically Based Simulation lecture at ETH by 
Thomas Wolf, Thomas Etterlin and Stephan Zehnder.

### Features
* Hinge, distance, spring, balljoint constraints
* Inactivity Detector
* Leightweight: Only GLM needed for the physics part
* Android (OpenGL ES) and Desktop (OpenGL) demos provided

### Compile
`git submodule init
git submodule update
mkdir build
cd build
cmake ..
make -j8`

### Run
`./main`


