#include <string>
#include <sstream>
#include "Light.h"

Light::Light(std::string driver_line){
    // Light string: "light x_loc y_loc z_loc w r g b"
    std::stringstream driver(driver_line);
    double light_obj_doubles[7] = {};
    std::string driver_type = "";
    driver >> driver_type;

    double temp = 0;
    int counter = 0;
    while (driver >> temp){
        light_obj_doubles[counter++] = temp;
    }
    if (counter != 7){
        // The correct number of numbers were not found
        load_successful = false;
        return;
    }
    assign_values(light_obj_doubles);
}

void Light::assign_values(double spec_nums[]){
    x_cord = spec_nums[0];
    y_cord = spec_nums[1];
    z_cord = spec_nums[2];
    w_value = spec_nums[3];
    red_color = spec_nums[4];
    green_color = spec_nums[5];
    blue_color = spec_nums[6];
}

bool Light::light_valid(){
    return load_successful;
}
