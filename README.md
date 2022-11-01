# Surface++

Things implemented:
 - OpenGL rendering;
 - Editor;
 - Python blueprint scripting;
 - Goal-Oriented Action Planning (GOAP) AI;
 - Audio playback;
 - Objects (de)serialization, ability to store game world to a file;
 - Particle system;
 - entt entity component system is used;


## Dependencies installation
```
apt-get install libsoil-dev libglm-dev libassimp-dev \
        libglew-dev libglfw3-dev libxinerama-dev \ 
        libxcursor-dev libxi-dev libasound2-dev libsdl2-dev

make init
```

## Run

To build and run *surfacepp*'s world editor in its current state, use:
```
make && ./build/tools/editor
```

Also, a game target is available:
```
make && ./build/app/dummy
```

Controls:
In both editor and game binaries use `F5-F6` to switch the mouse cursor mode.

