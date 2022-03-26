#include "Delay.h"
#include "IPlug_include_in_plug_src.h"
#include "IControls.h"

Delay::Delay(const InstanceInfo& info)
: Plugin(info, MakeConfig(kNumParams, kNumPresets))
{
  GetParam(kDelay)->InitDouble("Delay", 250., 0., 500., 0.01, "mS");
  GetParam(kFeedback)->InitDouble("Feedback", 50., 0., 100., 0.01, "%");
  GetParam(kLevel)->InitDouble("Level", 50, 0., 100., .1, "%");
  GetParam(kSwitch)->InitBool("Switch", true);

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
    pGraphics->AttachControl(new IBSwitchControl(b.GetCentredInside(100).GetVShifted(215), switchBitmap, kSwitch));
    pGraphics->AttachControl(new IBitmapControl(b.GetCentredInside(100).GetHShifted(-50).GetVShifted(40), ledBitmap, kSwitch));
    pGraphics->AttachControl(new ITextControl(b.GetPadded(0).GetVShifted(-270), "Delay", IText(32)));
    pGraphics->AttachControl(new ITextControl(b.GetPadded(0).GetVShifted(-120).GetHShifted(-60), "Delay", IText(24)));
    pGraphics->AttachControl(new ITextControl(b.GetPadded(0).GetVShifted(-120).GetHShifted(60), "Feedback", IText(24)));
    pGraphics->AttachControl(new ITextControl(b.GetPadded(0).GetVShifted(-10), "Level", IText(24)));
    pGraphics->AttachControl(new IBKnobRotaterControl(b.GetCentredInside(100).GetHShifted(-60).GetVShifted(-180), knobRotateBitmap, kDelay));
    pGraphics->AttachControl(new IBKnobRotaterControl(b.GetCentredInside(100).GetHShifted(60).GetVShifted(-180), knobRotateBitmap, kFeedback));
    pGraphics->AttachControl(new IBKnobRotaterControl(b.GetCentredInside(100).GetVShifted(-70), knobRotateBitmap, kLevel));
  };
#endif
}

Delay::~Delay()
{
  if (pBuffer)
  {
    delete[] pBuffer;
  }
}

#if IPLUG_DSP
void Delay::OnReset()
{
  TRACE;
  BufferSize = 2 * GetSampleRate();
  if (pBuffer)
  {
    delete[] pBuffer;
  }

  pBuffer = new double[BufferSize];

  if (pBuffer)
  {
    memset(pBuffer, 0, BufferSize * sizeof(double));
  }

  WriteIndex = 0;
  ReadIndex = 0;
}

void Delay::ProcessBlock(sample** inputs, sample** outputs, int nFrames)
{
  const double delay = GetParam(kDelay)->Value() * GetSampleRate() / 1000.;
  const double feedback = GetParam(kFeedback)->Value() / 100.;
  const double level = GetParam(kLevel)->Value() / 100.;
  const bool pswitch = GetParam(kSwitch)->Value();
  const int nChans = NOutChansConnected();

  ReadIndex = WriteIndex - (int)delay;
  if (ReadIndex < 0)
  {
    ReadIndex += BufferSize;
  }

  if (pswitch)
  {
    for (int s = 0; s < nFrames; s++)
    {
      for (int c = 0; c < nChans; c++)
      {
        double tmp = pBuffer[ReadIndex];
        if (delay == 0)
        {
          tmp = inputs[c][s];
        }
        pBuffer[WriteIndex] = inputs[c][s] + feedback * tmp;
        outputs[c][s] = (level * tmp + (1 - level) * inputs[c][s]);
        
        ++WriteIndex;
        if (WriteIndex >= BufferSize)
        {
          WriteIndex = 0;
        }

        ++ReadIndex;
        if (ReadIndex >= BufferSize)
        {
          ReadIndex = 0;
        }
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
