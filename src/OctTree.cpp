#include <iostream>
#include "OctTree.h"

/*
    OctTree is a recursive structure to break a model down into smaller groups of faces.  The model starts with a
    single box around it and then binary divisions of the boxes are made until the recursion depth is reached.  The
    recursive binary structure speeds up ray-triangle intersections as only faces contained in intersected boxes need
    be tested.
*/

OctTree::OctTree(Eigen::MatrixXd& verts, Eigen::MatrixXi& faces, int recursionLimit)
    : Vertices(verts), Faces(faces)
{
    // Constructor called directly by the Model object
    recursion_limit = recursionLimit;
    find_corners();
    center = (max_corner-min_corner) / 2 + min_corner;
    // The first box should surround the entire model, so all faces are passed.
    for(int i=0;i<Faces.cols();i++){
        contained_faces.insert(i);
    }
    subdivide_box();
}

OctTree::OctTree(OctTree* parent_bb, int recursionLimit, Eigen::Vector3d min, Eigen::Vector3d max)
    : Vertices(parent_bb->Vertices), Faces(parent_bb->Faces)
{
    // Constructor of sub boxes in the recursive tree
    recursion_limit = recursionLimit - 1;
    min_corner = min;
    max_corner = max;
    center = (max-min) / 2 + min;
    find_contained_faces(parent_bb->contained_faces);
    subdivide_box();
}

void OctTree::find_contained_faces(std::set<int> &parent_faces){
    Eigen::Vector3d point_a, point_b, point_c;
    for(auto face_num: parent_faces){
        point_a = get_vertex( Faces(0, face_num) - 1 );
        point_b = get_vertex( Faces(1, face_num) - 1 );
        point_c = get_vertex( Faces(2, face_num) - 1 );
        if (triangle_intersects(point_a, point_b, point_c)){
            contained_faces.insert(face_num);
        }
    }
}

double OctTree::max(double a, double b, double c) {
    return std::max(a,std::max(b, c));
}

double OctTree::min(double a, double b, double c) {
    return std::min(a,std::min(b, c));
}

bool OctTree::axis_test(const Eigen::Vector3d& axis, const Eigen::Vector3d& e, const Eigen::Vector3d& v0, const Eigen::Vector3d& v1, const Eigen::Vector3d& v2, const Eigen::Vector3d& u0, const Eigen::Vector3d& u1, const Eigen::Vector3d& u2) {
    double p0 = v0.dot(axis);
    double p1 = v1.dot(axis);
    double p2 = v2.dot(axis);
    double r = e[0] * std::abs(u0.dot(axis)) +
               e[1] * std::abs(u1.dot(axis)) +
               e[2] * std::abs(u2.dot(axis));
    if (std::max(-max(p0, p1, p2), min(p0, p1, p2)) > r) return false;
    return true;
}

bool OctTree::triangle_intersects(Eigen::Vector3d& v0, Eigen::Vector3d& v1, Eigen::Vector3d& v2) {
    // Test if this AABB intersects with the specified triangle
    Eigen::Vector3d dims = max_corner - min_corner;
    Eigen::Vector3d boxHalfSize(dims[0], dims[1], dims[2]);
    v0 = v0 - center;
    v1 = v1 - center;
    v2 = v2 - center;
    Eigen::Vector3d e0 = v1 - v0;
    Eigen::Vector3d e1 = v2 - v1;
    Eigen::Vector3d e2 = v0 - v2;

    Eigen::Vector3d u0 = Eigen::Vector3d(1.0, 0.0, 0.0);
    Eigen::Vector3d u1 = Eigen::Vector3d(0.0, 1.0, 0.0);
    Eigen::Vector3d u2 = Eigen::Vector3d(0.0, 0.0, 1.0);

    Eigen::Vector3d edges[] = {e0, e1, e2};
    Eigen::Vector3d axes[] = {u0, u1, u2};

    Eigen::Vector3d axis;
    for (auto edge:edges){
        for (auto axis:axes){
            axis = axis.cross(edge);
            if (!axis_test(axis, boxHalfSize, v0, v1, v2, u0, u1, u2)) return false;
        }
    }

    if (!axis_test(u0, boxHalfSize, v0, v1, v2, u0, u1, u2)) return false;
    if (!axis_test(u1, boxHalfSize, v0, v1, v2, u0, u1, u2)) return false;
    if (!axis_test(u2, boxHalfSize, v0, v1, v2, u0, u1, u2)) return false;

    Eigen::Vector3d normal = e0.cross(e1);
    if (!axis_test(normal, boxHalfSize, v0, v1, v2, u0, u1, u2)) return false;

    return true;
}

Eigen::Vector3d OctTree::get_vertex(int index){
    Eigen::Vector3d vertex = Eigen::Vector3d(Vertices(0, index), Vertices(1, index), Vertices(2, index));
    return vertex;
}

void OctTree::subdivide_box(){
    // Binary divide the current cube into 8 smaller cubes
    Eigen::Vector3d transform;
    Eigen::Vector3d cube_center = (max_corner - min_corner) * .5 + min_corner;
    Eigen::Vector3d temp_min;
    Eigen::Vector3d temp_max;
    if (contained_faces.size()>10 && recursion_limit>0){
        double half_x = (max_corner(0) - min_corner(0)) / 2.0;
        double half_y = (max_corner(1) - min_corner(1)) / 2.0;
        double half_z = (max_corner(2) - min_corner(2)) / 2.0;
        for (int x=0; x<2; x++){
            for (int y=0; y<2; y++){
                for (int z=0; z<2; z++){
                    transform << x*half_x, y*half_y, z*half_z;
                    temp_min = min_corner + transform;
                    temp_max = cube_center + transform;
                    contained_boxes.push_back(std::make_unique<OctTree>(this, recursion_limit, temp_min, temp_max));
                }
            }
        }
    }
}


void  OctTree::find_corners(){
    double min_val = std::numeric_limits<double>::lowest();
    double max_val = std::numeric_limits<double>::max();
    max_corner << min_val, min_val, min_val;
    min_corner << max_val, max_val, max_val;
    calculate_bounding();
}

void OctTree::calculate_bounding(){
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

std::set<int> OctTree::intersected_faces(Ray& ray){
    std::set<int> intersected_faces;
    if (!ray_intersects(ray)){
        return intersected_faces;
    }
    if (contained_boxes.size() > 0){
        for (std::unique_ptr<OctTree>& sub_bb: contained_boxes){
            std::set<int> hit_faces = sub_bb->intersected_faces(ray);
            intersected_faces.insert(hit_faces.begin(), hit_faces.end());
        }
    } else{
        intersected_faces = contained_faces;
    }
    return intersected_faces;
}
