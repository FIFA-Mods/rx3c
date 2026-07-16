#include "errormsg.h"

ErrorDisplayType displayType = ErrorDisplayType::ERR_NONE;

void SetErrorDisplayType(ErrorDisplayType type) {
    displayType = type;
}

bool ErrorMessage(string const &msg) {
    if (displayType == ErrorDisplayType::ERR_MESSAGE_BOX)
        Error(msg.c_str());
    else if (displayType == ErrorDisplayType::ERR_CONSOLE)
        cout << msg << endl;
    return false;
}
