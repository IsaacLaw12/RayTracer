#include "AnimatedObject.h"
#include <iostream>

AnimatedObject::AnimatedObject(Model* animated_asset){
     original_obj = animated_asset;
     current_obj = new Model(*original_obj);
     animation_file = animated_asset->get_animation_file();
     load_animation();
}

AnimatedObject::AnimatedObject(){
  // An empty constructor that children like WaveObject can call
  return;
}

void AnimatedObject::load_animation(){
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

void AnimatedObject::add_animation_section(int section_start, int section_end,  std::vector<double> transform_floats){
    int number_frames = section_end - section_start;
    if (number_frames == 0){
        return;
    }
    std::vector<double> single_frame_transform = fraction_of_transform(number_frames, transform_floats);
    for (int i=1; i<number_frames+1; i++){
        frame_setting current;
        current.frame_number = section_start + i - 1;
        current.transform_floats = multiple_of_transform(i, single_frame_transform);
        current.set_as_new_base_model = (i == number_frames);
        frame_steps.push_back(current);
    }
}

std::vector<double> AnimatedObject::fraction_of_transform(double divide_by, std::vector<double> transform_floats){
    // transform_floats should be 8 long
    std::vector<double> result;
    for (int i=0; i<8; i++){
        double temp = transform_floats[i];
        // Scale value defaults to 1
        if (i==4){
            temp = 1 + (temp-1) / divide_by;
        }
        // Transform values default to 0
        if (i>4 || i==3){
            temp = temp / divide_by;
        }
        result.push_back(temp);
    }
    return result;
}

std::vector<double> AnimatedObject::multiple_of_transform(double multiply_by, std::vector<double> transform_floats){
    return fraction_of_transform(1/multiply_by, transform_floats);
}

std::vector<std::string> AnimatedObject::extract_file_lines(std::string file_name){
    std::ifstream in(file_name);
    std::string driver_line;
    std::vector<std::string> result;

    while (std::getline(in, driver_line)){
        // An arbitrary check to make sure the line has contents
        if (driver_line.size() > 10){
            result.push_back(driver_line);
        }
    }
    return result;
}

void AnimatedObject::set_object(Model* new_obj){
    current_obj = new_obj;
}

void AnimatedObject::set_start_frame(unsigned starting_frame){
    current_frame = 0;
    while (current_frame < starting_frame){
        advance_frame();
    }
}

SceneObject* AnimatedObject::get_object(){
    return current_obj;
}

void AnimatedObject::advance_frame(){
    // Load the next model in the directory if there are any left
    if (has_next_frame()){
          frame_setting this_transform = frame_steps[current_frame];
          Transformation frame_transform(this_transform.transform_floats);
          delete current_obj;
          current_obj = new Model(*original_obj);
          frame_transform.transform_object(current_obj);
          if (this_transform.set_as_new_base_model){
              delete original_obj;
              original_obj = new Model(*current_obj);
          }

        current_frame++;
    }
}

bool AnimatedObject::has_next_frame(){
    return current_frame < frame_steps.size();
}

void AnimatedObject::reset_current_frame(){
    current_frame = 0;
}
