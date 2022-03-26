#include "Distortion.h"
#include "IPlug_include_in_plug_src.h"
#include "IControls.h"

Distortion::Distortion(const InstanceInfo& info)
: Plugin(info, MakeConfig(kNumParams, kNumPresets))
{
  GetParam(kVolume)->InitDouble("Volume", 50., 0.0, 100.0, 0.01, "%");
  GetParam(kDrive)->InitDouble("Drive", 50., 0.1, 100.0, 0.01, "%");
  GetParam(sSwitch)->InitBool("Switch", true);

#if IPLUG_EDITOR
  mMakeGraphicsFunc = [&]() {
    return MakeGraphics(*this, PLUG_WIDTH, PLUG_HEIGHT, PLUG_FPS, GetScaleForScreen(PLUG_WIDTH, PLUG_HEIGHT));
  };
  
  mLayoutFunc = [&](IGraphics* pGraphics) {
    const IBitmap switchBitmap = pGraphics->LoadBitmap((PNGSWITCH_FN), 2, true);
    const IBitmap ledBitmap = pGraphics->LoadBitmap((PNGSWITCH_LED), 2, true);
    const IBitmap knobRotateBitmap = pGraphics->LoadBitmap(PNGKNOBROTATE_FN);
    pGraphics->AttachCornerResizer(EUIResizerMode::Scale, false);
    pGraphics->AttachBackground(PNGBACKGROUND);
    pGraphics->LoadFont("Roboto-Regular", ROBOTO_FN);
    const IRECT b = pGraphics->GetBounds();
    pGraphics->AttachControl(new IBSwitchControl(b.GetCentredInside(100).GetVShifted(215), switchBitmap, sSwitch));
    pGraphics->AttachControl(new IBitmapControl(b.GetCentredInside(100).GetHShifted(-50).GetVShifted(40), ledBitmap, sSwitch));
    pGraphics->AttachControl(new ITextControl(b.GetPadded(0).GetVShifted(-270), "Distortion", IText(32)));
    pGraphics->AttachControl(new ITextControl(b.GetPadded(0).GetVShifted(-100).GetHShifted(-60), "Volume", IText(24)));
    pGraphics->AttachControl(new ITextControl(b.GetPadded(0).GetVShifted(-100).GetHShifted(60), "Drive", IText(24)));
    pGraphics->AttachControl(new IBKnobRotaterControl(b.GetCentredInside(100).GetHShifted(-60).GetVShifted(-180), knobRotateBitmap, kVolume));
    pGraphics->AttachControl(new IBKnobRotaterControl(b.GetCentredInside(100).GetHShifted(60).GetVShifted(-180), knobRotateBitmap, kDrive));
  };
#endif
}

#if IPLUG_DSP
void Distortion::ProcessBlock(sample** inputs, sample** outputs, int nFrames)
{
  const double volume = GetParam(kVolume)->Value() / 100.;
  const double drive = GetParam(kDrive)->Value();
  const bool pswitch = GetParam(sSwitch)->Value();
  const int nChans = NOutChansConnected();

  if (pswitch)
  {
    for (int s = 0; s < nFrames; s++)
    {
      for (int c = 0; c < nChans; c++)
      {
        if (inputs[c][s] >= 0)
          outputs[c][s] = 1 - exp(-fabs(inputs[c][s] * drive));
        else
          outputs[c][s] = -(1 - exp(-fabs(inputs[c][s] * drive)));
        outputs[c][s] *= volume;
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
