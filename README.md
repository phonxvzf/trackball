# Trackball
A simple trackball implemented using quaternion. I'm just trying to *git gud*, guys.

## Build Instructions
### Prerequisites
1. A modern C and C++ compiler
2. \*nix-like development environment
3. OpenGL 3.0
4. SDL2 and SDL2_image
5. libdl (for Glad)

### Building
```
make
```

## Running
```
$ ./trackball
```

## Instructions
- `w` to toggle wire-frame mode
- `t` to toggle between rendering texture and `0.5f * normal + vec3(0.5f, 0.5f, 0.5f)`
- `q` to quit program
