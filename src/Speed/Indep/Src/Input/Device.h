// #ifndef INPUT_DEVICE_H
// #define INPUT_DEVICE_H

// #ifdef EA_PRAGMA_ONCE_SUPPORTED
// #pragma once
// #endif

#include "ActionData.h"
#include "Speed/Indep/Libs/Support/Utility/FastMem.h"
#include "Speed/Indep/Src/Sim/SimModel.h"
#include "types.h"


enum Platform {
    PLATFORM_WIN = 0,
    PLATFORM_MAC = 1,
    PLATFORM_PS2 = 2,
    PLATFORM_XBOX = 3,
    PLATFORM_GC = 4,
    PLATFORM_XENON = 5,
    PLATFORM_PSP = 6,
    PLATFORM_MAX = 7,
};

enum RiResult {
    RI_OK = 0,
    RI_EVENT_QUEUE_FULL = 1,
    RI_EVENT_NOT_FOUND = 2,
    RI_FUNCTION_NOT_IMPLEMENTED = 3,
    RI_ACQUIRE_FAILED = 4,
    RI_NO_DIRECTINPUT_DEVICE = 5,
    RI_FAILED_GETTING_DEVICE_STATE = 6,
    RI_NO_EFFECT = 7,
};

struct Capabilities {
    uint32_t mNumDigitalButtons; // offset 0x0, size 0x4
    uint32_t mNumAnalogButtons; // offset 0x4, size 0x4
    unsigned int mAttached : 1; // offset 0x8, size 0x4
    unsigned int mForceFeedback : 1; // offset 0x8, size 0x4
    unsigned int mUnused : 30; // offset 0x8, size 0x4
};
union Data {
    // UNSOLVED can't seem to find these in the data?
    // struct Pad mPad; // offset 0x0, size 0x118
    // struct Keyboard mKeyboard; // offset 0x0, size 0x100
    // struct Mouse mMouse; // offset 0x0, size 0x14
};
enum Type {
    TYPE_UNKNOWN = 0,
    TYPE_KEYBOARD = 1,
    TYPE_MOUSE = 2,
    TYPE_PAD = 3,
};


struct Device {
    struct Info {
        Platform mPlatform; // offset 0x0, size 0x4
        Type mType; // offset 0x4, size 0x4
        uint32_t mJoypadID; // offset 0x8, size 0x4
        uint32_t mControllerID; // offset 0xC, size 0x4
        uint32_t mPortNum; // offset 0x10, size 0x4
    };

protected:
    Info mInfo; // offset 0x0, size 0x14
    Capabilities mCapabilities; // offset 0x14, size 0xC
    Data mData; // offset 0x20, size 0x118
    // UNSOLVED
    // This errors, unsure why
    // const struct __vtbl_ptr_type * _vptr.Device; // offset 0x138, size 0x4

    Device() ;

    virtual ~Device();

public:
    int32_t IsKeyboard() {}

    int32_t IsMouse() {}

    int32_t IsPad() {}

    Info* GetInfo() {}

    Capabilities* GetCapabilities() {}

    virtual Data* GetData() {}

    virtual RiResult Acquire() {}

    virtual RiResult Release() {}

    virtual RiResult Update() {}

    virtual Sim::Model::Effect* CreateEffect() {}

    virtual Sim::Model::Effect* GetEffect() {}

    virtual unsigned int GetKeyState(unsigned int) {}

    virtual unsigned int GetKeyState(unsigned int, unsigned int) {}

private:
    void InitData() ;

public:
    USE_FASTALLOC(Device);
    Device(enum Platform platform, enum Type type);
};

// #endif
