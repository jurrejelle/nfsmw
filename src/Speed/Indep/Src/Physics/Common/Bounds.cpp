#include "Speed/Indep/Src/Physics/Bounds.h"
#include "Speed/Indep/Libs/Support/Utility/UMath.h"
#include "Speed/Indep/Src/Physics/Dynamics.h"
#include "Speed/Indep/bWare/Inc/bWare.hpp"

inline void bPlatEndianSwap(UMath::Vector4 *v) {
    ::bPlatEndianSwap(&v->x);
    ::bPlatEndianSwap(&v->y);
    ::bPlatEndianSwap(&v->z);
    ::bPlatEndianSwap(&v->w);
}

namespace CollisionGeometry {

static Collections TheCollections;

inline Collection *BoundsPack::Table::Find(UCrc32 name) {
    Pair *iter = _STL::lower_bound(this->begin(), this->end(), Pair(name, nullptr));
    if (iter != this->end() && iter->Name == name) {
        return iter->Collection;
    }
    return nullptr;
}

BoundsPack::BoundsPack(bChunk *pack) : mChunk(pack) {
    bChunk *chunk;
    int count = 0;
    bChunk *last_chunk = pack->GetLastChunk();
    for (chunk = pack->GetFirstChunk(); chunk < last_chunk; chunk = chunk->GetNext()) {
        count = count + 1;
    }
    this->mTable.reserve(count);

    for (chunk = pack->GetFirstChunk(); chunk < last_chunk; chunk = chunk->GetNext()) {
        BoundsHeader *pheader = reinterpret_cast<BoundsHeader *>(chunk->GetAlignedData(16));
        UCrc32 name(pheader->fNameHash);
        if (pheader->fIsResolved == 0) {
            ::bPlatEndianSwap(&name);
        }

        if (this->mTable.Find(UCrc32(name)) == nullptr) {
            reinterpret_cast<Collection *>(pheader)->Init();
            this->mTable.Add(reinterpret_cast<Collection *>(pheader));
        }
    }
}

BoundsPack *Collections::Find(const bChunk *header) {
    for (BoundsPack *pack = this->GetHead(); pack != this->EndOfList(); pack = pack->GetNext()) {
        if (pack->GetHeader() == header) {
            return pack;
        }
    }
    return nullptr;
}

const Collection *Collections::Find(UCrc32 name) {
    for (BoundsPack *pack = this->GetHead(); pack != this->EndOfList(); pack = pack->GetNext()) {
        const Collection *collection = pack->Find(UCrc32(name));
        if (collection != nullptr) {
            return collection;
        }
    }
    return nullptr;
}

const Bounds *Collection::GetRoot() const {
    if (this->fNumBounds > 0) {
        return this->GetBounds();
    }
    return nullptr;
}

const Bounds *Collection::GetChild(const Bounds *parent, UCrc32 name) const {
    if (parent->fChildIndex >= 0) {
        for (int idx = 0; idx < static_cast<int>(parent->fNumChildren); idx++) {
            const Bounds *bnds = &this->GetBounds()[parent->fChildIndex + idx];
            if (bnds->fNameHash == name) {
                return bnds;
            }
        }
    }
    return nullptr;
}

const Bounds *Collection::GetChild(const Bounds *parent, unsigned int idx) const {
    if (idx < parent->fNumChildren && parent->fChildIndex >= 0) {
        return &this->GetBounds()[parent->fChildIndex + idx];
    }
    return nullptr;
}

const PCloud *Collection::GetPointCloud(const Bounds *parent) const {
    if (this->GetPCHeader()->fNumPClouds > 0 && parent->fPCloudIndex >= 0) {
        const PCloud *pcloud = this->GetPCloud();
        for (int i = 0; i < this->GetPCHeader()->fNumPClouds; i++) {
            if (i == static_cast<int>(parent->fPCloudIndex)) {
                return pcloud;
            }
            pcloud = reinterpret_cast<const PCloud *>(pcloud->fPList + pcloud->fNumVerts);
        }
    }
    return nullptr;
}

const Bounds *Collection::GetBounds(UCrc32 hash_name) const {
    if (hash_name != UCrc32::kNull) {
        for (int i = 0; i < this->fNumBounds; i++) {
            if (this->GetBounds()[i].fNameHash == hash_name) {
                return &this->GetBounds()[i];
            }
        }
    }
    return nullptr;
}

void Collection::Init() {
    if (this->fIsResolved == 0) {
        ::bPlatEndianSwap(&this->fNameHash);
        ::bPlatEndianSwap(&this->fNumBounds);
    }
    if (this->fIsResolved == 0) {
        int i;
        PCloud *pcloud;
        for (i = 0; i < this->fNumBounds; i++) {
            Bounds &bounds = this->GetBounds()[i];
            bounds.fPosition.EndianSwap();
            bounds.fHalfDimensions.EndianSwap();
            bounds.fOrientation.EndianSwap();
            bounds.fPivot.EndianSwap();
            ::bPlatEndianSwap(&bounds.fNumChildren);
            ::bPlatEndianSwap(&bounds.fChildIndex);
            ::bPlatEndianSwap(&bounds.fRadius);
            ::bPlatEndianSwap(&bounds.fPCloudIndex);
            ::bPlatEndianSwap(&bounds.fFlags);
            ::bPlatEndianSwap(&bounds.fNameHash);
            ::bPlatEndianSwap(&bounds.fSurface);
        }
        ::bPlatEndianSwap(&this->GetPCHeader()->fNumPClouds);
        pcloud = this->GetPCloud();
        i = 0;
        while (i < this->GetPCHeader()->fNumPClouds) {
            ::bPlatEndianSwap(&pcloud->fNumVerts);
            i = i + 1;
            pcloud->fPList = reinterpret_cast<UMath::Vector4 *>(pcloud + 1);
            for (int j = 0; j < pcloud->fNumVerts; j++) {
                ::bPlatEndianSwap(&pcloud->fPList[j]);
            }
            pcloud = reinterpret_cast<PCloud *>(pcloud->fPList + pcloud->fNumVerts);
        }
        this->fIsResolved = 1;
    } else {
        PCloud *pcloud = this->GetPCloud();
        for (int i = 0; i < this->GetPCHeader()->fNumPClouds; i++) {
            pcloud->fPList = reinterpret_cast<UMath::Vector4 *>(pcloud + 1);
            pcloud = reinterpret_cast<PCloud *>(pcloud->fPList + pcloud->fNumVerts);
        }
    }

    for (int i = 0; i < this->fNumBounds; i++) {
        this->GetBounds()[i].fCollection = this;
    }
}

bool Collection::AddTo(IBoundable *irbc, const Bounds *root, const SimSurface &defsurface, bool parsechildren) const {
    bool added = false;
    if (root != nullptr) {
        if (parsechildren && root->fNumChildren != 0) {
            for (unsigned int i = 0; i < root->fNumChildren; i++) {
                const Bounds *geom = this->GetChild(root, i);
                if (this->AddNode(irbc, geom, defsurface, true)) {
                    added = true;
                }
            }
        }
        if (!added) {
            if (this->AddNode(irbc, root, defsurface, false)) {
                added = true;
            }
        }
    }
    return added;
}

bool Collection::AddNode(IBoundable *iboundable, const Bounds *geom, const SimSurface &defsurface, bool ischild) const {
    bool result = false;
    UMath::Vector3 offset;
    UMath::Vector3 dim;
    UMath::Vector4 orientation;
    UMath::Matrix4 invmat;

    geom->GetHalfDimensions(dim);
    geom->GetPosition(offset);
    geom->GetOrientation(orientation);
    invmat = UMath::Matrix4::kIdentity;
    SimSurface surface(defsurface);

    if (geom->fSurface.GetValue() != 0) {
        surface = SimSurface(SimSurface::Lookup(geom->fSurface));
        if (surface == SimSurface::kNull) {
            surface = defsurface;
        }
    }

    if (ischild == true && (geom->fFlags & kBounds_Internal)) {
        result = false;
        return result;
    }

    if (!ischild) {
        UMath::QuaternionToMatrix4(orientation, invmat);
        invmat.v3 = UMath::Vector4Make(offset, 1.0f);
        OrthoInverse(invmat);
        offset = UMath::Vector3::kZero;
        orientation = UMath::Vector4::kIdentity;
    }

    if (geom->fFlags & (kBounds_PrimVsWorld | kBounds_PrimVsObjects | kBounds_PrimVsGround)) {
        if (iboundable->AddCollisionPrimitive(geom->fNameHash, dim, geom->fRadius, offset, surface, orientation,
                                              static_cast<BoundFlags>(geom->fFlags))) {
            result = true;
        }
    }

    if (geom->fFlags & kBounds_MeshVsGround) {
        const PCloud *pcloud = this->GetPointCloud(geom);
        if (pcloud != nullptr && pcloud->fNumVerts > 0) {
            if (!ischild) {
                UMath::Vector4 tmp[16];
                for (int i = 0; i < pcloud->fNumVerts; i++) {
                    UMath::Vector4 in = pcloud->fPList[i];
                    in.w = 1.0f;
                    UMath::RotateTranslate(in, invmat, tmp[i]);
                }
                iboundable->AddCollisionMesh(geom->fNameHash, tmp, pcloud->fNumVerts, surface, static_cast<BoundFlags>(geom->fFlags), false);
            } else {
                iboundable->AddCollisionMesh(geom->fNameHash, pcloud->fPList, pcloud->fNumVerts, surface, static_cast<BoundFlags>(geom->fFlags),
                                             true);
            }
            result = true;
        }
    }

    return result;
}

const Collection *Lookup(UCrc32 object_name_hash) {
    return TheCollections.Find(UCrc32(object_name_hash));
}

}; // namespace CollisionGeometry

int LoaderBounds(bChunk *chunk) {
    if (chunk->GetID() != 0x8003b900) {
        return 0;
    }
    CollisionGeometry::TheCollections.AddHead(new CollisionGeometry::BoundsPack(chunk));
    return 1;
}

int UnloaderBounds(bChunk *chunk) {
    if (chunk->GetID() != 0x8003b900) {
        return 0;
    }
    CollisionGeometry::BoundsPack *pack = CollisionGeometry::TheCollections.Find(chunk);
    if (pack != nullptr) {
        CollisionGeometry::TheCollections.Remove(pack);
        delete pack;
    }
    return 1;
}
