#pragma once

#include "IPlug_include_in_plug_hdr.h"

const int kNumPresets = 1;

enum EParams
{
  kSwitch = 0,
  kDelay,
  kFeedback,
  kLevel,
  kNumParams
};

using namespace iplug;
using namespace igraphics;

class Delay final : public Plugin
{
public:
  Delay(const InstanceInfo& info);
  ~Delay();
  
private:
  double* pBuffer = nullptr;
  int ReadIndex = 0;
  int WriteIndex = 0;
  int BufferSize = 0;

#if IPLUG_DSP
  void OnReset() override;
  void ProcessBlock(sample** inputs, sample** outputs, int nFrames) override;
#endif
};
