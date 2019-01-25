#include "AnimatedObject.h"
#include <iostream>
#include <filesystem>

AnimatedObject::AnimatedObject(std::string driver_line){
    model_to_scene = Transformation(driver_line);
    std::string asset_name = model_to_scene.get_asset_name();
    read_directory(asset_name, model_files);

    // If asset_name does not refer to a directory model_files will not be populated
    //     assume it is an .obj file if it's not a directory
    if (model_files.size() == 0){
        model_files.push_back(asset_name);
    }
}

struct AnimatedObject::path_leaf_string{
    std::string operator()(const std::filesystem::directory_entry& entry) const{
        return entry.path().leaf().string();
    }
};

void AnimatedObject::read_directory(const std::string& name, stringvec& v){
    std::filesystem::path p(name);
    std::filesystem::directory_iterator start(p);
    std::filesystem::directory_iterator end;
    std::transform(start, end, std::back_inserter(v), path_leaf_string());
}

void AnimatedObject::set_object(SceneObject* new_obj){
    current_obj = new_obj;
}

void AnimatedObject::advance_frame(){
    if (has_next_frame()){

        current_model++;
    }
}

bool AnimatedObject::has_next_frame(){
    return ( current_model < model_files.size() );
}
