#include "Speed/Indep/Src/Input/InputDevicePs3.h"
#include "Speed/Indep/Src/Input/IFeedBack.h"
#include "Speed/Indep/Src/Input/InputDevice.h"
#include "Speed/Indep/Src/Input/Common/FFBTypes.h"
#include "Speed/Indep/Src/Input/SteeringWheelDevice.h"
#include "dolphin/pad.h"

// Decl: 23
struct DeviceScalarInfo {
    DeviceScalarType type; // offset 0x0, size 0x4, Decl: 24
    const char * name; // offset 0x4, size 0x4, Decl: 25
    int system_index; // offset 0x8, size 0x4, Decl: 26
    float ramp_min; // offset 0xC, size 0x4
    float ramp_max; // offset 0x10, size 0x4
};

static struct InputEffectState effect_states[4]; // size: 0x50, address: 0x8047E874, Decl: 81

static const struct DeviceScalarInfo device_infos[37] = {

}; // size: 0x2E4, address: 0x803F424C, Decl: 89


struct InputDevice* GameDevice::Construct(int port) { // Decl: 197

}

inline bool GameDevice::IsWheel() { // Decl: 211
    if (mWheelDevice != nullptr) {
        return mWheelDevice->IsConnected();
    }
    return false;
} 

inline struct UTL::COM::IUnknown* GameDevice::GetInterfaces() { return this; }; // Decl: 213

inline struct UTL::COM::IUnknown* GameDevice::GetSecondaryDevice() { return mWheelDevice; } // Decl: 214 

int GameDevice::mCount; // size: 0x4, address: 0x8041E4B0, Decl: 734

// DWARF Unmatched
// UNSOLVED
void GameDevice::Initialize() { // Decl: 735
  const DeviceScalarInfo *info;
  const char *name;
  int i;

  i = 0;
  this->mNumScalars = i;
  info = device_infos;
  name = info->name;

  if (name == nullptr) {
    return;
  }

  do {
    this->fDeviceScalar[i].InitializeDeviceScalar(
        info->type, name, &this->fPrevValues[i],
        &this->fCurrentValues[i]);
    info = info + 1;
    i = i + 1;
    this->mNumScalars = this->mNumScalars + 1;
    name = info->name;
    if (name == nullptr) {
      return;
    }
  } while (i <= 0x24);
}

// I am unsure where this is defined?
// UNSOLVED
bool input_connected[4];
bool gShowPortInfo;

bool GameDevice::IsConnected() { // Decl: 755
  bool uVar2;
  SteeringWheelDevice *pSVar3;
  PADStatus HardwarePadStatus [4];
  PADStatus auStack_38 [52];
  
  pSVar3 = this->mWheelDevice;
  if ((pSVar3 == nullptr) || pSVar3->IsConnected()) {
    if (gShowPortInfo) {
      PADRead(auStack_38);
    }
    return input_connected[this->GetDeviceIndex()];
  }
return true;
}

void GameDevice::StartVibration() { // Decl: 775

}

void GameDevice::StopVibration() { // Decl: 783

}

void GameDevice::PollDevice() { // Decl: 795

}

int GameDevice::GetNumDeviceScalar() {  // Decl: 898
        return this->mNumScalars;
};

GameDevice::GameDevice(int deviceIndex) : InputDevice(deviceIndex), IFeedback(nullptr){ // Decl: 903

}

GameDevice::~GameDevice(){ // Decl: 921

}; 

void PauseEffects() { // Decl: 934

}

void ResumeEffects() { // Decl: 949

}

void ResetEffects() { // Decl: 961

}

void BeginUpdate() { // Decl: 980

}

void EndUpdate() { // Decl: 1003

}

void UpdateRoadNoise(bool front, const struct SimSurface & surface, float speed) { // Decl: 1011

}

void UpdateTireSkid(bool front, const struct SimSurface & surface, float speed) { // Decl: 1043

}

void UpdateTireSlip(bool front, const struct SimSurface & surface, float speed) { // Decl: 1077

}

void UpdateRPM(float powerband, float overrev, float throttle) { // Decl: 1107

}

void UpdateShiftPotential(enum ShiftPotential potential) { // Decl: 1116

}

void UpdateEngineBlown(bool blown) { // Decl: 1207

}

void UpdateNOS(bool engaged, float NOSLevel) { // Decl: 1247

}

void UpdateShifting(bool shifting) { // Decl: 1286

}

void ReportCollision(const COLLISION_INFO & cinfo, bool iamA) { // Decl: 1320

}
