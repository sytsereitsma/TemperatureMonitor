#include "catch.hpp"
#include "fakeit.hpp"
#include <string>
#include "StringStream.h"
#include "ChangeReporter.h"
#include "constants.h"
#include "Sensor.h"

using namespace fakeit;

TEST_CASE( "ChangeReporter tester", "[base]" ) {
    OStringStream logStream;
    Mock <Sensor> mockSensor;
    mockSensor.get().SetName ("Frobozz");

    SECTION("FAIL.") {
        REQUIRE(false);
    }

#if 0
    SECTION("Reports values in NMEA style.") {
        ChangeReporter reporter (logStream, mockSensor);

        reporter.Update ();

        REQUIRE (logStream.mString == "$TMP,5-00-010203040506-07,1.23*4F\n");
    }
    SECTION("By default reports each update.") {
        ChangeReporter reporter (logStream, kAddress, 3);

        reporter.Update (1.23f);
        REQUIRE (logStream.GetAndReset () == "$TMP,3-00-010203040506-07,1.23*49\n");

        reporter.Update (1.23f);
        REQUIRE (logStream.GetAndReset () == "$TMP,3-00-010203040506-07,1.23*49\n");
    }

    SECTION("Only reports changes larger than minimum change.") {
        ChangeReporter reporter (logStream, kAddress, 3);
        const float kMinimumChange (0.2f);
        const float kInitialValue (1.23f);
        reporter.SetMinimumChange (kMinimumChange);

        reporter.Update (kInitialValue);
        logStream.mString = "";

        //Change not large enough
        reporter.Update (kInitialValue + kMinimumChange - 0.00001f);
        REQUIRE (logStream.mString == "");

        //Changed more than minimal change -> report
        reporter.Update (kInitialValue + kMinimumChange);
        REQUIRE (logStream.GetAndReset () == "$TMP,3-00-010203040506-07,1.43*4F\n");

        //Negative change almost back to initial value, but not enough
        reporter.Update (kInitialValue + 0.00001f);
        REQUIRE (logStream.mString == "");

        //Negative change back to initial value -> report
        reporter.Update (kInitialValue);
        REQUIRE (logStream.GetAndReset () == "$TMP,3-00-010203040506-07,1.23*49\n");
    }
#endif
}
