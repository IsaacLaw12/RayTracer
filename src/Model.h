#ifndef OBJ_INCLUDED
#define OBJ_INCLUDED

#include <string>
#include <vector>
#include <cmath>
class BoundingBox;
#include "BoundingBox.h"

// Needs to link Eigen in the makefile with g++ -I/path/to/Eigen
#include <Eigen>
#include <Core>


class Model: public SceneObject {
  public:
        Model(std::string driver_file);
        Model();
        Eigen::MatrixXd get_vertices();
        Eigen::MatrixXi get_faces();
        Eigen::Vector3d get_vertex(int index);
        void save_vertices(Eigen::MatrixXd new_vs);
        void save_model(std::ostream &output);
        bool model_loaded();

        bool test_intersection(Eigen::Vector3d &vertex_a, Eigen::Vector3d &vertex_b, Eigen::Vector3d &vertex_c, Eigen::Vector3d& ray_pt, Eigen::Vector3d& ray_dir, double& t_value);

  private:
        friend class BoundingBox;
        void load_model();
        void on_model_load();
        void load_material(std::string material_file);

        void add_vertex(double vx,double vy,double vz);
        void add_face(int v1, int v2, int v3);
        void add_face_normal(Eigen::Vector3d face_normal);

        void calculate_face_normals();
        void output_formatted_float(std::ostream &o, double number, int precision);

        BoundingBox* bounding_box;
        Eigen::MatrixXd Vertices = Eigen::MatrixXd();
        Eigen::MatrixXi Faces = Eigen::MatrixXi();
        Eigen::MatrixXd FaceNormals = Eigen::MatrixXd();

        std::string original_file;
        bool load_successful = true;
};
#endif
