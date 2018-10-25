#include <iostream>
#include "BoundingBox.h"

/*
    BoundingBox is a recursive structure to break a model down into smaller groups of faces.  The model starts with a
    single box around it and then binary divisions of the boxes are made until the recursion depth is reached.  The 
    recursive binary structure speeds up ray-triangle intersections as only faces contained in intersected boxes need
    be tested.
*/

BoundingBox::BoundingBox(Eigen::MatrixXd& verts, Eigen::MatrixXi& faces, int recursionLimit) 
    : Vertices(verts), Faces(faces)
{
    // Constructor called directly by the Model object
    recursion_limit = recursionLimit - 1;
    std::cout << "HERE ARE THE VERTICES: " << Vertices << "\n";
    find_corners();
    // The first box should surround the entire model, so all faces are passed.
    for(int i=0;i<Vertices.cols();i++){
        std::cout << "Pushing back faces: " << i << "\n";
        parent_faces.push_back(i);
        contained_faces.push_back(i);
    }
    subdivide_box();
}

BoundingBox::BoundingBox(BoundingBox* parent_bb, int recursionLimit, Eigen::Vector3d min, Eigen::Vector3d max)
    : Vertices(parent_bb->Vertices), Faces(parent_bb->Faces)
{
    // Constructor of sub boxes in the recursive tree
    std::cout << "Making sub-box with depth: " << recursionLimit << "\n";
    recursion_limit = recursionLimit - 1;
    min_corner = min;
    max_corner = max;
    parent_faces = parent_bb->contained_faces;
    find_contained_faces();
    subdivide_box();
}

void BoundingBox::find_contained_faces(){
    Eigen::Vector3d point_a, point_b, point_c;
    for (unsigned int i=0; i<parent_faces.size(); i++){
        int face_num = parent_faces[i];
        point_a = get_vertex( Faces(0, face_num) );
        point_b = get_vertex( Faces(1, face_num) );
        point_c = get_vertex( Faces(2, face_num) );
        if (point_contained(point_a) || point_contained(point_b) || point_contained(point_c)){
            contained_faces.push_back(face_num);
        }
    }
}

bool BoundingBox::point_contained(Eigen::Vector3d point){
    bool contained = false;
    Eigen::Vector3d lower_bounds, upper_bounds;
    lower_bounds = point - min_corner;
    upper_bounds = max_corner - point;
    if (lower_bounds.minCoeff() >= 0 && upper_bounds.minCoeff() >= 0){
        contained = true;
    }
    return contained;
}

Eigen::Vector3d BoundingBox::get_min_corner(){
    return min_corner;
}

Eigen::Vector3d BoundingBox::get_max_corner(){
    return max_corner;
}

Eigen::Vector3d BoundingBox::get_vertex(int index){
    Eigen::Vector3d vertex = Eigen::Vector3d(Vertices(0, index), Vertices(1, index), Vertices(2, index));
    return vertex;
}

void BoundingBox::subdivide_box(){
    // Binary divide the current cube into 8 smaller cubes
    Eigen::Vector3d transform;
    Eigen::Vector3d cube_center = (max_corner - min_corner) * .5 + min_corner;
    Eigen::Vector3d temp_min;
    Eigen::Vector3d temp_max;
    if (contained_faces.size()>0 && recursion_limit>0){
        double half_side = max_corner(0) - min_corner(0);
        for (int x=0; x<2; x++){
            for (int y=0; y<2; y++){
                for (int z=0; z<2; z++){
                    transform << x*half_side, y*half_side, z*half_side;
                    temp_min = min_corner + transform;
                    temp_max = cube_center + transform;
                    BoundingBox sub_bb = BoundingBox(this, recursion_limit, temp_min, temp_max);
                    contained_boxes.push_back(sub_bb);
                }
            }
        }
    }
}


void  BoundingBox::find_corners(){
    double min_val = std::numeric_limits<double>::lowest();
    double max_val = std::numeric_limits<double>::max();
    max_corner << min_val, min_val, min_val;
    min_corner << max_val, max_val, max_val;
    calculate_bounding();
}

void BoundingBox::calculate_bounding(){
    for(int i=0; i<Vertices.cols(); i++){
        Eigen::Vector3d vertex = get_vertex(i);
        if (vertex(0) < min_corner(0)){
            min_corner(0) = vertex(0);
        }
        if (vertex(1) < min_corner(1)){
            min_corner(1) = vertex(1);
        }
        if (vertex(2) < min_corner(2)){
            min_corner(2) = vertex(2);
        }
        if (vertex(0) > max_corner(0)){
            max_corner(0) = vertex(0);
        }
        if (vertex(1) > max_corner(1)){
            max_corner(1) = vertex(1);
        }
        if (vertex(2) > max_corner(2)){
            max_corner(2) = vertex(2);
        }
    }
}

std::vector<int> BoundingBox::intersected_faces(Eigen::Vector3d ray_pt, Eigen::Vector3d ray_dir){
    std::vector<int> intersected_faces;
    std::cout << "intersected\n";
    if (!ray_intersects(ray_pt, ray_dir)){
        return intersected_faces;
    }
    if (contained_boxes.size() > 0){
        for (auto &sub_bb: contained_boxes){
            std::vector<int> hit_faces = sub_bb.intersected_faces(ray_pt, ray_dir);
            intersected_faces.insert(intersected_faces.end(), hit_faces.begin(), hit_faces.end());
        }
    } else{
        intersected_faces = contained_faces;
        std::cout << "leaf node found: \n ";
        for (auto i = intersected_faces.begin(); i != intersected_faces.end(); ++i)
            std::cout << *i << ' ';
        std::cout << "\n";
    }
    // Remove duplicate face indexes
    intersected_faces.erase( std::unique( intersected_faces.begin(), intersected_faces.end() ), intersected_faces.end());
    return intersected_faces;
}

bool BoundingBox::ray_intersects(Eigen::Vector3d ray_pt, Eigen::Vector3d ray_dir){
    Eigen::Vector3d center = (max_corner - min_corner) * .5 + min_corner;
    double radius = ((max_corner - min_corner) * .5).norm();
    double v = (center - ray_pt).dot(ray_dir);
    double c = (center - ray_pt).norm();
    std::cout << "max_corner\n" << max_corner << "\nmin_corner \n" << min_corner << "\n";
    std::cout << "Radius: " << radius << " V: " << v << " C: " << c << "\n";
    if ((radius*radius) > (c*c - v*v)){
        return true;
    } else{
        return false;
    }
}