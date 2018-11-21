#ifndef OBJ_INCLUDED
#define OBJ_INCLUDED

#include <string>
#include <vector>
#include <map>
#include <cmath>
class BoundingBox;
#include "BoundingBox.h"
#include "SceneObject.h"

// Needs to link Eigen in the makefile with g++ -I/path/to/Eigen
#include <Eigen>
#include <Core>


class Model: public SceneObject {
  public:
        Model(std::string driver_file, std::string smoothing);
        Model();
        Eigen::MatrixXd get_vertices();
        Eigen::MatrixXi get_faces();
        Eigen::Vector3d get_vertex(int index);
        Eigen::Vector3d get_face_normal(int index);
        Eigen::Vector3d get_smooth_face_normal(int face_num, double beta, double gamma);
        Eigen::Vector3d get_vertex_normal(int face_num, int face_vert);
        void save_vertices(Eigen::MatrixXd new_vs);
        bool model_loaded();

        double intersect_ray(Eigen::Vector3d ray_pt, Eigen::Vector3d ray_dir, Eigen::Vector3d &hit_normal);
        bool test_intersection(Eigen::Vector3d &vertex_a, Eigen::Vector3d &vertex_b, Eigen::Vector3d &vertex_c, Eigen::Vector3d& ray_pt, Eigen::Vector3d& ray_dir, double& t_value);

  private:
        friend class BoundingBox;
        void load_model();
        void map_vertices_faces();
        void on_model_load();
        void load_material(std::string material_file);

        void add_vertex(double vx, double vy, double vz);
        void add_face(int v1, int v2, int v3);
        void add_face_normal(Eigen::Vector3d face_normal);

        void calc_vertex_normal(int vertex_num, const Eigen::Vector3d& original_face, Eigen::Vector3d& average);
        void calculate_vertex_normals();
        void calculate_face_normals();

        BoundingBox* bounding_box;
        Eigen::MatrixXd Vertices = Eigen::MatrixXd();
        Eigen::MatrixXi Faces = Eigen::MatrixXi();
        Eigen::MatrixXd FaceNormals = Eigen::MatrixXd();

        std::vector<std::vector<int>> vertex_to_faces;
        // Index will be face_num*3 + relative vertex face number (0-2)
        std::vector<Eigen::Vector3d> vertex_normals;

        std::string original_file;
        bool load_successful = true;
        bool smoothing = false;
        double MISSED_T_VALUE = 1000000000;
};
#endif
