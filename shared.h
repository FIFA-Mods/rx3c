#pragma once
#include "D3DDevice.h"

struct GlobalVars {
    D3DDevice *device = nullptr;
};

GlobalVars &globalVars();
