#ifndef BUILD_REGION_HPP
#define BUILD_REGION_HPP

namespace BuildRegion {

bool IsAmerica();
bool IsEurope();
bool IsEuropeFr();
bool IsEuropeGer();
bool IsJapan();
bool IsPal();

inline const char *GetCarBadgingSuffix() {
    if (IsEurope()) {
        return "_EU";
    } else {
        return nullptr;
    }
}

}; // namespace BuildRegion

#endif
