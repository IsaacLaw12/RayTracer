#include <iostream>
#include "Scene.h"
#include "RenderImage.h"

std::string SAVE_FOLDER = "./tmp_renders/";

std::string strip_file_ending(std::string original){
    std::size_t found = original.find(".");
    if (found != std::string::npos){
        original = original.substr(0, found);
    }
    return original;
}

void prepare_directory(){
    // Clear the saved render folder of previous contents
    std::cout << "Removing " << SAVE_FOLDER << " directory\n";
    char remove_command[80] = "rm -r ";
    strcat(remove_command, SAVE_FOLDER.c_str());
    if (0 == system(remove_command)){
        std::cout << "Directory removal successful\n";
    }

    std::cout << "Creating " << SAVE_FOLDER << " directory\n";
    char create_command[80] = "mkdir ";
    strcat(create_command, SAVE_FOLDER.c_str());
    if (0 == system(create_command)){
        std::cout << "Directory creation successful\n";
    } else{
        std::cout << "Directory creation failed\n";
    }
}

int main(int argc, char*argv[]){
    if (argc != 3){
        std::cout << "Error: run with " << argv[0] << " <driver_file> <save_image_file>" << std::endl;
        return 1;
    }
    std::string driver_file = argv[1];
    std::string save_image_file = argv[2];

    Scene sc(driver_file);
    prepare_directory();
    save_image_file = strip_file_ending(save_image_file);

    while (sc.advance_frame()){
        std::string padded_frame_num = std::to_string(sc.get_current_frame());
        while (padded_frame_num.size() < 4){
            padded_frame_num = "0" + padded_frame_num;
        }
        std::string image_name = SAVE_FOLDER + save_image_file + padded_frame_num + ".ppm";
        RenderImage ri = RenderImage(&sc);
        ri.render_image(image_name);
    }
    return 0;
}
