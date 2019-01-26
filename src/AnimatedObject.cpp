#include "AnimatedObject.h"
#include <iostream>

AnimatedObject::AnimatedObject(std::string driver_line){
    model_to_scene = new Transformation(driver_line);
    std::string asset_name = model_to_scene->get_asset_name();
    read_directory(asset_name, model_files);

    // If asset_name does not refer to a directory model_files will not be populated
    //     assume it is an .obj file if it's not a directory
    if (model_files.size() == 0){
        model_files.push_back(asset_name);
    }
}
 
struct path_leaf_string{
    std::string operator()(const boost::filesystem::directory_entry& entry) const{
        return entry.path().leaf().string();
    }
};

void read_directory(const std::string& name, stringvec& v){
    boost::filesystem::path p(name);
    boost::filesystem::directory_iterator start(p);
    boost::filesystem::directory_iterator end;
    std::transform(start, end, std::back_inserter(v), path_leaf_string());
}

struct AnimatedObject::path_leaf_string{
    std::string operator()(const std::exprimental::filesystem::directory_entry &entry) const{
        return entry.path().leaf().string();
    }
};

void AnimatedObject::read_directory(const std::string& name, std::vector<std::string>& v){
    std::exprimental::filesystem::path p(name);
    std::exprimental::filesystem::directory_iterator start(p);
    std::exprimental::filesystem::directory_iterator end;
    std::transform(start, end, std::back_inserter(v), path_leaf_string());
}

void AnimatedObject::set_object(SceneObject* new_obj){
    current_obj = new_obj;
}

SceneObject* get_object(){
    return current_obj;
}

void AnimatedObject::advance_frame(){
    if (has_next_frame()){
        std::string model_file_name = model_files[current_model];
        current_obj = new Model(model_file_name, model_to_scene->smoothing);
        if (!current_obj->model_loaded()){
            std::cout << "MODEL NOT LOADED: " << model_file_name << "\n";
        } else{
            model_to_scene->transform_object(current_obj);
        }
        current_model++;
    }
}

bool AnimatedObject::has_next_frame(){
    return ( current_model < model_files.size() );
}
