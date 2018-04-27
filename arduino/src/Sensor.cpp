#include "Sensor.h"
#include <string.h>

Sensor::Sensor () {
    mName [0] = '\0';
    mName [sizeof (mName) - 1] = '\0';
}

void Sensor::SetName (const char* inName) {
    strncpy (mName, inName, sizeof (mName) - 1);
}

const char* Sensor::GetName () const {
    return mName;
}
