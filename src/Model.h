#ifndef OBJ_INCLUDED
#define OBJ_INCLUDED

#include <string>
#include <vector>
#include "Model.h"

// Needs to link Eigen in the makefile with g++ -I/path/to/Eigen
#include <Eigen>
#include <Core>


class Model {
  public:
        Model(std::string driver_file);
        Eigen::MatrixXd get_vertices();
        Eigen::MatrixXi get_faces();
        Eigen::Vector3d get_vertex(int index);
        void save_vertices(Eigen::MatrixXd new_vs);
        void save_model(std::ostream &output);
        bool model_loaded();

  private:
        void load_model();
        void add_vertex(double vx,double vy,double vz);
        void add_face(int v1, int v2, int v3);
        void add_face_normal(Eigen::Vector3d face_normal);
        void calculate_face_normals();
        void output_formatted_float(std::ostream &o, double number, int precision);

        Eigen::MatrixXd Vertices = Eigen::MatrixXd();
        Eigen::MatrixXi Faces = Eigen::MatrixXi();
        Eigen::MatrixXd FaceNormals = Eigen::MatrixXd();
        std::string original_file;
        bool load_successful = true;
};
#endif
