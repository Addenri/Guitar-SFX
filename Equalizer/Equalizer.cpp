#include "Equalizer.h"
#include "IPlug_include_in_plug_src.h"
#include "IControls.h"
#include "SVF.h"

Equalizer::Equalizer(const InstanceInfo& info)
: Plugin(info, MakeConfig(kNumParams, kNumPresets))
{
  GetParam(sSwitch)->InitBool("Switch", true);
  GetParam(kLowPassShelf)->InitDouble("Low Shelf", .0, -36.0, 36.0, .1, "dB");
  GetParam(kLow)->InitDouble("Low", .0, -36.0, 36.0, .1, "dB");
  GetParam(kMid0)->InitDouble("Mid", .0, -36.0, 36.0, .1, "dB");
  GetParam(kMid1)->InitDouble("Mid", .0, -36.0, 36.0, .1, "dB");
  GetParam(kMid2)->InitDouble("Mid", .0, -36.0, 36.0, .1, "dB");
  GetParam(kHigh)->InitDouble("High", .0, -36.0, 36.0, .1, "dB");
  GetParam(kHighPassShelf)->InitDouble("High Shelf", .0, -36.0, 36.0, .1, "dB");

#if IPLUG_EDITOR
  mMakeGraphicsFunc = [&]() {
    return MakeGraphics(*this, PLUG_WIDTH, PLUG_HEIGHT, PLUG_FPS, GetScaleForScreen(PLUG_WIDTH, PLUG_HEIGHT));
  };
  
  mLayoutFunc = [&](IGraphics* pGraphics) {
    const IBitmap switchBitmap = pGraphics->LoadBitmap((PNGSWITCH_FN), 2, true);
    const IBitmap ledBitmap = pGraphics->LoadBitmap((PNGSWITCH_LED), 2, true);
    const IBitmap sliderHandleBitmap = pGraphics->LoadBitmap(PNGSLIDERHANDLE_FN);
    const IBitmap sliderTrackBitmap = pGraphics->LoadBitmap(PNGSLIDERTRACK_FN);
    pGraphics->AttachCornerResizer(EUIResizerMode::Scale, false);
    pGraphics->AttachBackground(PNGBACKGROUND);
    pGraphics->LoadFont("Roboto-Regular", ROBOTO_FN);
    const IRECT b = pGraphics->GetBounds();
    pGraphics->AttachControl(new ITextControl(b.GetPadded(0).GetVShifted(-190).GetHShifted(580), "7-Band EQ", IText(32, COLOR_WHITE)));
    pGraphics->AttachControl(new ITextControl(b.GetPadded(0).GetVShifted(264).GetHShifted(-580), "Gain, dB", IText(24, COLOR_WHITE)));
    pGraphics->AttachControl(new ITextControl(b.GetPadded(0).GetVShifted(264).GetHShifted(-412), "Gain, dB", IText(24, COLOR_WHITE)));
    pGraphics->AttachControl(new ITextControl(b.GetPadded(0).GetVShifted(264).GetHShifted(-244), "Gain, dB", IText(24, COLOR_WHITE)));
    pGraphics->AttachControl(new ITextControl(b.GetPadded(0).GetVShifted(264).GetHShifted(-76), "Gain, dB", IText(24, COLOR_WHITE)));
    pGraphics->AttachControl(new ITextControl(b.GetPadded(0).GetVShifted(264).GetHShifted(92), "Gain, dB", IText(24, COLOR_WHITE)));
    pGraphics->AttachControl(new ITextControl(b.GetPadded(0).GetVShifted(264).GetHShifted(260), "Gain, dB", IText(24, COLOR_WHITE)));
    pGraphics->AttachControl(new ITextControl(b.GetPadded(0).GetVShifted(264).GetHShifted(428), "Gain, dB", IText(24, COLOR_WHITE)));

    pGraphics->AttachControl(new ITextControl(b.GetPadded(0).GetVShifted(-264).GetHShifted(-580), "120 Hz", IText(24, COLOR_WHITE)));
    pGraphics->AttachControl(new ITextControl(b.GetPadded(0).GetVShifted(-264).GetHShifted(-412), "240 Hz", IText(24, COLOR_WHITE)));
    pGraphics->AttachControl(new ITextControl(b.GetPadded(0).GetVShifted(-264).GetHShifted(-244), "600 Hz", IText(24, COLOR_WHITE)));
    pGraphics->AttachControl(new ITextControl(b.GetPadded(0).GetVShifted(-264).GetHShifted(-76), "2 kHz", IText(24, COLOR_WHITE)));
    pGraphics->AttachControl(new ITextControl(b.GetPadded(0).GetVShifted(-264).GetHShifted(92), "5 kHz", IText(24, COLOR_WHITE)));
    pGraphics->AttachControl(new ITextControl(b.GetPadded(0).GetVShifted(-264).GetHShifted(260), "8 kHz", IText(24, COLOR_WHITE)));
    pGraphics->AttachControl(new ITextControl(b.GetPadded(0).GetVShifted(-264).GetHShifted(428), "12 kHz", IText(24, COLOR_WHITE)));

    pGraphics->AttachControl(new IBSwitchControl(b.GetCentredInside(100).GetVShifted(40).GetHShifted(580), switchBitmap, sSwitch));
    pGraphics->AttachControl(new IBitmapControl(b.GetCentredInside(100).GetVShifted(-100).GetHShifted(580), ledBitmap, sSwitch));
    pGraphics->AttachControl(new IBSliderControl(b.GetCentredInside(60.f, 500.f).GetHShifted(-580), sliderHandleBitmap, sliderTrackBitmap, kLowPassShelf));
    pGraphics->AttachControl(new IBSliderControl(b.GetCentredInside(60.f, 500.f).GetHShifted(-412), sliderHandleBitmap, sliderTrackBitmap, kLow));
    pGraphics->AttachControl(new IBSliderControl(b.GetCentredInside(60.f, 500.f).GetHShifted(-244), sliderHandleBitmap, sliderTrackBitmap, kMid0));
    pGraphics->AttachControl(new IBSliderControl(b.GetCentredInside(60.f, 500.f).GetHShifted(-76), sliderHandleBitmap, sliderTrackBitmap, kMid1));
    pGraphics->AttachControl(new IBSliderControl(b.GetCentredInside(60.f, 500.f).GetHShifted(92), sliderHandleBitmap, sliderTrackBitmap, kMid2));
    pGraphics->AttachControl(new IBSliderControl(b.GetCentredInside(60.f, 500.f).GetHShifted(260), sliderHandleBitmap, sliderTrackBitmap, kHigh));
    pGraphics->AttachControl(new IBSliderControl(b.GetCentredInside(60.f, 500.f).GetHShifted(428), sliderHandleBitmap, sliderTrackBitmap, kHighPassShelf));
  };
#endif
}

#if IPLUG_DSP
void Equalizer::ProcessBlock(sample** inputs, sample** outputs, int nFrames)
{
  const bool pswitch = GetParam(sSwitch)->Value();
  const double LowShelfGain = GetParam(kLowPassShelf)->Value();
  const double LowGain = GetParam(kLow)->Value();
  const double MidGain0 = GetParam(kMid0)->Value();
  const double MidGain1 = GetParam(kMid1)->Value();
  const double MidGain2 = GetParam(kMid2)->Value();
  const double HighGain = GetParam(kHigh)->Value();
  const double HighShelfGain = GetParam(kHighPassShelf)->Value();
  const int nChans = NOutChansConnected();
  sample** lshelfpassed = inputs;
  sample** lpassed = inputs;
  sample** midpassed0 = inputs;
  sample** midpassed1 = inputs;
  sample** midpassed2 = inputs;
  sample** hpassed = inputs;

  //Filter setup
  //Налаштування фільтрів
  SVF<sample, 2> eqLPShelf = SVF<sample, 2>::EMode::kLowPassShelf;
  eqLPShelf.SetFreqCPS(120.);
  eqLPShelf.SetGain(LowShelfGain);

  SVF<sample, 2> eqLow = SVF<sample, 2>::EMode::kBell;
  eqLow.SetFreqCPS(240.);
  eqLow.SetGain(LowGain);

  SVF<sample, 2> eqMid0 = SVF<sample, 2>::EMode::kBell;
  eqMid0.SetFreqCPS(600.);
  eqMid0.SetGain(MidGain0);

  SVF<sample, 2> eqMid1 = SVF<sample, 2>::EMode::kBell;
  eqMid1.SetFreqCPS(2000.);
  eqMid1.SetGain(MidGain1);

  SVF<sample, 2> eqMid2 = SVF<sample, 2>::EMode::kBell;
  eqMid2.SetFreqCPS(5000.);
  eqMid2.SetGain(MidGain2);

  SVF<sample, 2> eqHigh = SVF<sample, 2>::EMode::kBell;
  eqHigh.SetFreqCPS(8000.);
  eqHigh.SetGain(HighGain);

  SVF<sample, 2> eqHPShelf = SVF<sample, 2>::EMode::kHighPassShelf;
  eqHPShelf.SetFreqCPS(12000.);
  eqHPShelf.SetGain(HighShelfGain);

  if (pswitch)
  {
    eqLPShelf.ProcessBlock(inputs, lshelfpassed, 2, nFrames);
    eqLow.ProcessBlock(lshelfpassed, lpassed, 2, nFrames);
    eqMid0.ProcessBlock(lpassed, midpassed0, 2, nFrames);
    eqMid1.ProcessBlock(midpassed0, midpassed1, 2, nFrames);
    eqMid2.ProcessBlock(midpassed1, midpassed2, 2, nFrames);
    eqHigh.ProcessBlock(midpassed2, hpassed, 2, nFrames);
    eqHPShelf.ProcessBlock(hpassed, outputs, 2, nFrames);
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
