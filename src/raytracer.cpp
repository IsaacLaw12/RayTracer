#include <iostream>
#include "Scene.h"
#include "RenderImage.h"

void create_directory(std::string save_folder){
    //   ***** THIS USES LINUX SYSTEM CALLS, TO RUN ON WINDOWS REWRITE ******
    // Clear the saved render folder of previous contents
    char remove_command[80] = "rm -r ";
    strcat(remove_command, save_folder.c_str());
    system(remove_command);

    char create_command[80] = "mkdir ";
    strcat(create_command, save_folder.c_str());
    if (0 == system(create_command)){
        std::cout << save_folder << " directory created successfully\n";
    } else{
        std::cout << save_folder << " directory creation failed\n";
    }
}

int parse_int(std::string str, int &status){
    int result = 0;
    try {
        result = stoi(str);
    }
    catch(std::invalid_argument& e){
        status = 1;
    }
    return result;
}

std::string parse_folder_name(std::string name){
    std::string result = "";
    for (auto c: name){
        if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')){
            result += c;
        }
    }
    return result;
}

int parse_args(int argc, char*argv[], int &num_threads, std::string &driver_file, std::string &save_folder){
    int status = 0;

    // Set Defaults
    num_threads = 1;

    // Parse flags
    for (int i=1; i<argc-2; i++){
        std::string arg = argv[i];
        if (arg == "-t" || arg == "--threads"){
            int valid_int = 0;
            num_threads = parse_int(std::string(argv[++i]), valid_int);
            if (valid_int != 0 || (!(i < argc-2))){
                std::cout << "Number of threads not specified\n";
                status = 1;
                num_threads = 1;
            } else{
                num_threads = std::max(std::min(num_threads, 100), 1);
            }
        }
    }
    if (!(argc >= 3)){
        status = 1;
    }
    // Extract two mandatory arguments
    if (status == 0){
        driver_file = argv[argc-2];
        save_folder = parse_folder_name(argv[argc-1]);
    } else{
        std::cout << "Error: run with " << argv[0] << " <driver_file> <save_folder>\n\n"
                  << "    driver_file: A scene specification file, more detail in scene_driver.md\n"
                  << "    save_folder: The folder to save output images to. If it already exists current\n"
                  << "        contents will be replaced\n\n"
                  << "    Optional:\n"
                  << "         --threads, -t  specify number of threads\n\n";
    }
    return status;
}

std::string image_name(int frame_number, std::string save_folder){
    std::string padded_frame_num = std::to_string(frame_number);
    while (padded_frame_num.size() < 4){
        padded_frame_num = "0" + padded_frame_num;
    }
    std::string result = save_folder + "/output" + padded_frame_num + ".ppm";
    return result;
}

void multi_thread_render(RenderImage &ri, int number_threads){
    std::vector<std::thread> render_threads;
    for (int i=0; i<number_threads; i++)
        render_threads.push_back(std::thread(&RenderImage::render_tiles, &ri));

    for (std::thread & t : render_threads)
        t.join();
}

void render_scene(Scene &sc, int number_threads, std::string save_folder){
    RenderImage ri = RenderImage(&sc);
    while (sc.advance_frame()){
        ri.reset();
        if (number_threads > 1)
            multi_thread_render(ri, number_threads);
        else
            ri.render_tiles();
        ri.save_image(image_name(sc.get_current_frame(), save_folder));
    }
}

int main(int argc, char*argv[]){
    int number_threads = 1;
    std::string driver_file = "";
    std::string save_folder = "";
    int valid_args = parse_args(argc, argv, number_threads, driver_file, save_folder);
    if (valid_args != 0){
        return valid_args;
    }

    create_directory(save_folder);
    Scene sc(driver_file);
    render_scene(sc, number_threads, save_folder);
    return 0;
}
