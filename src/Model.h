#ifndef OBJ_INCLUDED
#define OBJ_INCLUDED

#include <string>
#include <vector>
#include "Model.h"
#include <limits>

// Needs to link Eigen in the makefile with g++ -I/path/to/Eigen
#include <Eigen>
#include <Core>


class Model {
  public:
        Model(std::string driver_file);
        Model();
        Eigen::MatrixXd get_vertices();
        Eigen::MatrixXi get_faces();
        Eigen::Vector3d get_vertex(int index);
        void save_vertices(Eigen::MatrixXd new_vs);
        void save_model(std::ostream &output);
        bool model_loaded();

        Eigen::Matrix3d get_diffuse_color();
        Eigen::Matrix3d get_ambient_color();
        double intersect_ray(Eigen::Vector3d ray_pt, Eigen::Vector3d ray_dir, Eigen::Vector3d &hit_normal);
  private:
        void load_model();
        void load_material(std::string material_file);
        void add_vertex(double vx,double vy,double vz);
        void add_face(int v1, int v2, int v3);
        void add_face_normal(Eigen::Vector3d face_normal);
        void calculate_face_normals();
        void calculate_bounding();
        bool intersects_bounding(Eigen::Vector3d ray_pt, Eigen::Vector3d ray_dir);
        void output_formatted_float(std::ostream &o, double number, int precision);

        Eigen::MatrixXd Vertices = Eigen::MatrixXd();
        Eigen::MatrixXi Faces = Eigen::MatrixXi();
        Eigen::MatrixXd FaceNormals = Eigen::MatrixXd();
        Eigen::Matrix3d diffuse_color = Eigen::Matrix3d().Zero();
        Eigen::Matrix3d ambient_color = Eigen::Matrix3d().Zero();
        // Bounding sphere coordinates
        Eigen::Vector3d min_point = Eigen::Vector3d();
        Eigen::Vector3d max_point = Eigen::Vector3d();
        Eigen::Vector3d center = Eigen::Vector3d();
        double radius;

        std::string original_file;
        bool load_successful = true;
};
#endif
