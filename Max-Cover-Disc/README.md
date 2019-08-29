Given a real number which represents the size of the required radius, and a set of n points in the plane, the program outputs the center of the closed disc or radius r that covers the maximum number of points.
Execution:
  The running of the program must contain at least one argument - the squared radius.
  In case no file name was provided with the desired n points, the default is to turn to "points.txt". If such a file doesn't exists, or if the filename couldn't be found in the given path - the program returns an error and ask for a valid file.

I have converted all input points to closed discs with radius r (input), and used an arrangement for these discs - a dual problem to the original.
Note that the arrangement seperates each disc into two x-monotone curves.
There are two cases for the result:
  1. In case there are no intersecting or touching circles in the arrangement - the result will be the first point contained in the first lex. order disc from the arrangement (one of the two points that split the disc into curves).
  2. In case there are intersecting or touching circles in the arrangement - the point which is contained in most circles will be returned as the center of the desired disc.

Note that if the point is contained in a disc in the dual problem (arrangement), the disc for which its center is that point will contain the point which is the center of the disc in the primal problem.
Therefore, the point which will be returned is the one contained in most discs among all points and therefore it is the one that if we create a disc with it as center, it will cover the maximal number of points possible in the given input set with the given radius.

I have used CGAL - Computational Geometry Algorithms Library in order write this program.
