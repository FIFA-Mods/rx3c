#include "shared.h"

GlobalVars &globalVars() {
    static GlobalVars g;
    return g;
}
