§Dual Contouring implementation in C++
=====================================

# Who?
Emil Ernerfeldt, June 2013


# What?
This is a C++ implementation of Dual Contouring, as outlined in http://www.frankpetterson.com/publications/dualcontour/dualcontour.pdf


# Program
The program generates a distance field containing a cylinder, a box and a spherical hole in these. It then proceeds to generate a triangle mesh for this, outputting it to a file 'mesh.obj'.

Settings can be found in "settings.hpp"


# Building

    > cd build
    > cmake .. && make
    > ./dc
    > open -a "OBJ Viewer" mesh.obj

or:

    > cd build
    > cmake -G Xcode ..
    > open DC.xcodeproj/

Only tested with clang. If there are any issues you may want to lower the warning-levels in CMakeLists.txt.

The code relies heavily on C++11, as well as one compiler-specific extension (nameless structs).


# Implementation
The differences from the paper include:
* No octrees, and hence no simplifications
* Adapted for input on a discrete lattice (3D grid): no interpolation on edges (tried it, works badly), instead uses points/normals from all corners on a sign-changing edge. This produces good results even for sharp corners.
* Uses standard "AtA" solving of the least-squares problems, as that is not only simple and fast, but sufficent when no simplifications are made (the paper recommends QR decomposition).
* Double precision floats to overcome the numberical instability of the least-square solver. Test floats by typedeffing 'real' in Math.hpp.
* Currently, each input voxel must have a correct sign. One could extend the method to handle 'null' voxels, so that no surface is created between a null and a non-null voxel.


# Limitations of Dual Contouring
## Triangle intersection
Dual Contouring produces one vertex per voxel, but that vertex doesn't necessarily lay inside of the voxel. This produces potential triangle intersections, mostly at very acute angles (sharp corners). One can clamp the coordinates to the voxel. Clamping is both necessary and sufficent to ensure no intersecting triangles, but doesn't always produce quite as nice results.

A better solution is to check for triangle intersections and resolve as an extra step after the contouring algorithm is done. This is left as an exercise to the reader.

## Input jitter
Dual Contouring is VERY sensitive to input jitter, both on the input distances and the normals. You can test this by setting PerturbField in Settings.hpp. This makes is un-suitable to real-world data, and more apt for computer generated data (e.g. CSG output).
