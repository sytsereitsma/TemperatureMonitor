#include "catch.hpp"
#include <string>
#include "StringStream.h"
#include "utils.h"
#include "constants.h"

using namespace Catch::Matchers;

TEST_CASE( "Utilities tester", "[base]" ) {
    StringStream logStream;

    SECTION("Generic types are printed") {
        //Just try some random types
        logStream << int {123};
        REQUIRE (logStream.GetAndReset () == "123");

        logStream << float {123.345}; //Defaults to 2 digits precision
        REQUIRE (logStream.GetAndReset () == "123.35");

        logStream << 'A';
        REQUIRE (logStream.GetAndReset () == "A");
    }

    SECTION("Print hex") {
        logStream << Hex {123};
        REQUIRE (logStream.GetAndReset () == "7B");

        logStream << Hex {5};
        REQUIRE (logStream.GetAndReset () == "05");
    }

    SECTION("Print on wire address") {
        const uint8_t kAddress [kAddressSize] = {0, 1, 2, 3, 4, 5, 6, 33};
        logStream << OneWireAddress {kAddress}; //Prints hex values of the address bytes
        REQUIRE (logStream.GetAndReset () == "00-010203040506-21");
    }

    SECTION("NMEA checksum") {
        const uint8_t kBufferSize (7);
        const uint8_t kBuffer [kBufferSize] = {0, 11, 2, 33, 4, 55, 6};
        REQUIRE (NMEAChecksum (kBuffer, kBuffer + kBufferSize) == 0x1D);
    }
}
