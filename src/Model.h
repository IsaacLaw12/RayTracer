#ifndef OBJ_INCLUDED
#define OBJ_INCLUDED

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <cmath>
class BoundingBox;
#include "BoundingBox.h"
#include "SceneObject.h"
#include "Ray.h"
#include "Transformation.h"

// Needs to link Eigen in the makefile with g++ -I/path/to/Eigen
#include <Eigen>
#include <Core>


class Model: public SceneObject {
  public:
        Model(std::string driver_line);
        Model(const Model&old_model);
        Model();
        Eigen::MatrixXd get_vertices();
        Eigen::MatrixXi get_faces();
        Eigen::Vector3d get_vertex(int index);
        Eigen::Vector3d get_face_normal(int index);
        Eigen::Vector3d get_smooth_face_normal(int face_num, double beta, double gamma);
        Eigen::Vector3d get_vertex_normal(int face_num, int face_vert);
        std::string get_animation_file();
        bool is_animated();
        void set_smooth(bool smooth);
        void set_vertices(Eigen::MatrixXd new_vs);
        void set_vertices_faces(Eigen::MatrixXd new_vs, Eigen::MatrixXi new_fs);
        bool model_loaded();

        double intersect_ray(Ray& ray, Eigen::Vector3d &hit_normal);
        Ray get_refracted_ray(Ray &orig_ray, Eigen::Vector3d &intersect_pos, Eigen::Vector3d &normal);
        void get_refract_exit(Eigen::Vector3d &intersect_pos, Eigen::Vector3d &refract_ray, Eigen::Vector3d &exit, Eigen::Vector3d& exit_normal);
        bool test_intersection(Eigen::Vector3d &vertex_a, Eigen::Vector3d &vertex_b, Eigen::Vector3d &vertex_c, Ray& ray, double& t_value);

  private:
        friend class BoundingBox;
        void load_model();
        void map_vertices_faces();
        void on_model_load();

        void add_vertex(double vx, double vy, double vz);
        void add_face(int v1, int v2, int v3);
        void add_face_normal(Eigen::Vector3d face_normal);

        void calc_vertex_normal(int vertex_num, const Eigen::Vector3d& original_face, Eigen::Vector3d& average);
        void calculate_vertex_normals();
        void calculate_face_normals();

        std::unique_ptr<BoundingBox> bounding_box;
        Eigen::MatrixXd Vertices = Eigen::MatrixXd();
        Eigen::MatrixXi Faces = Eigen::MatrixXi();
        Eigen::MatrixXd FaceNormals = Eigen::MatrixXd();

        std::vector<std::vector<int>> vertex_to_faces;
        // Index will be face_num*3 + relative vertex face number (0-2)
        std::vector<Eigen::Vector3d> vertex_normals;

        std::string original_file;
        std::string animation_file;
        bool load_successful = true;
        bool smoothing = false;
};
#endif
