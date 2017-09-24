#include "Vox.h"
#include "IPlug_include_in_plug_src.h"
#include "IControl.h"
#include "resource.h"
#include <mutex>
#include <pitch_detection.hpp>
#include "smbPitchShift.h"
#include <thread>

const int kNumPrograms = 1;
const double noteThreshold = 0.3;
bool onsetBool = false;
double frequency = 0.0;
std::vector<double> pitchVec;
std::mutex mtx;



void onsetCallback(OnsetsDSAudioBuf *audio, size_t offset) {
	onsetBool = true;
}

enum EParams
{
  kGain = 0,
  kNumParams
};

enum ELayout
{
  kWidth = GUI_WIDTH,
  kHeight = GUI_HEIGHT,

  kGainX = 110,
  kGainY = 250,
  kKnobFrames = 60
};

Vox::Vox(IPlugInstanceInfo instanceInfo)
  :	IPLUG_CTOR(kNumParams, kNumPrograms, instanceInfo), mGain(1.)
{
	TRACE;

  //arguments are: name, defaultVal, minVal, maxVal, step, label
  GetParam(kGain)->InitDouble("Gain", 50., 0., 100.0, 0.01, "%");
  GetParam(kGain)->SetShape(2.);

  IGraphics* pGraphics = MakeGraphics(this, kWidth, kHeight);
  pGraphics->AttachBackground(BACKGROUND_ID,BACKGROUND_FN);

  IBitmap knob = pGraphics->LoadIBitmap(KNOB_ID, KNOB_FN, kKnobFrames);

  pGraphics->AttachControl(new IKnobMultiControl(this, kGainX, kGainY, kGain, &knob));

  AttachGraphics(pGraphics);

  //MakePreset("preset 1", ... );
  MakeDefaultPreset((char *) "-", kNumPrograms);

  wav = new WavVoice();
  wav->addFile("C:/Users/charl/Documents/vox/all_star.txt");

  currentSyllable = 0;
  currentFrame = 0;

  //setting up stuff for onset detection
  odsData = new float[onsetsds_memneeded(ODS_ODF_RCOMPLEX, 512, 11)];
  onsetsds_init(&ods, odsData, ODS_FFT_FFTW3_HC, ODS_ODF_RCOMPLEX, 512, 11, 44100.f );
  onsetBool = false;
  onsetsds_init_audiodata(&audioBuf, &ods, 256);
}

Vox::~Vox() { delete wav; delete odsData; onsetsds_destroy_audiodata(&audioBuf); }


void Vox::ProcessDoubleReplacing(double** inputs, double** outputs, int nFrames)
{
  // Mutex is already locked for us.
  // need at 16000 sample rate
  double* in1 = inputs[0];
  double* in2 = inputs[1];
  double* out1 = outputs[0];
  double* out2 = outputs[1];


  /*std::vector<double> vec(in1, in1 + nFrames);
  if (pitchVec.size() < 4096) { //just append to end
	  pitchVec.insert(pitchVec.end(), vec.begin(), vec.end());
  }
  else if (pitchVec.size() == 4096) { //sliding window approach
	  std::vector<double>::const_iterator first = pitchVec.begin() + nFrames;
	  std::vector<double>::const_iterator last = pitchVec.end();
	  std::vector<double> newVec(first, last);
	  newVec.insert(newVec.end(), vec.begin(), vec.end());
	  pitchVec = newVec;
  }*/
 

	
  //do onset processing stuff
  float *in1F = new float[nFrames];
  for (int i = 0; i < nFrames; i++) {
	  in1F[i] = (float)in1[i];
  }

  onsetsds_process_audiodata(&audioBuf, in1F, nFrames, onsetCallback);
  delete in1F;


  mtx.lock();
  if (onsetBool) {
	  onsetBool = false;
	  mtx.unlock();
	  currentFrame = 0;
	  currentSyllable++;
	  if (currentSyllable >= wav->nSyllables) {
		  currentSyllable = 0;
	  }
  }
  else {
	  mtx.unlock();
  }

  int i = 0;
  while (currentFrame < wav->framesPerSyllable[currentSyllable] && i < nFrames) {
	  out1[i] = wav->getFrames(currentSyllable, currentFrame);
	  out2[i] = out1[i];
	  currentFrame++;
	  i++;
  }

  

  if (1 >= 0) {
	  //pitch shift the fuck out of it
	  //find out how many semitones
	  //call pitch shift function
	  
	  /*float *out1F = new float[nFrames];
	  float *out2F = new float[nFrames];
	  for (int i = 0; i < nFrames; i++) {
		  out1F[i] = (float)out1[i];
	  }
	  smbPitchShift(pow(2., 3 / 12.), nFrames, 2048, 4, 44100, out1F, out2F);
	  for (int i = 0; i < nFrames; i++) {
		  out1[i] = out2F[i];
		  out2[i] = out1[i];
	  }
	  delete out1F;
	  delete out2F;*/

  }



  /*if (onsetBool) {
	  for (int i = 0; i < nFrames; i += 1) {
		  if (currentFrame >= wav->framesPerSyllable[currentSyllable]) {
			  currentFrame = 0;
			  currentSyllable++;
			  if (currentSyllable >= wav->nSyllables) {
				  currentSyllable = 0;
			  }
		  }
		  out1[i] = wav->getFrames(currentSyllable, currentFrame);
		  out2[i] = out1[i];
		  currentFrame++;
	}

  }*/
  /*
  for (int s = 0; s < nFrames; ++s, ++in1, ++in2, ++out1, ++out2)
  {
    *out1 = *in1 * mGain;
    *out2 = *in2 * mGain;
  }*/
}

void Vox::Reset()
{
  TRACE;
  IMutexLock lock(this);
}

void Vox::OnParamChange(int paramIdx)
{
  IMutexLock lock(this);

  switch (paramIdx)
  {
    case kGain:
      mGain = GetParam(kGain)->Value() / 100.;
      break;

    default:
      break;
  }
}
