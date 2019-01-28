#include <iostream>
#include "Scene.h"
#include "RenderImage.h"

std::string extract_base_name(std::string original){
    return original;
}

void prepare_directory(){
    system("rmdir -r /tmp/");
    system("mkdir /tmp/");
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
    save_image_file = extract_base_name(save_image_file);

    int frame_num = 0;
    while (sc.advance_frame()){
        std::string image_name = "/tmp" + save_image_file + std::to_string(frame_num) + ".ppm";
        RenderImage ri = RenderImage(&sc);
        ri.render_image(image_name);
    }
    return 0;
}
