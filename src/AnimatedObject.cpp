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
    } else{
        directory_name = asset_name;
    }
    if (!filter_obj_files()){
        std::cout << "Is not an .obj file or a folder containing .obj files: " << asset_name << "\n";
    }
}

AnimatedObject::AnimatedObject(){
  // An empty constructor that children like WaveObject can call
  return;
}

bool AnimatedObject::filter_obj_files(){
    std::vector<std::string> temp_files;
    std::string obj = ".obj";
    for (auto mf:model_files){
        if (mf.length() <= obj.length())
          continue;
        if (0 == mf.compare(mf.length() - obj.length(), obj.length(), obj) ){
            temp_files.push_back(mf);
        }
    }
    model_files = temp_files;
    return model_files.size() > 0;
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

void AnimatedObject::set_object(Model* new_obj){
    current_obj = new_obj;
}

Model* AnimatedObject::get_object(){
    std::cout << "returning current obj\n";
    return current_obj;
}

void AnimatedObject::advance_frame(){
    // Load the next model in the directory if there are any left
    if (has_next_frame()){
        std::string model_file_name = get_model_file_name(current_frame);
        current_obj = new Model(model_file_name, model_to_scene->get_smoothing());
        current_obj->set_lighting_group(model_to_scene->get_lighting_group());
        if (!current_obj->model_loaded()){
            std::cout << "MODEL NOT LOADED: " << model_file_name << "\n";
        } else{
            Model* model_cast = (Model*) current_obj;
            model_to_scene->transform_object(model_cast);
        }
        current_frame++;
    }
}

std::string AnimatedObject::get_model_file_name(int model_index){
    // If the .obj asset is contained in a directory append the file name with the directory name
    std::string temp = "";
    if (directory_name.length()){
        temp += directory_name + "/";
    }
    temp += model_files[model_index];
    return temp;
}

bool AnimatedObject::has_next_frame(){
    return ( current_frame < model_files.size() );
}
