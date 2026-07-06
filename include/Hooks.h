#pragma once

#include "Settings.h"

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
                auto waterHeight = a_this->GetWaterHeight();
                if (actorPos.z + (actorHeight * Settings::GetSingleton()->Submerged) < waterHeight) {
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