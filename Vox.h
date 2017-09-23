#ifndef __VOX__
#define __VOX__

#include "IPlug_include_in_plug_hdr.h"

class Vox : public IPlug
{
public:
  Vox(IPlugInstanceInfo instanceInfo);
  ~Vox();

  void Reset();
  void OnParamChange(int paramIdx);
  void ProcessDoubleReplacing(double** inputs, double** outputs, int nFrames);

private:
  double mGain;
};

#endif
