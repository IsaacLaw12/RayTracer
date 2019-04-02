#ifndef WAVEANI_INCLUDED
#define WAVEANI_INCLUDED

#include "SceneObject.h"
#include "AnimatedObject.h"
#include "WaveObject.h"


class WaveAnimation: public AnimatedObject {
  public:
      WaveAnimation(std::string driver_line, int current_frame);
      void advance_frame();
      SceneObject* get_object();

  private:
      WaveObject* wave_object;

};
#endif
