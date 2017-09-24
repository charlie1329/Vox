#ifndef __VOX__
#define __VOX__

#include "IPlug_include_in_plug_hdr.h"
#include "Voice.h"
#include "onsetsds.h"
#include "onsetsdshelpers.h"
#include <vector>

class Vox : public IPlug
{
public:
  Vox(IPlugInstanceInfo instanceInfo);
  ~Vox();
  void Reset();
  void OnParamChange(int paramIdx);
  void ProcessDoubleReplacing(double** inputs, double** outputs, int nFrames);
  OnsetsDS ods;
  OnsetsDSAudioBuf audioBuf;
  float *odsData;


private:
  double mGain;
  WavVoice *wav;
  int currentSyllable;
  int currentFrame;
};

#endif
