#include "catch.hpp"
#include "fakeit.hpp"
#include "DS18S20.h"
#include "IOneWire.h"
#include <string>
#include "StdOutStream.h"

using namespace fakeit;
namespace {
    const uint8_t kGoodCrc8 {0xA5}; //Not really, but we're mocking here, so it does not matter
    const uint8_t kGoodAddress [kAddressSize] = {0x28, 1, 2, 3, 4, 5, 6, kGoodCrc8};
    const uint8_t kZeroAddress [kAddressSize] = {0};

    struct Context {
        Context () {
            When(Method(mockOneWire, search))
                .Return(0);
            When(Method(mockOneWire, reset_search))
                .Return();
            When(Method(mockOneWire, crc8))
                .Return(0);
            When(Method(mockOneWire, write))
                .Return();
            When(Method(mockOneWire, read))
                .Return(0);
            When(Method(mockOneWire, read_bytes))
                .Return();
            When(Method(mockOneWire, select))
                .Return();
            When(Method(mockOneWire, reset))
                .Return(1);
            When(Method(mockOneWire, GetPin))
                .AlwaysReturn(3);
        }

        void MockDetection (const uint8_t* inAddress, uint8_t inCRC) {
            When(Method(mockOneWire, crc8))
                .Return(inCRC);

            When(Method(mockOneWire, search))
                .Do ([inAddress](uint8_t* address, bool search_mode) -> uint8_t {
                    memcpy(address, inAddress, kAddressSize);
                    return 1;
                }); 
        }

        Mock <IOneWire> mockOneWire;
    };
}

TEST_CASE( "DS18S20 testers", "[base]" ) {
    StdOutStream logStream;

    SECTION("Detects sensor address at construction") {
        Context ctx;
        ctx.MockDetection (kGoodAddress, kGoodCrc8);

        DS18S20 ds(ctx.mockOneWire.get (), logStream);

        std::string expectedAddress (kGoodAddress, kGoodAddress + kAddressSize);
        std::string actualAddress (ds.GetAddress (), ds.GetAddress () + kAddressSize);
        REQUIRE_THAT (actualAddress, Catch::Matchers::Equals(expectedAddress));
    }

    SECTION("Address is all zeroes when there is an address CRC error") {
        Context ctx;
        When(Method(ctx.mockOneWire, crc8))
            .Return(0xDD);

        When(Method(ctx.mockOneWire, search))
            .Return(1);

        DS18S20 ds(ctx.mockOneWire.get (), logStream);

        REQUIRE (memcmp (kZeroAddress, ds.GetAddress (), kAddressSize) == 0);
    }

    SECTION("Detection failure after successful detection resets address") {
        Context ctx;
        ctx.MockDetection (kGoodAddress, kGoodCrc8);

        DS18S20 ds(ctx.mockOneWire.get (), logStream);

        ctx.MockDetection (kGoodAddress, kGoodCrc8 + 1);
        REQUIRE (!ds.Detect ());

        REQUIRE (memcmp (kZeroAddress, ds.GetAddress (), kAddressSize) == 0);
    }

    SECTION("Detect parasitic power") {
        Context ctx;
        ctx.MockDetection (kGoodAddress, kGoodCrc8);
        When(Method(ctx.mockOneWire, read))
            .Return(0);

        DS18S20 ds(ctx.mockOneWire.get (), logStream);
        REQUIRE (ds.IsParasitePowered ());

        Verify(Method(ctx.mockOneWire,write)
            .Using(0xB4, 0))
            .Once ();
    }

    SECTION("Detect VCC power") {
        Context ctx;
        ctx.MockDetection (kGoodAddress, kGoodCrc8);
        When(Method(ctx.mockOneWire, read))
            .Return(0xFF);

        DS18S20 ds(ctx.mockOneWire.get (), logStream);
        REQUIRE (!ds.IsParasitePowered ());

        //write parameter checked by "Detect parasitic power" test case
    }
}
