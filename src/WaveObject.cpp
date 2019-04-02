#include "WaveObject.h"

WaveObject::WaveObject(std::string driver_line, int current_time){
    set_time(current_time);
    extract_wave_info(driver_line);
    set_bounding_box();
    generate_waves(num_waves, 12345);
}

Ray WaveObject::get_refracted_ray(Ray &ray, Eigen::Vector3d &n, Eigen::Vector3d &m){
    (void) n;
    (void) m;
    return ray;
}

double WaveObject::intersect_ray(Ray& ray, Eigen::Vector3d &hit_normal){
    // Find max and min of search
    double near_edge = 0;
    double far_edge = 1;
    if (!bounding_box->ray_intersects(ray, near_edge, far_edge))
        return MISSED_T_VALUE;

    // Step along ray between max and min
    double t_value = step_search(ray, near_edge, far_edge);
    // //std::cout << "near&far: " << near_edge << ", " << far_edge << "\n" << "ray: \n" << ray.get_dir() << "\n\n\n";
    if (t_value >= near_edge && t_value <= far_edge){
        //std::cout << "Here\n";
        t_value = refine_search(ray, t_value);
        hit_normal = find_surf_norm(ray, t_value);
    } else{
        t_value = MISSED_T_VALUE;
    }
    //std::cout << t_value << "\n";
    return t_value;
}

double WaveObject::step_search(Ray& ray, double start, double end){
    double current_pos = start;
    Eigen::Vector3d ray_pos = ray.get_ray_loc(start);
    bool start_side = is_above_wave(ray_pos);
    do {
        current_pos += search_step;
        ray_pos = ray.get_ray_loc(current_pos);
    } while (current_pos < end && is_above_wave(ray_pos) == start_side);

    return current_pos;
}

double WaveObject::refine_search(Ray &ray, double search_end){
    // Use Binary search to find a precise t_value that is close to where the intersection occurs
    double search_start = search_end - search_step;
    bool start_side = is_above_wave(ray.get_ray_loc(search_start));
    double step_middle;

    for (int i=0; i<48; i++){
        step_middle = (search_end - search_start) / 2 + search_start;
        bool middle_side = is_above_wave(ray.get_ray_loc(step_middle));
        if (middle_side == start_side){
            search_start = step_middle;
        } else{
            search_end = step_middle;
        }
    }
    return step_middle;
}

bool WaveObject::is_above_wave(Eigen::Vector3d loc){
    // Return true if loc is in the positive direction along the surface normal from the corresponding place on the wave
    double offset_one = 0;
    double offset_two = 0;
    find_offsets(loc, offset_one, offset_two);

    Eigen::Vector3d wave_location = wave_function(offset_one, offset_two);
    Eigen::Vector3d wave_p_norm = wave_location + surface_normal;
    Eigen::Vector3d wave_n_norm = wave_location - surface_normal;
    // norm() returns the magnitude of the vector
    double p_diff = (loc - wave_p_norm).norm();
    double n_diff = (loc - wave_n_norm).norm();
    return p_diff < n_diff;
}

void WaveObject::set_time(int current_time){
    time = current_time;
}

Eigen::Vector3d WaveObject::find_surf_norm(Ray &ray, double t_value){
    double epsilon = .05;
    Eigen::Vector3d loc = ray.get_ray_loc(t_value);
    double offset_one = 0;
    double offset_two = 0;
    find_offsets(loc, offset_one, offset_two);

    Eigen::Vector3d intersect_point = wave_function(offset_one, offset_two);
    Eigen::Vector3d point_one = wave_function(offset_one + epsilon, offset_two);
    Eigen::Vector3d point_two = wave_function(offset_one, offset_two + epsilon);

    Eigen::Vector3d dir_one = point_one - intersect_point;
    Eigen::Vector3d dir_two = point_two - intersect_point;
    // TODO make sure the surface normal is pointing in the right direction
    Eigen::Vector3d temp;
    temp << 0, 1, 0;
    //return temp;
    return -1 * (dir_one.cross(dir_two)).normalized();
}

void WaveObject::find_offsets(Eigen::Vector3d loc, double &offset_one, double &offset_two){
    Eigen::Vector3d shifted = loc - first_corner;
    offset_one = direction_one.dot(shifted);
    offset_two = direction_two.dot(shifted);
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
    direction_one = direction_one.normalized();
    direction_two = direction_two.normalized();
    second_corner = first_corner + side_len_one * direction_one + side_len_two * direction_two;

    surface_normal = direction_one.cross(direction_two);
    surface_normal = surface_normal.normalized();

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

    bounding_box = std::make_unique<BoundingBox>(min, max);
}

void WaveObject::find_min_max(Eigen::Vector3d &min, Eigen::Vector3d &max){
    // On the bounded 2d wave space find the min and max x,y,z values
    double safety_margin = 2;
    for (int dir_one=0; dir_one<=1; dir_one++){
        for (int dir_two=0; dir_two<=1; dir_two++){
            Eigen::Vector3d current = first_corner + dir_one*side_len_one*direction_one
                                                   + dir_two*side_len_two*direction_two;
            for (int i=1; i<=2; i++){
                int sign = 3 - i * 2;  // Extract 1 or -1
                Eigen::Vector3d with_wave = current + sign * surface_normal * wave_height * safety_margin;
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
    std::uniform_real_distribution<double> random_freq(2, 6);

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
    }
}

Eigen::Vector3d WaveObject::get_wave_direction(double angle){
    /*
    Use Rodrigue's rotation formula to rotate an orthognal unit vector around
    the normal of the wave surface.  The result is a unit vector that aims in a
    direction defined by angle on the wave plane.
    */
    Eigen::Vector3d v = direction_one;
    Eigen::Vector3d k = surface_normal;
    k = k.normalized();
    Eigen::Vector3d rotated_vector = v * cos(angle) + (k.cross(v)) * sin(angle)
                                     + k * (k.dot(v)) * (1 - cos(angle));
    return rotated_vector;
}


Eigen::Vector3d WaveObject::wave_function(double offset_one, double offset_two){
    double current_time = time / 60;
    double total = 0;
    Eigen::Vector3d point = first_corner + offset_one * direction_one + offset_two * direction_two;
    // direction one waves
    for (auto wave:component_waves){
        //double sine = sin(offset_one * offset_two + time);
        double sine = (sin(wave.direction.dot(point)*wave.frequency + current_time*wave.phase_constant)+1)/2;
        total += wave_height * wave.amplitude * pow(sine, 2);
    }
    total = total / component_waves.size();
    Eigen::Vector3d result = point + total * surface_normal;
    return result;
}
