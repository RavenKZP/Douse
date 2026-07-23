#pragma once

#include "Settings.h"
#include "ClibUtil/editorID.hpp"

std::string ToLower(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
    return s;
}

// Based on po3's Splashes-of-Skyrim
// https://github.com/powerof3/Splashes-of-Skyrim
// water height, isLava
std::pair<float, bool> get_water_height(const RE::NiPoint3& a_pos) {
    float waterHeight = -RE::NI_INFINITY;
    bool isLava = false;

    if (const auto waterManager = RE::TESWaterSystem::GetSingleton()) {
        const RE::BSSpinLockGuard locker(waterManager->lock);

        for (const auto& waterObjectPtr : waterManager->waterObjects) {
            const auto waterObject = waterObjectPtr.get();
            if (!waterObject) {
                continue;
            }
            for (const auto& boundPtr : waterObject->multiBounds) {
                const auto bound = boundPtr.get();
                if (!bound) {
                    continue;
                }
                if (auto size{bound->size}; size.z <= 10.0f) {  // avoid sloped water
                    auto center{bound->center};
                    const auto boundMin = center - size;
                    const auto boundMax = center + size;
                    if (!(a_pos.x < boundMin.x || a_pos.x > boundMax.x || a_pos.y < boundMin.y ||
                          a_pos.y > boundMax.y)) {
                        waterHeight = center.z;

                        bool lava = false;
                        auto editorID = clib_util::editorID::get_editorID(waterObject->waterType);
                        if (!editorID.empty()) {
                            std::string lowerStr = ToLower(editorID);
                            if (lowerStr.find("lava") != std::string::npos) {
                                lava = true;
                            }
                        }
                        return std::make_pair(waterHeight, lava);
                    }
                }
            }
        }
    }

    return std::make_pair(-RE::NI_INFINITY, false);
}

namespace Hooks {
    struct ProcessInWaterHook {
        static bool ProcessInWater(RE::Actor* a_this, RE::hkpCollidable* a_collidable, float a_waterHeight,
                                   float a_deltaTime) {  // 09C

            if (!a_this || a_this->IsDead() || a_this->IsDisabled() || a_this->IsDeleted() || !a_this->Is3DLoaded() ||
                a_this->IsEssential() || a_this->IsProtected()) {
                return ProcessInWater_(a_this, a_collidable, a_waterHeight, a_deltaTime);
            }
            if (a_this->HasKeywordString("Douse_KillByWater")) {
                float actorHeight = a_this->GetHeight();
                auto actorPos = a_this->GetPosition();
                auto [waterHeight, isLava] = get_water_height(actorPos);
                if (!isLava && actorPos.z + (actorHeight * Settings::GetSingleton()->Submerged) < waterHeight) {
                    a_this->DoDamage(99999, nullptr, true);
                }
            }
            return ProcessInWater_(a_this, a_collidable, a_waterHeight, a_deltaTime);
        }
        static inline REL::Relocation<decltype(ProcessInWater)> ProcessInWater_;
    };

    inline void InstallHooks() {
        ProcessInWaterHook::ProcessInWater_ =
            REL::Relocation<std::uintptr_t>(RE::VTABLE_Character[0])
                .write_vfunc(REL::Relocate(0x9C, 0x9C, 0x9E), ProcessInWaterHook::ProcessInWater);
        // VR might not work
    }
}