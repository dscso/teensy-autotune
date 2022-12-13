#ifndef faust_teensy_h_
#define faust_teensy_h_

#include "Arduino.h"
#include "AudioStream.h"
#include "Audio.h"

#include "Echo.h"

class MyDsp : public AudioStream
{
public:
  MyDsp();
  ~MyDsp();

  virtual void update(void);

private:
  q31_t buffer[AUDIO_BLOCK_SAMPLES * 2] __attribute__((aligned(4)));
  // float32_t float_buffer[AUDIO_BLOCK_SAMPLES * 2] __attribute__((aligned(4)));
};

#endif
