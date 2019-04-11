#include "WaveAnimation.h"

WaveAnimation::WaveAnimation(std::string driver_line, int current_frame){
    wave_object = new WaveObject(driver_line, current_frame);
}

void WaveAnimation::advance_frame(){
    wave_object->set_time(wave_object->get_time() + 1);
}

SceneObject* WaveAnimation::get_object(){
    return wave_object;
}
