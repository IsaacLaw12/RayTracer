#include <iostream>
#include <fstream>
#include "Model.h"
#include <sstream>
#include <iomanip>


Model::Model(std::string driver_line){
    Transformation model_to_scene = Transformation(driver_line);
    original_file = model_to_scene.get_asset_name();
    load_model();
    model_to_scene.transform_object(this);
    smoothing = model_to_scene.get_smoothing();
    animation_file = model_to_scene.get_animation_file();
    on_model_load();
}

Model::Model(const Model&old_model){
    // AFTER COPYING on_model_load() NEEDS TO BE CALLED
    Vertices = old_model.Vertices;
    Faces = old_model.Faces;
    FaceNormals = old_model.FaceNormals;
    vertex_to_faces = old_model.vertex_to_faces;
    vertex_normals = old_model.vertex_normals;
    original_file = old_model.original_file;
    animation_file = old_model.animation_file;
    smoothing = old_model.smoothing;
}

Model::Model(){}

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
        if(obj_line.empty()){
            continue;
        }
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
}

void Model::map_vertices_faces(){
    int num_verts = Vertices.cols();
    std::vector<std::vector<int>> temp(num_verts);
    vertex_to_faces = temp;

    for (int face_num=0; face_num<Faces.cols(); face_num++){
        for (int j=0; j<3; j++){
            int vert = Faces(j, face_num);
            vertex_to_faces[vert-1].push_back(face_num);
        }
    }
}

void Model::on_model_load(){
    map_vertices_faces();
    calculate_face_normals();
    int recursion_depth = 8;
    std::cout << "Building octtree\n";
    bounding_box = new BoundingBox(Vertices, Faces, recursion_depth);
    calculate_vertex_normals();
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
    /*
      Calculate the normal for each face by taking the cross product of two of the triangle's sides
    */
    FaceNormals = Eigen::MatrixXd();
    Eigen::Vector3d first_vertex, second_vertex, third_vertex;
    Eigen::Vector3d first_vector, second_vector;
    for (int i=0; i<Faces.cols(); i++){
        first_vertex << get_vertex( Faces(0, i) - 1 );
        second_vertex << get_vertex( Faces(1, i) - 1 );
        third_vertex << get_vertex( Faces(2, i) - 1 );
        // Calculate normal in counter clockwise direction?
        first_vector = first_vertex - third_vertex;
        second_vector = first_vertex - second_vertex;

        add_face_normal((-1*first_vector.cross(second_vector)).normalized());
    }
}

void Model::calculate_vertex_normals(){
    /* Assumes that calculate_face_normals was called first
      For every face calculate and store the vertex normal for its three vertices
    */
    int num_vert_norms = Faces.cols()*3;
    std::vector<Eigen::Vector3d> temp(num_vert_norms);
    vertex_normals = temp;
    Eigen::Vector3d original, average;
    for (int face_num=0; face_num<Faces.cols(); face_num++){
        original = get_face_normal(face_num);
        for (int f_vert=0; f_vert<3; f_vert++){
            int vertex_num = Faces(f_vert, face_num);
            average = get_face_normal(face_num);
            calc_vertex_normal(vertex_num, original, average);
            vertex_normals[3*face_num + f_vert] = average;
        }
    }
}

void Model::calc_vertex_normal(int vertex_num, const Eigen::Vector3d& original_face, Eigen::Vector3d& average){
    int num_to_average = 1;
    Eigen::Vector3d conn_face_normal;
    for (auto connected_face:vertex_to_faces[vertex_num-1]){
        conn_face_normal = get_face_normal(connected_face);
        double angle = conn_face_normal.dot(original_face);
        // If angle between face normals is less than 22.5 degrees then smooth between them
        if (angle <= 1 && angle >=.9239){
            average = average + conn_face_normal;
            num_to_average++;
        }
    }
    average = average.normalized();
}

Eigen::Vector3d Model::get_vertex(int index){
    Eigen::Vector3d vertex = Eigen::Vector3d(Vertices(0, index), Vertices(1, index), Vertices(2, index));
    return vertex;
}

Eigen::Vector3d Model::get_face_normal(int index){
    Eigen::Vector3d normal = Eigen::Vector3d(FaceNormals(0, index), FaceNormals(1, index), FaceNormals(2, index));
    return normal;
}

Eigen::Vector3d Model::get_smooth_face_normal(int face_num, double beta, double gamma){
    Eigen::Vector3d normal_one, normal_two, normal_three;
    normal_one = get_vertex_normal(face_num, 0);
    normal_two = get_vertex_normal(face_num, 1);
    normal_three = get_vertex_normal(face_num, 2);
    return ((1-beta-gamma) * normal_one + beta*normal_two + gamma*normal_three).normalized();
}

Eigen::Vector3d Model::get_vertex_normal(int face_num, int face_vert){
    return vertex_normals[face_num*3 + face_vert];
}

std::string Model::get_animation_file(){
    return animation_file;
}

bool Model::is_animated(){
    return animation_file.size() > 0;
}

Eigen::MatrixXd Model::get_vertices(){
    return Vertices;
}

void Model::set_smooth(bool smooth){
    smoothing = smooth;
}

void Model::set_vertices(Eigen::MatrixXd new_vs){
    Vertices = new_vs;
    on_model_load();
}

void Model::set_vertices_faces(Eigen::MatrixXd new_vs, Eigen::MatrixXi new_fs){
    Faces = new_fs;
    set_vertices(new_vs);
}

bool Model::model_loaded(){
    return load_successful;
}

Eigen::MatrixXi Model::get_faces(){
    return Faces;
}

double Model::intersect_ray(Ray& ray, Eigen::Vector3d &hit_normal){
    double smallest_t = MISSED_T_VALUE;
    const Eigen::Vector3d& ray_dir = ray.get_dir();
    Eigen::Vector3d a_vertex, b_vertex, c_vertex,  a_b, a_c, a_l, solution;
    Eigen::Matrix3d M = Eigen::Matrix3d();
    Eigen::Matrix3d MMs1, MMs2, MMs3;
    double beta, gamma, t_value;
    std::set<int> intersected_faces = bounding_box->intersected_faces(ray);

    //std::cout << "Intersected_faces: "<<intersected_faces.size() << "\n";
    //std::cout << "returned intersected faces: \n ";
    //    for (auto i = intersected_faces.begin(); i != intersected_faces.end(); ++i)
    //        std::cout << *i << ' ';
    //std::cout << "\n";
    for (auto face_num: intersected_faces){
    //for (int face_num=0; face_num<Faces.cols(); face_num++){
        a_vertex = get_vertex( Faces(0, face_num) - 1 );
        b_vertex = get_vertex( Faces(1, face_num) - 1 );
        c_vertex = get_vertex( Faces(2, face_num) - 1 );

        a_b = a_vertex - b_vertex;
        a_c = a_vertex - c_vertex;
        a_l = a_vertex - ray.get_point();

        M << a_b(0), a_c(0), ray_dir(0),
             a_b(1), a_c(1), ray_dir(1),
             a_b(2), a_c(2), ray_dir(2);
        MMs1 = Eigen::Matrix3d(M);
        MMs2 = Eigen::Matrix3d(M);
        MMs3 = Eigen::Matrix3d(M);
        for (int i=0; i<3; i++){
            MMs1(i,0) = a_l(i);
            MMs2(i,1) = a_l(i);
            MMs3(i,2) = a_l(i);
        }
        beta = MMs1.determinant() / M.determinant();
        if (beta < 0 || beta > 1){
            continue;
        }
        gamma = MMs2.determinant() / M.determinant();
        if (gamma < 0 || beta+gamma > 1){
            continue;
        }

        t_value = MMs3.determinant() / M.determinant();
        if ((t_value < smallest_t) && (t_value > 0.00001)){
            smallest_t = t_value;
            if (smoothing){
                hit_normal = get_smooth_face_normal(face_num, beta, gamma);
            } else{
                hit_normal = get_face_normal(face_num);
            }

            if (hit_normal.dot(ray_dir) > 0)
                hit_normal = -1 * hit_normal;
        }
    }

    return smallest_t;
}

Ray Model::get_refracted_ray(Ray &orig_ray, Eigen::Vector3d &intersect_pos, Eigen::Vector3d &normal){
    Eigen::Vector3d T1, T2;
    double this_eta = get_eta();
    Eigen::Vector3d W = -1 * orig_ray.get_dir();
    T1 = refract_ray(W, normal, 1.0, this_eta);
    Eigen::Vector3d exit, Nin;
    get_refract_exit(intersect_pos, T1, exit, Nin);
    T1 = -1 * T1;
    T2 = refract_ray(T1, Nin, this_eta, 1.0);
    Ray exit_ray = Ray(exit, T2);
    return exit_ray;
}

void Model::get_refract_exit(Eigen::Vector3d &intersect_pos, Eigen::Vector3d &refract_ray, Eigen::Vector3d &exit, Eigen::Vector3d& exit_normal){
    Ray refracted = Ray(intersect_pos, refract_ray);
    double t_value = intersect_ray(refracted, exit_normal);
    exit = intersect_pos + refract_ray * t_value;
}

bool Model::test_intersection(Eigen::Vector3d &vertex_a, Eigen::Vector3d &vertex_b, Eigen::Vector3d &vertex_c, Ray& ray, double& t_value){
    // Moller Trumbore Algorithm
    const double EPSILON = 0.0000001;
    Eigen::Vector3d edge_ba, edge_ca, pvec, qvec, origin_a;
    double det, invDet, u,v;
    edge_ba = vertex_b - vertex_a;
    edge_ca = vertex_c - vertex_a;
    pvec = ray.get_dir().cross(edge_ca);
    det = pvec.dot(edge_ba);
    if (abs(det) < EPSILON){
        // This ray is parallel to this triangle
        //std::cout << "abs(det): " << abs(det) << "\n";
        return false;
    }
    invDet = 1.0 / det;
    // Check that u is in bounds
    origin_a = ray.get_point() - vertex_a;
    u = (origin_a.dot(pvec)) * invDet;
    std::cout << "1\n";
    if (u < 0.0 || u > 1.0)
        return false;
    std::cout << "2\n";
    qvec = origin_a.cross(edge_ba);
    v = ray.get_dir().dot(qvec) * invDet;
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
    std::cout << "LOADING MATERIAL: " << material_file << "\n";
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

        if (line_type[0] == 'K' && line_type[1] == 's'){
            obj_read >> k1 >> k2 >> k3;
            specular_color(0, 0) = k1;
            specular_color(1, 1) = k2;
            specular_color(2, 2) = k3;
        }

        if (line_type[0] == 'K' && line_type[1] == 'o'){
            obj_read >> k1 >> k2 >> k3;
            refract_color(0, 0) = k1;
            refract_color(1, 1) = k2;
            refract_color(2, 2) = k3;
        }

        if (line_type[0] == 'e'){
            if (line_type[1] == 't'){
                if (line_type[2] == 'a'){
                    obj_read >> k1;
                    eta = k1;
                }
            }
        }

        if (line_type[0] == 'N' && line_type[1] == 's'){
            obj_read >> phong;
        }

    }

}
