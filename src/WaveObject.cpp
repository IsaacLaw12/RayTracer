#include "WaveObject.h"

WaveObject::WaveObject(Eigen::Vector3d corner_one, double len_one, double len_two, Eigen::Vector3d dir_one, Eigen::Vector3d dir_two, int res, std::string material_file, double height, int number_waves){
    // corner_one, corner_two:  Represent points in space
    // dir_one, dir_two: Should be orthogonal, defines the direction the sides go out from corner_one
    wave_object = new Model();
    wave_object->load_material(material_file);
    wave_object->set_smooth(true);
    resolution = res;
    wave_height = height;
    num_waves = number_waves;
    side_one_len = len_one;
    side_two_len = len_two;
    first_corner = corner_one;
    direction_one = dir_one.normalized();
    direction_two = dir_two.normalized();
    second_corner = len_one * dir_one + len_two * dir_two;
    generate_waves(num_waves, 1234);
}

void WaveObject::generate_waves(int num_waves, int random_seed){
    /*
    Populate component_waves with waves with random amplitude, frequency, and angle
    */
    std::default_random_engine angle_generator(random_seed);
    std::uniform_real_distribution<double> random_angle(0, 2*M_PI);

    std::default_random_engine freq_generator(random_seed * 2);
    std::uniform_real_distribution<double> random_freq(5, 10);

    std::default_random_engine ampl_generator(random_seed * 3);
    std::uniform_real_distribution<double> random_ampl(.5, 1);

    std::default_random_engine pc_generator(random_seed *4);
    std::uniform_real_distribution<double> random_pc(.5, 4);

    for(int i=0; i<num_waves; i++){
        wave_parameters current;
        current.amplitude = random_ampl(ampl_generator);
        current.frequency = random_freq(freq_generator);
        current.direction = get_wave_direction(random_angle(angle_generator));
        current.phase_constant = random_pc(pc_generator);
        component_waves.push_back(current);
        std::cout << "wave_frequency: " << current.frequency << " wave.amplitude: " << current.amplitude << "\n";
        std::cout << "direction: \n" << current.direction << "\n";
    }/*
    wave_parameters current;
    current.amplitude = 1;
    current.frequency = 15;
    current.direction = Eigen::Vector3d(1, 0, 0);
    component_waves.push_back(current);
*/
}

Eigen::Vector3d WaveObject::get_wave_direction(double angle){
    /*
    Use Rodrigue's rotation formula to rotate an orthognal unit vector around
    the normal of the wave surface.  The result is a unit vector that aims in a
    direction defined by angle on the wave plane.
    */
    Eigen::Vector3d v = direction_one;
    Eigen::Vector3d k = direction_one.cross(direction_two);
    k = k.normalized();
    Eigen::Vector3d rotated_vector = v * cos(angle) + (k.cross(v)) * sin(angle)
                                     + k * (k.dot(v)) * (1 - cos(angle));
    return rotated_vector;
}

void WaveObject::update_model(){
    // Calculate the wave_function displacement on a grid of points, save vertices and faces to model
    double side_one = 0;
    double side_two = 0;
    find_side_lengths(side_one, side_two);
    double cell_size = find_grid_cell_size();

    Eigen::MatrixXd vertices = Eigen::MatrixXd();
    int vertex_num = 0;
    for (int i=0; (i*cell_size)<=side_one; i++){
        for (int j=0; (j*cell_size)<=side_two; j++){
            if (vertices.cols() <= vertex_num){
                vertices.conservativeResize(3, vertex_num+1);
            }
            Eigen::Vector3d point_in_space = first_corner + direction_one * i * cell_size + direction_two * j * cell_size;
            point_in_space += wave_function(i*cell_size, j*cell_size, current_frame);
            vertices.col(vertex_num++) << point_in_space(0), point_in_space(1), point_in_space(2);
        }
    }

    std::cout << "vertices size: " << vertices.rows() << "x" << vertices.cols()  << "\n";
    int row_length = side_one / cell_size + 1; // +1  because a row is number of cells surrounded by vertices on both sides
    Eigen::MatrixXi faces = create_faces(vertices, row_length);
    std::cout << "Faces size: " << faces.rows() << "x" << faces.cols() << "\n";
    wave_object->set_vertices_faces(vertices, faces);
}

void WaveObject::set_height(double height){
    wave_height = height;
}

void WaveObject::set_num_waves(int waves){
    num_waves = waves;
}

Eigen::Vector3d WaveObject::wave_function(int offset_one, int offset_two, double time){
    time = time / 60;
    double total = 0;
    Eigen::Vector3d point = first_corner + offset_one * direction_one + offset_two * direction_two;
    // direction one waves
    for (auto wave:component_waves){
        double sine = (sin(wave.direction.dot(point)*wave.frequency + time*wave.phase_constant) +1)/2;
        total += wave_height * wave.amplitude * sine;//pow(sine, 16);
    }
    total = total / component_waves.size();
    Eigen::Vector3d result = Eigen::Vector3d(0, 0, 0);
    //std::cout << "TOTAL SINE FUNC: " << total << "\n";
    result += total * (direction_one.cross(direction_two));
    return result;
}

double WaveObject::find_grid_cell_size(){
    double first_length = 0;
    double second_length = 0;
    find_side_lengths(first_length, second_length);
    // Divide the smaller side by resolution to find the cell size
    if (first_length < second_length){
        return first_length / resolution;
    } else{
        return second_length / resolution;
    }
}

void WaveObject::find_side_lengths(double& side_one, double& side_two){
    // Find the length of the sides of the rectangle defined by the corners
    Eigen::Vector3d diagonal = second_corner - first_corner;
    side_one = direction_one.dot(diagonal);
    side_two = direction_two.dot(diagonal);
}

Eigen::MatrixXi WaveObject::create_faces(Eigen::MatrixXd &vertices, int row_length){
    // A face is just a 3-column in the faces matrix with the index number of three vertices in the their matrix
    Eigen::MatrixXi faces = Eigen::MatrixXi();
    for (int i=row_length; i<vertices.cols(); i++){
        // In a grid of points these triangles have a 90 in the NE corner of each square
        if ((i+1) % row_length != 0)
            add_face(faces, i, i+1, i-row_length);
        // These triangles have a 90 in the SW corner of each square
        if (i%row_length != 0)
            add_face(faces, i, i-row_length, i-row_length-1);
    }
    return faces;
}

void WaveObject::add_face(Eigen::MatrixXi &faces, int a, int b, int c){
    // Face numbers are stored as base 1, assume a,b,c are 0 zero based and convert
    int num_cols = faces.cols();
    faces.conservativeResize(3, num_cols+1);
    faces.col(num_cols) << a + 1, b + 1, c + 1;
}

void WaveObject::advance_frame(){
    update_model();
    current_frame++;
}

Model* WaveObject::get_object(){
    std::cout << "Returning wave object\n";
    return wave_object;
}
