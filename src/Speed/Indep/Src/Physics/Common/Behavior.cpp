#include "Speed/Indep/Src/Physics/Behavior.h"
#include "Speed/Indep/Src/Generated/AttribSys/GenericAccessor.h"
#include "Speed/Indep/Src/Physics/PhysicsObject.h"

Behavior::Behavior(const BehaviorParams &params, unsigned int num_interfaces)
    : Sim::Object(num_interfaces + 1),   //
      mPaused(false),                    //
      mOwner(params.fowner),             //
      mIOwner(params.fowner),            //
      mMechanic(params.fMechanic),       //
      mSignature(params.fSig),           //
      mPriority(0),                      //
      mProfile(nullptr) {
    const Attrib::Instance &attribs = params.fowner->GetAttributes();
    unsigned int count = attribs->Num_BEHAVIOR_ORDER();
    while (this->mPriority < count) {
        if (attribs->BEHAVIOR_ORDER(this->mPriority) == this->mMechanic) {
            break;
        }
        this->mPriority = this->mPriority + 1;
    }
}

void Behavior::Pause(bool pause) {
    if (this->mPaused != pause) {
        this->mPaused = pause;
        if (pause) {
            this->OnPause();
        } else {
            this->OnUnPause();
        }
    }
}
