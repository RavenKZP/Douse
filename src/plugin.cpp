#include "logger.h"

#include "Hooks.h"

SKSEPluginLoad(const SKSE::LoadInterface *skse) {
    SKSE::Init(skse);
    Hooks::InstallHooks();
    return true;
}
