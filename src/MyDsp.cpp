#include "MyDsp.h"
#include <Arduino.h>
#include <arm_math.h>
#include <arm_const_structs.h>
#include <math.h>

#define AUDIO_OUTPUTS 2
#define FFTSIZE AUDI

#define MULT_16 32767
#define DIV_16 0.0000305185
#define MORE_EFFICANT_MEMCPY(dest, source, lenght) \
  for (int i = 0; i < lenght; i += 8)              \
  {                                                \
    dest[i] = source[i];                           \
    dest[i + 1] = source[i + 1];                   \
    dest[i + 2] = source[i + 2];                   \
    dest[i + 3] = source[i + 3];                   \
    dest[i + 4] = source[i + 4];                   \
    dest[i + 5] = source[i + 5];                   \
    dest[i + 6] = source[i + 6];                   \
    dest[i + 7] = source[i + 7];                   \
  }

MyDsp::MyDsp() : AudioStream(AUDIO_OUTPUTS, new audio_block_t *[AUDIO_OUTPUTS])
{

  // setting up DSP objects
  /*echoL.setDel(10000);
  echoL.setFeedback(0.5);
  echoR.setDel(10000);
  echoR.setFeedback(0.5);*/
}

MyDsp::~MyDsp() {}

void MyDsp::update(void)
{
  audio_block_t *inBlock[AUDIO_OUTPUTS];
  audio_block_t *outBlock[AUDIO_OUTPUTS];
  for (int channel = 0; channel < AUDIO_OUTPUTS; channel++)
  {
    inBlock[channel] = receiveReadOnly(channel);
    outBlock[channel] = allocate();
    if (outBlock[channel])
    {
      if (inBlock[channel])
      {
        // convert to buffer readable by ARM native instructions
        memset(buffer, 0, sizeof(buffer));
        for (int i = 0; i < AUDIO_BLOCK_SAMPLES - 1; i++)
        {
          // real part
          buffer[i * 2] = inBlock[channel]->data[i] << 16; // i dont know what i am doing
          buffer[i * 2 + 1] = 0;
        }
        // arm_q15_to_float(inBlock[channel]->data, float_buffer, AUDIO_BLOCK_SAMPLES * 2);
        const arm_cfft_instance_q31 *fft_size = &arm_cfft_sR_q31_len1024;

        arm_cfft_q31(fft_size, buffer, 0, 1);
        // zeroing out negative frequencies
        for (int i = 0; i < AUDIO_BLOCK_SAMPLES; i++)
        {
          buffer[i] = 0;
        }
        int offset = 10;

        if (offset >= 0)
          for (int i = (AUDIO_BLOCK_SAMPLES - 1) * 2; i > AUDIO_BLOCK_SAMPLES; i--)
          {
            buffer[i] = buffer[i - offset];
          }
        arm_cfft_q31(fft_size, buffer, 1, 1);

        for (int i = 0; i < AUDIO_BLOCK_SAMPLES - 1; i++)
        {
          outBlock[channel]->data[i] = buffer[i * 2] >> 4;
        }
      }

      transmit(outBlock[channel], channel);
      if (inBlock[channel])
        release(inBlock[channel]);
      release(outBlock[channel]);
    }
  }
}
