#include <iostream>
#include <fstream>
#include <string>
#include <array>
#include <CGAL/IO/Polyhedron_builder_from_STL.h>


// The following function computes the given points vector's bounding box values
double* find_bounding_box(std::vector<std::array<double, 3>> points){
    static double bounding_box[3];
    // initializing the extreme values to hold the ones of the first point
    double min_x = points.at(0)[0];
    double max_x = points.at(0)[0];
    double min_y = points.at(0)[1];
    double max_y = points.at(0)[1];
    double min_z = points.at(0)[2];
    double max_z = points.at(0)[2];
    // traversing all triplets while each represents a point. (skipping the first point)
    for(int i = 1; i < points.size(); i++){
        // determining which values are the extremes in order to find the correct boundary
        if(points.at(i)[0] < min_x) min_x = points.at(i)[0];
        if(points.at(i)[0] > max_x) max_x = points.at(i)[0];
        if(points.at(i)[1] < min_y) min_y = points.at(i)[1];
        if(points.at(i)[1] > max_y) max_y = points.at(i)[1];
        if(points.at(i)[2] < min_z) min_z = points.at(i)[2];
        if(points.at(i)[2] > max_z) max_z = points.at(i)[2];
    }
    // computing the bounding box according to the found extreme points 
    bounding_box[0] = max_x - min_x;
    bounding_box[1] = max_y - min_y;
    bounding_box[2] = max_z - min_z;
    //std::cout << "Finnished computing bounding box successfully!\n";
    return bounding_box;
}

// The following function receives an STL file, a points vector and a triangles vector. It reads the file into the vectors.
void read_stl(std::string filename, std::vector<std::array<double, 3>> &points, std::vector<std::array<int, 3>> &triangles){
    std::ifstream infile(filename, std::ios::in);
    // in case the file could not be opened
    if(!infile){
        std::cerr << "Error: Cannot open file " << filename << std::endl;
        return;
    }
    if(!CGAL::read_STL(infile, points, triangles)){
        std::cerr << "Error: invalid STL file" << std::endl;
        return;
    }
    infile.close();
    return;
}

// The following function receives a vector of triplets of doubles - each representing a point, finds the minimal
// z-coordinate and subtracts its value from all other points. The method updates the input point vector as required..
void sub_min_z(std::vector<std::array<double, 3>> &points){
    double min_z = points.at(0)[2];
    // finding minimal z value
    for(int i = 1; i < points.size(); i++){
        if(points.at(i)[2] < min_z) min_z = points.at(i)[2];
    }
    // subtracting the minimal z value from each point
    for(int i = 0; i < points.size(); i++){
        points.at(i)[2] -= min_z;
    }
    return;
}

// The following method rescales each point of the current vector to fit into the bounding box
void rescale_vector(std::vector<std::array<double, 3>> &points, double x_bound, double y_bound, double z_bound){
    double* bounding_box = find_bounding_box(points);
    double x_scale = x_bound/bounding_box[0];
    double y_scale = y_bound/bounding_box[1];
    double z_scale = z_bound/bounding_box[2];
    double scale = fmin(fmin(x_scale, y_scale), z_scale);
    // if the current bounding box is smaller than the limitiations specified by the user - we dont scale the
    // model since it fits in the bounding box
    if(scale >= 1) return;
    for(int i = 0; i < points.size(); i++){
        points.at(i)[0] *= scale;
        points.at(i)[1] *= scale;
        points.at(i)[2] *= scale;
    }
    return;
}
// The following method receives a points vector and a triangles vector along with a file name and creates a new STL file
// with the data from the vectors.
void write_stl(std::vector<std::array<double, 3>> points, std::vector<std::array<int, 3>> triangles, std::string input_name){
    std::ofstream new_stl_file(input_name + "_out.stl", std::ios_base::app);
    new_stl_file.precision(2);
    new_stl_file << "solid model" << std::endl;
    // iterating over all triangles in the triangles vector
    for(int i = 0; i < triangles.size(); i++){
        new_stl_file << "facet normal 0.0 0.0 0.0" << std::endl;
        new_stl_file << "outer loop" << std::endl;
        // iterating over all three indices inside a single triangle
        for(int j = 0; j < 3; j++){
            new_stl_file << "vertex ";
            // iterating over the coordinates of a certain point in the points vector
            for(int k = 0; k < 3; k++){
                new_stl_file << std::fixed << points.at(triangles.at(i)[j])[k] << " ";
            }
            new_stl_file << std::endl;
        }
        new_stl_file << "endloop" << std::endl;
        new_stl_file << "endfacet" << std::endl;
    }
    new_stl_file << "endsolid" << std::endl;
    new_stl_file.close();
}

int main(int argc, char *argv[]){
    std::vector<std::array<double, 3>> points;
    std::vector<std::array<int, 3>> triangles;
    double x_bounding_box, y_bounding_box, z_bounding_box;
    int index_of_dot;
    std::string new_file_name;
    // in case a bounding box was not specified
    if(argc < 4){
        std::cerr << "No bounding box specified! you must provide at least three numbers as arguments\n" << std::endl;
        return 1;
    }
    std::string filename = (argc > 4) ? argv[4] : "polyhedron.stl";
    // initializing variables to hold the values of the input bounding box
    try{
        x_bounding_box = std::stod(argv[1]);
        y_bounding_box = std::stod(argv[2]);
        z_bounding_box = std::stod(argv[3]);
    }
    // if the values that were given are non-numeric catch the exception and prints whats missing
    catch(...){
        std::cerr << "Please enter valid numbers as integers or double to the first three arguments!\n" << std::endl;
        return 1;
    }
    // reading data from the STL file to points and triangles vectors
    read_stl(filename, points, triangles);
    // rescaling the points vector in order to fit the bounding box
    rescale_vector(points, x_bounding_box, y_bounding_box, z_bounding_box);
    sub_min_z(points);
    // finding the correct name for the new stl file
    index_of_dot = filename.find(".");
    new_file_name = filename.substr(0, index_of_dot);
    // writing the rescaled model into a new STL file
    write_stl(points, triangles, new_file_name);
    return 0;
}
