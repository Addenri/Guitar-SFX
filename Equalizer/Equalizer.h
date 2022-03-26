#pragma once

#include "IPlug_include_in_plug_hdr.h"

const int kNumPresets = 1;

enum EParams
{
  sSwitch,
  kLowPassShelf,
  kLow,
  kMid0,
  kMid1,
  kMid2,
  kHigh,
  kHighPassShelf,
  kNumParams
};

using namespace iplug;
using namespace igraphics;

class Equalizer final : public Plugin
{
public:
  Equalizer(const InstanceInfo& info);

#if IPLUG_DSP
  void ProcessBlock(sample** inputs, sample** outputs, int nFrames) override;
#endif
};
