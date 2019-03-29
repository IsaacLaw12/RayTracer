#include "WaveAnimation.h"

WaveAnimation::WaveAnimation(std::string driver_line, int current_frame){
    wave_object = new WaveObject(driver_line, current_frame);
}

void WaveAnimation::advance_frame(){
    wave_object->set_time(current_frame++);
}

SceneObject* WaveAnimation::get_object(){
    std::cout << "Returning wave object\n";
    return (*SceneObject) wave_object;
}
