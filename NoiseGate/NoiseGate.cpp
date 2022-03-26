#include "NoiseGate.h"
#include "IPlug_include_in_plug_src.h"
#include "IControls.h"
#include "math.h"

NoiseGate::NoiseGate(const InstanceInfo& info)
: Plugin(info, MakeConfig(kNumParams, kNumPresets))
{
  GetParam(kHold)->InitDouble("Hold", .5, .001, 1.0, .001, "Sec");
  GetParam(kThreshold)->InitDouble("Threshold", 50., .0, 100.0, .01, "%");
  GetParam(sSwitch)->InitBool("Switch", true);

#if IPLUG_EDITOR
  mMakeGraphicsFunc = [&]() {
    return MakeGraphics(*this, PLUG_WIDTH, PLUG_HEIGHT, PLUG_FPS, GetScaleForScreen(PLUG_WIDTH, PLUG_HEIGHT));
  };
  
  mLayoutFunc = [&](IGraphics* pGraphics) {
    const IBitmap knobRotateBitmap = pGraphics->LoadBitmap(PNGKNOBROTATE_FN);
    const IBitmap switchBitmap = pGraphics->LoadBitmap((PNGSWITCH_FN), 2, true);
    const IBitmap ledBitmap = pGraphics->LoadBitmap((PNGSWITCH_LED), 2, true);
    pGraphics->AttachCornerResizer(EUIResizerMode::Scale, false);
    pGraphics->AttachBackground(PNGBACKGROUND);
    pGraphics->LoadFont("Roboto-Regular", ROBOTO_FN);
    const IRECT b = pGraphics->GetBounds();
    pGraphics->AttachControl(new IBSwitchControl(b.GetCentredInside(100).GetVShifted(215), switchBitmap, sSwitch));
    pGraphics->AttachControl(new IBitmapControl(b.GetCentredInside(100).GetHShifted(-50).GetVShifted(40), ledBitmap, sSwitch));
    pGraphics->AttachControl(new ITextControl(b.GetPadded(0).GetVShifted(-270), "Noise Gate", IText(32)));
    pGraphics->AttachControl(new ITextControl(b.GetPadded(0).GetVShifted(-100).GetHShifted(-60), "Hold", IText(24)));
    pGraphics->AttachControl(new ITextControl(b.GetPadded(0).GetVShifted(-100).GetHShifted(60), "Threshold", IText(24)));
    pGraphics->AttachControl(new IBKnobRotaterControl(b.GetCentredInside(100).GetHShifted(-60).GetVShifted(-180), knobRotateBitmap, kHold));
    pGraphics->AttachControl(new IBKnobRotaterControl(b.GetCentredInside(100).GetHShifted(60).GetVShifted(-180), knobRotateBitmap, kThreshold));


  };
#endif
}
    
#if IPLUG_DSP
void NoiseGate::ProcessBlock(sample** inputs, sample** outputs, int nFrames)
{
  const double hold = GetParam(kHold)->Value();
  const double threshold = GetParam(kThreshold)->Value() / 100000.;
  const bool pswitch = GetParam(sSwitch)->Value();
  const int nChans = NOutChansConnected();
  const int freq = GetSampleRate();
  static unsigned int skip = 0;
  static bool gate = 1;

  if (pswitch)
  {
    for (int s = 0; s < nFrames; s++)
    {
      if (skip > 0)
        skip--;
      for (int c = 0; c < nChans; c++)
      {
        if (fabs(inputs[c][s]) <= 1 * threshold)
        {
          gate = 0;
        }

        else
        {
          gate = 1;
          skip = freq * hold;
        }

        if(!gate && skip == 0)
          outputs[c][s] = inputs[c][s] * 0.;
        else
          outputs[c][s] = inputs[c][s];
      }
    }
  }
  
  //Bypass mode
  //Режим обходу
  else
  {
    for (int s = 0; s < nFrames; s++) {
      for (int c = 0; c < nChans; c++) {
        outputs[c][s] = inputs[c][s];
      }
    }
  }
}
#endif
