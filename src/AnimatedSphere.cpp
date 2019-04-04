#include "AnimatedSphere.h"
#include <iostream>

AnimatedSphere::AnimatedSphere(Sphere* animated_asset){
     sphere = animated_asset;
     animation_file = animated_asset->get_animation_file();
     load_animation();
}

void AnimatedSphere::load_animation(){
    std::vector<std::string> animation_steps = extract_file_lines(animation_file);
    int section_start = 0;
    int section_end = 0;
    for (std::string animation_step:animation_steps){
        std::stringstream driver(animation_step);
        driver >> section_end;
        std::vector<double> transform_floats;
        double temp = 0;
        while (driver >> temp){
            transform_floats.push_back(temp);
        }
        add_animation_section(section_start, section_end, transform_floats);
        section_start = section_end;
    }
}

void AnimatedSphere::add_animation_section(int section_start, int section_end,  std::vector<double> transform_floats){
    int number_frames = section_end - section_start;
    if (number_frames == 0){
        return;
    }
    std::vector<double> single_frame_transform = fraction_of_transform(number_frames, transform_floats);
    for (int i=1; i<number_frames+1; i++){
        sp_frame_setting current;
        current.frame_number = section_start + i - 1;
        current.center_change = Eigen::Vector3d(single_frame_transform[0], single_frame_transform[1], single_frame_transform[2]);
        current.radius_change = single_frame_transform[3];
        frame_steps.push_back(current);
    }
}

std::vector<double> AnimatedSphere::fraction_of_transform(double divide_by, std::vector<double> transform_floats){
    // transform_floats should be 8 long
    std::vector<double> result;
    for (auto i:transform_floats)
        std::cout << i << std::endl;
    for (int i=0; i<4; i++){
        double temp = transform_floats[i];
        temp = temp / divide_by;
        result.push_back(temp);
    }
    return result;
}

void AnimatedSphere::set_object(Sphere* new_obj){
    sphere = new_obj;
}

SceneObject* AnimatedSphere::get_object(){
    return sphere;
}

void AnimatedSphere::advance_frame(){
    if (has_next_frame()){
        sp_frame_setting this_transform = frame_steps[current_frame];
        sphere->add_to_center(this_transform.center_change);
        sphere->add_to_radius(this_transform.radius_change);
        current_frame++;
    }
}

bool AnimatedSphere::has_next_frame(){
    return current_frame < frame_steps.size();
}
