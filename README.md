# Block World #
**Objective of the game** : Collect all the gold boxes without touching any obstacles

## Installing dependencies ##
**tl;dr** : This project requires GLFW and glad libraries. If installed skip to the next section.

Installing GLFW :
 - Install CMake
 - Obtain & Extract the GLFW source code from [here](https://github.com/glfw/glfw/archive/master.zip)
 - Compile with below commands
```   
cd glfw-master
mkdir build
cd build
cmake -DBUILD_SHARED_LIBS=ON ..
make && sudo make install
```
 Installing GLAD:
 - Go to [http://glad.dav1d.de](http://glad.dav1d.de)
 - Select the following settings
   Language: C/C++
   Specification: OpenGL
   gl: Version 4.5
   gles1: Version 1.0
   gles2: Version 3.2
   Profile: Core
   Select 'Add All' under extensions and click Generate.
 - Download the zip file generated.
 - Copy contents of include/ folder in the downloaded directory 
   to /usr/local/include/
 - src/glad.c should be always compiled along with your OpenGL 
   code

## Building Instructions ##

Create a build directory and build the project in the following manner
```mkdir build
cd build
cmake .. && make
./game
```
## Game controls ##

### Keyboard Controls ###

**c** : Cycle among different camera positions. There are a total of 5 predefined camera positions for better gaming experiance
**m** : Rotate about the current point of view horizontally
**n** : Rotate about the current point of view vertically
**Arrow keys** : For navigation
**p** : Toggle panning mode
**q** : quit the game
**r** : Restart the game

### Mouse Controls ###

**LMB** : click LMB and drag to adjust the current camera view ( only works when panning mode is active)
**Mouse Wheel** : For zoomin and zoomout 


