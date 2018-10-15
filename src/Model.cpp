#include <iostream>
#include <fstream>
#include "Model.h"
#include <sstream>
#include <iomanip>

Model::Model(std::string object_file){
    original_file = object_file;
    load_model();
}

void Model::load_model(){
    std::ifstream in(original_file);
    if (!in){
        std::cerr << "Could not open " << original_file << std::endl;
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
    }

   calculate_face_normals();

    if (Vertices.cols() == 0){
        // No vertices were found
        load_successful = false;
    }
    in.close();
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
    unsigned int index_of_period = output_str.find('.');
    if (index_of_period == std::string::npos){
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

Eigen::MatrixXi Model:get_faces(){
    return Faces;
}
