#include "SmackableTrigger.h"
#include "Speed/Indep/Libs/Support/Utility/FastMem.h"
#include "Speed/Indep/Libs/Support/Utility/UMath.h"
#include "Speed/Indep/Src/World/WCollisionAssets.h"

SmackableTrigger::SmackableTrigger(HMODEL hmodel, bool virgin, const UMath::Matrix4 &objectmatrix, const UMath::Vector3 &dim,
                                   unsigned int extra_flags) {
    unsigned int flags = extra_flags | 0x40143;
    void *eventMem = gFastMem.Alloc(0x48, "SmackTrigger");
    CARP::EventList *el = static_cast<CARP::EventList *>(eventMem);
    this->mTrigger = new WTrigger(objectmatrix, dim, el, flags);
    el->fNumEvents = 1;
    CARP::EventStaticData *es = el->Event();
    this->mEventData = reinterpret_cast<ESpawnSmackable::StaticData *>(&es[1]);
    es->fDataOffset = 0x10;
    es->fEventID = ESpawnSmackable::kEventID;
    es->fPad = 0;
    es->fEventSize = 0x38;
    WCollisionAssets::Get().AddTrigger(this->mTrigger);
    if (!virgin) {
        this->mTrigger->UpdateBox(objectmatrix, dim);
    }
    this->mEventData->fScenery = hmodel;
    this->mEventData->fVirginSpawn = virgin;
    UMath::Matrix4ToQuaternion(objectmatrix, this->mEventData->fOrientation);
    this->mEventData->fPosition = UMath::Vector4To3(objectmatrix.v3);
    this->mTrigger->Enable();
}

void SmackableTrigger::Fire() {
    this->mTrigger->FireEvents(nullptr);
}

void SmackableTrigger::Disable() {
    this->mTrigger->Disable();
}

void SmackableTrigger::Enable() {
    this->mTrigger->Enable();
}

bool SmackableTrigger::IsEnabled() const {
    if (this->mTrigger->IsEnabled()) {
        return true;
    }
    return false;
}

void SmackableTrigger::GetObjectMatrix(UMath::Matrix4 &matrix) const {
    UMath::Vector4 q = this->mEventData->fOrientation;
    UMath::QuaternionToMatrix4(q, matrix);
    matrix.v3 = UMath::Vector4Make(this->mEventData->fPosition, 1.0f);
}

void SmackableTrigger::Move(const UMath::Matrix4 &matrix, const UMath::Vector3 &dim, bool virgin) {
    this->mTrigger->UpdateBox(matrix, dim);
    this->mEventData->fVirginSpawn = virgin;
    UMath::Matrix4ToQuaternion(matrix, this->mEventData->fOrientation);
    this->mEventData->fPosition = UMath::Vector4To3(matrix.v3);
}

SmackableTrigger::~SmackableTrigger() {
    gFastMem.Free(this->mTrigger->fEvents, 0x48, "SmackTrigger");
    this->mTrigger->fEvents = nullptr;
    WCollisionAssets::Get().RemoveTrigger(this->mTrigger);
    delete this->mTrigger;
    this->mTrigger = nullptr;
    this->mEventData = nullptr;
}
