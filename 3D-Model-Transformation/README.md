The program accepts an axis-aligned bounding box and a .STL file that describes a 3D model.
The goal is to scale the given model such that the scaled model is the largest that could fit into the given bounding box.

This is a required tool in 3D printing since we have different sized printers and we wish to be able to print models which vary in size.
The program also translates the model such that the z=0 plane becomes a supporting plane.

The output is a .STL file which contains the description of the model after scaling its size appropriately.

In order to parse the .STL files, I used CGAL - Computational Geometry Algorithms Library.
