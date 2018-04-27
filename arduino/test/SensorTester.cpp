#include "catch.hpp"
#include "Sensor.h"

using namespace Catch::Matchers;

TEST_CASE("Sensor tester", "[base]" ) {
    SECTION("Default constructor sets empty name") {
        Sensor sensor1;
        REQUIRE_THAT (sensor1.GetName (), Catch::Matchers::Equals(""));
    }

    SECTION("Set name") {
        Sensor sensor;
        sensor.SetName("Chuck Norris");
        REQUIRE_THAT (sensor.GetName (), Catch::Matchers::Equals("Chuck Norris"));
    }

    SECTION("Set name clips name after 31 characters") {
        Sensor sensor;
        sensor.SetName("Chuck Norris once got bitten by a cobra");
        REQUIRE_THAT (sensor.GetName (), Catch::Matchers::Equals("Chuck Norris once got bitten by"));
    }
}
