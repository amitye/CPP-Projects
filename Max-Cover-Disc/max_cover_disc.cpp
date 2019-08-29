#include <CGAL/Exact_predicates_exact_constructions_kernel.h>
#include <CGAL/Arr_circle_segment_traits_2.h>
#include <CGAL/Arrangement_2.h>
#include <iostream>
#include <fstream>
#include <CGAL/Sqrt_extension.h>
#include <CGAL/enum.h>
#include <CGAL/Rational_traits.h>
#include <boost/timer.hpp>

typedef CGAL::Exact_predicates_exact_constructions_kernel Kernel;
typedef CGAL::Arr_circle_segment_traits_2<Kernel> Traits;
typedef Traits::CoordNT CoordNT;
typedef Traits::NT NT;
typedef CGAL::Arrangement_2<Traits> Arrangement;
typedef CGAL::Sqrt_extension<CGAL::Gmpq, CGAL::Gmpq> Sqrt_type;
typedef Kernel::Point_2 Point_2;
typedef Kernel::Circle_2 Circle_2;
typedef Traits::Curve_2 Curve_2;

using namespace std;

// the following function receives a file name and returns a vector of the points specified
// in the file
vector<Point_2> file_points_to_vector(string file_name){
    vector<Point_2> result;
    ifstream infile(file_name, ifstream::in);
    // in case there was a problem opening the file, return an empty vector for the main method to throw an error
    if(infile.fail()){
        return result;    
    }
    int num_of_points;
    Point_2 current;
    // reading first value in file - according to documentation this is the number of points
    infile >> num_of_points;
    for(int i = 0; i < num_of_points; i++){
        infile >> current;
        result.push_back(current);
    }
    infile.close();
    return result;
}

// the following function receives a vector of points and a radius r and returns an
// arrangement which contains circles such that each point is a center of a circle with
// radius r
Arrangement vector_to_arrangement(vector<Point_2> points, Kernel::FT radius, vector<Circle_2> &comp){
    Arrangement arr;
    Circle_2 circle;
    Curve_2 curve;
    // iterating over all points in order to create circles
    for(int i = 0; i < points.size(); i++){
        circle = Circle_2(points[i], radius, CGAL::CLOCKWISE);
        comp.push_back(circle);
        curve = Curve_2(circle);
        // inserting the curve and the center point to the arrangement
        insert(arr, curve);
    }
    return arr;
}

// The following function returns true if a point is in the circle or on its boundary
bool is_inside(Traits::Point_2 point, Circle_2 circle){
    bool result = false;
    NT center_x = circle.center().x();
    NT center_y = circle.center().y();
    NT squared_radius = circle.squared_radius();
    if(((point.x() - center_x) * (point.x() - center_x)) + ((point.y() - center_y) * (point.y() - center_y)) <= squared_radius){
        result = true;
    }
    return result;
}

// The following function iterates over all vertices in the arrangement and returns the point that 
// is inside the maximal number of circles among all points, along with the number of circles it is
// in.
pair<Traits::Point_2, int> maximal_point_from_arr(Arrangement arr, vector<Circle_2> comp){
    pair<Traits::Point_2, int> result;
    result.second = 0;
    // iterating over all vertices in the arrangement
    Arrangement::Vertex_const_iterator vit;
    for(vit = arr.vertices_begin(); vit != arr.vertices_end(); ++vit){
        pair<Traits::Point_2, int> current;
        current.first = vit->point();
        current.second = 0;
        // checking each vertex with all circles
        for(int j = 0; j < comp.size(); j++){
            if(is_inside(current.first, comp[j])) current.second++;
        }
        // if the current point is inside more circles than the one the was defined maximal, change
        // the current point to be the  maximal
        if(current.second > result.second) result.swap(current);
    }
    return result;
}

// exporting a coordinate of type square-root extension to the standard output
std::ostream& operator<<(std::ostream& os, const CoordNT& c){
    CGAL::Rational_traits<NT> rt;
    cout << "(" << rt.numerator(c.a0()) << "/" << rt.denominator(c.a0()) << "," << rt.numerator(c.a1()) << "/" << rt.denominator(c.a1()) << "," << rt.numerator(c.root()) << "/" << rt.denominator(c.root()) << ")";
    return os;
}

// exporting a point of two coordinates of type square-root extension to the standard output
std::ostream& operator<<(std::ostream& os, const Traits::Point_2& p){
    cout << "(" << p.x() << "," << p.y() << ")";
    return os;
}

int main(int argc, char **argv){
    // setting the correct value of r (size of radius) out of input r^2
    string radius = argv[1];
    NT squared_r = NT(radius);
    const char* points_file = (argc > 2)? argv[2] : "points.txt";
    vector<Point_2> point_vector = file_points_to_vector(points_file);
    // in case the file could not be read
    if(point_vector.empty()){
        cout << "Error: The file " << points_file << " could not be read!" << endl;
        exit(1);
    }
    vector<Circle_2> circles_vector;
    // introducing a timer after the input file was read
    boost::timer timer;
    Arrangement arr = vector_to_arrangement(point_vector, squared_r, circles_vector);
    pair<Traits::Point_2, int> best = maximal_point_from_arr(arr, circles_vector);
    double secs = timer.elapsed();
    cout << "Execution time: " << secs << endl;
    cout << "The maximal covering disc covers " << best.second << " points" << endl;
    cout << "The maximum covering disc is centered at "  << best.first << endl;
    return 0;

}







