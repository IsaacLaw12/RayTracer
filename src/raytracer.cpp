#include <iostream>
#include "Scene.h"

int main(int argc, char*argv[]){
    if (argc != 3){
        std::cout << "Error: run with " << argv[0] << " <driver_file> <save_image_file>" << std::endl;
        return 1;
    }
    std::string driver_file = argv[1];
    std::string save_image_file = argv[2];
    Scene sc(driver_file);
    sc.render_image(save_image_file);

    return 0;
}

