#include <iostream>
#include <fstream>
#include "Model.h"
#include <sstream>
#include <iomanip>

Model::Model(std::string object_file){
    original_file = object_file;
    load_model();
}

Model::Model(){}

void Model::load_model(){
    std::ifstream in(original_file);
    if (!in){
        //std::cerr << "Could not open " << original_file << std::endl;
        load_successful = false;
        return;
    }
    std::string obj_line;
    std::string line_type = "";
    double vx, vy, vz;
    int f1, f2, f3;
    std::string s1, s2, s3;
    while (std::getline(in, obj_line)){
        std::stringstream obj_read(obj_line);
        obj_read >> line_type;
        if (line_type[0] == 'v' && line_type.size() == 1){
            obj_read >> vx >> vy >> vz;
            add_vertex(vx, vy, vz);
        }
        if (line_type[0] == 'f'){
            obj_read >> f1 >> s1;
            obj_read >> f2 >> s2 ;
            obj_read >> f3 >> s3 ;
            add_face(f1, f2, f3);
        }
        // Materials line starts with mtllib
        if (line_type[0] == 'm'){
            std::string file_name;
            obj_read >> file_name;
            load_material(file_name);
        }
    }
    in.close();
    on_model_load();
}

void Model::on_model_load(){
    calculate_face_normals();
    int recursion_depth = 5;
    bounding_box = new BoundingBox(Vertices, Faces, recursion_depth);
}


void Model::add_vertex(double vx, double vy, double vz){
    int num_cols = Vertices.cols();
    Vertices.conservativeResize(4, num_cols+1);
    Vertices.col(num_cols) << vx, vy, vz, 1;
}

void Model::add_face(int v1, int v2, int v3){
    int num_cols = Faces.cols();
    Faces.conservativeResize(3, num_cols+1);
    Faces.col(num_cols) << v1, v2, v3;
}

void Model::add_face_normal(Eigen::Vector3d face_normal){
    int num_cols = FaceNormals.cols();
    FaceNormals.conservativeResize(3, num_cols+1);
    FaceNormals.col(num_cols) << face_normal;
}

void Model::calculate_face_normals(){
    FaceNormals = Eigen::MatrixXd();
    int num_cols = Faces.cols();
    Eigen::Vector3d first_vertex, second_vertex, third_vertex;
    Eigen::Vector3d first_vector, second_vector;
    for (int i=0; i<num_cols; i++){
        first_vertex << get_vertex( Faces(0, i) - 1 );
        second_vertex << get_vertex( Faces(1, i) - 1 );
        third_vertex << get_vertex( Faces(2, i) - 1 );
        // Calculate normal in counter clockwise direction?
        first_vector = first_vertex - third_vertex;
        second_vector = first_vertex - second_vertex;

        add_face_normal(first_vector.cross(second_vector).normalized());
    }
}

Eigen::Vector3d Model::get_vertex(int index){
    Eigen::Vector3d vertex = Eigen::Vector3d(Vertices(0, index), Vertices(1, index), Vertices(2, index));
    return vertex;
}

Eigen::MatrixXd Model::get_vertices(){
    return Vertices;
}

void Model::save_vertices(Eigen::MatrixXd new_vs){
    Vertices = new_vs;
    on_model_load();
}

void Model::save_model(std::ostream &output){
    std::ifstream original(original_file);
    std::string orig_line;

    int vert_num = 0;
    while (std::getline(original, orig_line)){
        if (orig_line.size() > 0 && !std::isspace(orig_line[0])){
            if (orig_line[0] == 's' || (orig_line[0] == 'v' && orig_line[1] == 'n')){
                continue;
            } else{
                if (orig_line[0] == 'v'){
                    output << "v";
                    int PRECISION = 7;
                    for (int i=0; i<3; i++){
                        output << " ";
                        output_formatted_float(output, Vertices(i, vert_num), PRECISION);
                    }
                    output << "\n";
                    vert_num++;
                } else{
                    output << orig_line << "\n";
                }
            }
        }
    }
    original.close();
}

void Model::output_formatted_float(std::ostream &o, double number, int precision){
    // Outputs doubles with "precision" worth of tens places after period
    std::string output_str = std::to_string(number);
    int index_of_period = output_str.find('.');
    if (index_of_period == -1){
        output_str += ".";
        index_of_period = output_str.find('.');
    }
    unsigned int goal_length = index_of_period + precision + 1;
    while (output_str.size() < goal_length){
        output_str += "0";
    }
    o << output_str;
}

bool Model::model_loaded(){
    return load_successful;
}

Eigen::MatrixXi Model::get_faces(){
    return Faces;
}

double Model::intersect_ray(Eigen::Vector3d ray_pt, Eigen::Vector3d ray_dir, Eigen::Vector3d &hit_normal){
    double smallest_t = -1;

    Eigen::Vector3d a_vertex, b_vertex, c_vertex,  a_b, a_c, a_l, solution;
    Eigen::Matrix3d left_hand = Eigen::Matrix3d();
    double beta, gamma, t_value;
    bool found_intersection;
    std::set<int> intersected_faces = bounding_box->intersected_faces(ray_pt, ray_dir);
    std::cout << "returned intersected faces: \n ";
        for (auto i = intersected_faces.begin(); i != intersected_faces.end(); ++i)
            std::cout << *i << ' ';
    std::cout << "\n";
    //for (auto face_num: intersected_faces){
    for (int i=0; i<Faces.cols(); i++){
        int face_num = i;
        a_vertex = get_vertex( Faces(0, face_num) - 1 );
        b_vertex = get_vertex( Faces(1, face_num) - 1 );
        c_vertex = get_vertex( Faces(2, face_num) - 1 );

        a_b = a_vertex - b_vertex;
        a_c = a_vertex - c_vertex;
        a_l = a_vertex - ray_pt;

        left_hand << a_b(0), a_c(0), ray_dir(0),
                     a_b(1), a_c(1), ray_dir(1),
                     a_b(2), a_c(2), ray_dir(2);
        solution = left_hand.colPivHouseholderQr().solve(a_l);

        beta = solution[0];
        gamma = solution[1];
        t_value = solution[2];

        if (beta < 0 || gamma < 0){
            continue;
        }
        if (beta+gamma <= 1){
            if ( (smallest_t == -1) || (t_value < smallest_t)){
                smallest_t = t_value;
                hit_normal << FaceNormals(0, face_num),
                              FaceNormals(1, face_num),
                              FaceNormals(2, face_num);
            }
        }
 /*
        found_intersection = test_intersection(a_vertex, b_vertex, c_vertex, ray_pt, ray_dir, t_value);
        if (found_intersection){
            if ( (smallest_t == -1) || (t_value < smallest_t)){
                smallest_t = t_value;
                hit_normal << FaceNormals(0, face_num),
                              FaceNormals(1, face_num),
                              FaceNormals(2, face_num);
            }
        }
*/
    }
    return smallest_t;
}

bool Model::test_intersection(Eigen::Vector3d &vertex_a, Eigen::Vector3d &vertex_b, Eigen::Vector3d &vertex_c, Eigen::Vector3d& ray_pt, Eigen::Vector3d& ray_dir, double& t_value){
    // Moller Trumbore Algorithm
    const double EPSILON = 0.0000001;
    Eigen::Vector3d edge_ba, edge_ca, pvec, qvec, origin_a;
    double det, invDet, u,v;
    edge_ba = vertex_b - vertex_a;
    edge_ca = vertex_c - vertex_a;
    pvec = ray_dir.cross(edge_ca);
    det = pvec.dot(edge_ba);
    if (abs(det) < EPSILON){
        // This ray is parallel to this triangle
        //std::cout << "abs(det): " << abs(det) << "\n";
        return false;
    }
    invDet = 1.0 / det;
    // Check that u is in bounds
    origin_a = ray_pt - vertex_a;
    u = (origin_a.dot(pvec)) * invDet;
    std::cout << "1\n";
    if (u < 0.0 || u > 1.0)
        return false;
    std::cout << "2\n";
    qvec = origin_a.cross(edge_ba);
    v = ray_dir.dot(qvec) * invDet;
    if (v < 0.0 || (u + v) > 1.0)
        return false;
    std::cout << "3\n";
    // At this stage we can compute t to find out where the intersection point is on the line.
    double t = invDet * edge_ca.dot(qvec);
    if (t > EPSILON){
        std::cout << "4\n";
        t_value = t;
        return true;
    }
    else {
        // This means that there is a line intersection but not a ray intersection.
        return false;
    }

}

void Model::load_material(std::string material_file){
    std::ifstream in(material_file);
    if (!in){
        std::cerr << "Could not open " << material_file << std::endl;
        return;
    }
    std::string obj_line;
    std::string line_type = "";

    double k1, k2, k3;
    while (std::getline(in, obj_line)){
        std::stringstream obj_read(obj_line);
        obj_read >> line_type;

        if (line_type[0] == 'K' && line_type[1] == 'a'){
            obj_read >> k1 >> k2 >> k3;
            ambient_color(0, 0) = k1;
            ambient_color(1, 1) = k2;
            ambient_color(2, 2) = k3;
        }

        if (line_type[0] == 'K' && line_type[1] == 'd'){
            obj_read >> k1 >> k2 >> k3;
            diffuse_color(0, 0) = k1;
            diffuse_color(1, 1) = k2;
            diffuse_color(2, 2) = k3;
        }
    }

}
