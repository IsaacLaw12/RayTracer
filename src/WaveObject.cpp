#include "WaveObject.h"

WaveObject::WaveObject(std::string driver_line, int current_time){
    set_time(current_time);
    extract_wave_info(driver_line);
    set_bounding_box();
    generate_waves(num_waves, 1234);
}

WaveObject::intersect_ray(Ray& ray, Eigen::Vector3d &hit_normal){
    // Find max and min of search
    double near_edge = 0;
    double far_edge = 1;
    if (!bounding_box.ray_intersects(ray, near_edge, far_edge))
        return MISSED_T_VALUE;

    // Step along ray between max and min

    // Test for having passed to the other side of the wave function then binary search for a more precise value
}

void WaveObject::set_time(int current_time){
    time = current_time;
}

void WaveObject::extract_wave_info(std::string driver_line){
    // wave  corner1 side_length side_length dir1 dir2 res height  num_waves material
    std::stringstream d_line(driver_line);
    std::string line_type, material_file;
    double c1x, c1y, c1z, len_one, len_two, d1x, d1y, d1z, d2x, d2y, d2z, res, height;
    int n_waves;
    d_line >>line_type>>c1x>>c1y>>c1z>>len_one>>len_two>>d1x>>d1y>>d1z>>d2x>>d2y>>d2z>>res>>height>>n_waves>>material_file;

    side_len_one = len_one;
    side_len_two = len_two;
    first_corner = Eigen::Vector3d(c1x, c1y, c1z);
    direction_one = Eigen::Vector3d(d1x, d1y, d1z);
    direction_two = Eigen::Vector3d(d2x, d2y, d2z);
    direction_one = dir_one.normalized();
    direction_two = dir_two.normalized();
    second_corner = first_corner + side_len_one * dir_one + side_len_two * dir_two;
    wave_height = height;
    num_waves = n_waves;
    search_step = 1 / res;

    load_material(material_file);
}

void WaveObject::set_bounding_box(){
    // Find the coordinates bounding the the wave surface
    Eigen::Vector3d min = first_corner;
    Eigen::Vector3d max = first_corner;
    find_min_max(min, max);

    bounding_box = BoundingBox(min, max);
}

void WaveObject::find_min_max(Eigen::Vector3d &min, Eigen::Vector3d &max){
    // On the bounded 2d wave space find the min and max x,y,z values
    Eigen::Vector3d surface_normal = direction_one.cross(direction_two);
    surface_normal = surface_normal.normalized();

    for (int dir_one=0; dir_one<=1; dir_one++){
        for (int dir_two=0; dir_two<=1; dir_two++){
            Eigen::Vector3d current = first_corner + dir_one*side_len_one*direction_one
                                                   + dir_two*side_len-two*direction_two;
            for (int i=1; i<=2; i++){
                int sign = 3 - i * 2;
                Eigen::Vector3d with_wave = current + sign * surface_normal * wave_height;
                extract_vector_extremes(with_wave, min, max);
            }

        }
    }
}

void WaveObject::extract_vector_extremes(Eigen::Vector3d original, Eigen::Vector3d &min, Eigen::Vector3d &max){
    // Iterate original and elementwise update min and max if a smaller or larger value is found
    for (int i=0; i<=2; i++){
        if (original(i) < min(i))
            min(i) = original(i);
        if (original(i) > max(i))
            max(i) = original(i);
    }
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
    }
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
    Eigen::Vector3d result = total * (direction_one.cross(direction_two));
    return result;
}
