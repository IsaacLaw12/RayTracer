#include "Image.h"

Image::Image(int width, int height){
    set_dimensions(width, height);
}

Image::Image(const Image&old_image){
    anti_alias = old_image.get_aa();
    int width = old_image.get_width();
    int height = old_image.get_height();
    set_dimensions(width, height);
}

int Image::get_width() const{
    return orig_width;
}

int Image::get_height() const{
    return orig_height;
}

int Image::get_aa() const{
    return anti_alias;
}

void Image::set_dimensions(int width, int height){
    int scale = anti_alias + 1;
    orig_width = width;
    orig_height = height;
    image_width = width * scale;
    image_height = height * scale;
    red_pixels = Eigen::MatrixXd();
    green_pixels = Eigen::MatrixXd();
    blue_pixels = Eigen::MatrixXd();
    red_pixels.conservativeResize(image_width, image_height);
    green_pixels.conservativeResize(image_width, image_height);
    blue_pixels.conservativeResize(image_width, image_height);

    t_values = Eigen::MatrixXd();
    t_values.conservativeResize(image_width, image_height);
}

void Image::write_pixel(int index_x, int index_y, Eigen::Vector3d rgb){
    red_pixels(index_x, index_y) = rgb(0);
    green_pixels(index_x, index_y) = rgb(1);
    blue_pixels(index_x, index_y) = rgb(2);
}

void Image::write_t_value(int index_x, int index_y, double t_value){
    // This keeps track of the distance of the first ray used to find This
    //     pixel. This could be used in depth of view calculations.
    t_values(index_x, index_y) = t_value;
}

void Image::save_image(std::string file_name){
    std::ofstream output(file_name);

    if (focus_blur){
        // Blur the rgb matrices based on their distance t_values
        std::cout << "Adding focus blur\n";
        apply_focus_blur();
    }

    if (anti_alias > 0){
        std::cout << "Anti-aliasing image\n";
        apply_anti_alias();
    }

    output << "P3" << "\n";
    output << orig_width << " " << orig_height << " 255" << "\n";
    int red, green, blue;
    for (int i=orig_height-1;i>=0; i--){
        for (int j=0; j<orig_width; j++){
            red = convert_255(red_pixels(j, i));
            green = convert_255(green_pixels(j, i));
            blue = convert_255(blue_pixels(j, i));
            output << red << " " << green << " " << blue << " ";
        }
        output << "\n";
    }
    output.close();
    std::cout << "Image saved at: " << file_name << "\n";
}

void Image::set_focus_blur(double distance, double strength, double length){
    focus_distance = distance;
    focus_strength = strength;
    focus_length = length;
    focus_blur = true;
}

void Image::set_anti_alias(int aa_level){
    anti_alias = aa_level;
    set_dimensions(image_width, image_height);
}

int Image::convert_255(double value){
    // Convert a 0-1 double value to 0-255 int value
    if (value > 1)
        value = 1;
    if (value < 0)
        value = 0;
    value = value * 255.0;
    return int(value);
}

void Image::apply_focus_blur(){
    post_red_pixels = Eigen::MatrixXd(red_pixels);
    post_green_pixels = Eigen::MatrixXd(green_pixels);
    post_blue_pixels = Eigen::MatrixXd(blue_pixels);

    for (int x=0; x<image_width; x++){
        for (int y=0; y<image_height; y++){
            int blur = get_pixel_blur(x, y);
            blur_pixel(x, y, blur);
        }
    }
    red_pixels = Eigen::MatrixXd(post_red_pixels);
    green_pixels = Eigen::MatrixXd(post_green_pixels);
    blue_pixels = Eigen::MatrixXd(post_blue_pixels);
}

void Image::apply_anti_alias(){
    std::vector<Eigen::MatrixXd*> image;
    image.push_back(&red_pixels);
    image.push_back(&green_pixels);
    image.push_back(&blue_pixels);

    int scale = anti_alias + 1;
    int new_width = image_width / scale;
    int new_height = image_height / scale;

    Eigen::MatrixXd scaled_down = Eigen::MatrixXd();
    scaled_down.resize(new_width, new_height);
    for (Eigen::MatrixXd* cur: image){
        for (int x=0; x<new_width; x++){
            for (int y=0; y<new_height; y++){
                scaled_down(x, y) = get_anti_alias_val(x, y, scale, cur);
            }
        }
        // Assign scaled down values to image
        cur->resize(new_width, new_height);
        *cur = Eigen::MatrixXd(scaled_down);
    }
    std::cout << "Image_width: " << image_width << "\n";
    image_width = new_width;
    std::cout << "Image_width: " << image_width << "\n";
    image_height = new_height;
}

double Image::get_anti_alias_val(int index_x, int index_y, int scale, Eigen::MatrixXd* cur_color){
    double pixel_total = 0;
    int count = 0;
    for (int x=index_x*scale; x<(index_x+1)*scale; x++){
        for (int y=index_y*scale; y<(index_y+1)*scale; y++){
           pixel_total += (*cur_color)(x, y);
           count++;
        }
    }
    return pixel_total / count;
}

int Image::get_pixel_blur(int index_x, int index_y){
    // Calculates how close to the focal center this pixel was found. Blur progressively from there
    // Returns blur_number, which represents the number of pixels out from center to average
    double blur_number = 0;
    double t_value = t_values(index_x, index_y);
    double dist_from_center = abs(focus_distance - t_value);
    if (dist_from_center > focus_length){
        double dist_into_blur = dist_from_center - focus_length;
        blur_number = dist_into_blur * focus_strength / 100;
        int max_blur = image_width / 50;
        if (blur_number > max_blur)
            blur_number = max_blur;
    }
    return (int) blur_number;
}

void Image::blur_pixel(int index_x, int index_y, int blur_size){
    // Original color matrices are not affected. Their blurred averages are
    //     written to post_* matrices
    double red_avg = 0;
    double green_avg = 0;
    double blue_avg = 0;
    int avg_num = 0;
    if (blur_size <= 0) return;

    for (int x=-blur_size; x<=blur_size; x++){
        for (int y=-blur_size; y<=blur_size; y++){
            int x_index = index_x + x;
            if (x_index < 0 || x_index >= image_width){
               continue;
            }
            int y_index = index_y + y;
            if (y_index < 0 || y_index >= image_height){
               continue;
            }
            red_avg += red_pixels(x_index, y_index);
            green_avg += green_pixels(x_index, y_index);
            blue_avg += blue_pixels(x_index, y_index);
            avg_num++;
        }
    }
    if (avg_num > 0){
        post_red_pixels(index_x, index_y) = red_avg / avg_num;
        post_green_pixels(index_x, index_y) = green_avg / avg_num;
        post_blue_pixels(index_x, index_y) = blue_avg / avg_num;
    }
}
