#pragma once
#include "Rx3Utils.h"

enum ErrorDisplayType {
    ERR_MESSAGE_BOX,
    ERR_CONSOLE,
    ERR_NONE
};

extern ErrorDisplayType displayType;

void SetErrorDisplayType(ErrorDisplayType type);
bool ErrorMessage(string const &msg);
