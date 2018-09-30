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
    while (std::getline(in, obj_line)){
        std::stringstream vertice_read(obj_line);
        vertice_read >> line_type;
        if (line_type[0] == 'v' && line_type.size() == 1){
            vertice_read >> vx >> vy >> vz;
            add_vertice(vx, vy, vz);
        }
    }

    if (Vertices.cols() == 0){
        // No vertices were found
        load_successful = false;
    }
    in.close();
}

void Model::add_vertice(double vx, double vy, double vz){
    int num_cols = Vertices.cols();
    Vertices.conservativeResize(4, num_cols+1);
    Vertices.col(num_cols) << vx, vy, vz, 1;
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

class Face {

};
