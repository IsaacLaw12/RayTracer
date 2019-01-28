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

void AnimatedObject::read_directory(const std::string& name, std::vector<std::string>& v){
    DIR* dirp = opendir(name.c_str());
    if (dirp == NULL){
        return;
    } else{
        struct dirent * dp;
        while ((dp = readdir(dirp)) != NULL) {
            v.push_back(dp->d_name);
        }
        closedir(dirp);
    }
}

void AnimatedObject::set_object(SceneObject* new_obj){
    current_obj = new_obj;
}

SceneObject* AnimatedObject::get_object(){
    return current_obj;
}

void AnimatedObject::advance_frame(){
    if (has_next_frame()){
        std::string model_file_name = model_files[current_model];
        current_obj = new Model(model_file_name, model_to_scene->get_smoothing());
        current_obj->set_lighting_group(model_to_scene->get_lighting_group());
        if (!current_obj->model_loaded()){
            std::cout << "MODEL NOT LOADED: " << model_file_name << "\n";
        } else{
            Model* temp = (Model*) current_obj;
            model_to_scene->transform_object(temp);
        }
        current_model++;
    }
}

bool AnimatedObject::has_next_frame(){
    return ( current_model < model_files.size() );
}
