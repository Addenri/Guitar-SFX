#pragma once

#include "IPlug_include_in_plug_hdr.h"

const int kNumPresets = 1;

enum EParams
{
  sSwitch = 0,
  kHold,
  kThreshold,
  kNumParams
};

using namespace iplug;
using namespace igraphics;

class NoiseGate final : public Plugin
{
public:
  NoiseGate(const InstanceInfo& info);

#if IPLUG_DSP
  void ProcessBlock(sample** inputs, sample** outputs, int nFrames) override;
#endif
};
