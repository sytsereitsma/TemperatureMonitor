#include "catch.hpp"
#include "fakeit.hpp"
#include "DS18S20.h"
#include "IOneWire.h"
#include <string>
#include <memory>
#include "StringStream.h"

using namespace fakeit;
namespace {
    const uint8_t kGoodCrc8 {0xA5}; //Not really, but we're mocking here, so it does not matter
    const uint8_t kGoodAddress [kAddressSize] = {0x28, 1, 2, 3, 4, 5, 6, kGoodCrc8};
    const uint8_t kZeroAddress [kAddressSize] = {0};
    constexpr uint8_t kScratchPadSize {9u};
    const uint8_t kScratchPad [kScratchPadSize] {0x12, 0x34, 2, 3, 4, 5, 6, 7, kGoodCrc8};

    struct Context {
        Context () :
            ds (mockOneWire.get (), logStream)
        {
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

        void MockWriteCommand (uint8_t inCommand, uint8_t inPower) {
            When(Method(mockOneWire, reset))
                .Return(1);

            auto compare = [this] (const void* inAddress) {
                return memcmp (ds.GetAddress (), inAddress, kAddressSize) == 0;
            };
            When(Method(mockOneWire, select)
                .Matching(compare))
                .Return();
            When(Method(mockOneWire, write)
                .Using(inCommand, inPower))
                .Return();
        }

        bool DoSuccessfulDetection () {
            MockDetection (kGoodAddress, kGoodCrc8);
            return ds.Detect ();
        }

        void MockReadScratchpad (uint8_t inComputedCRC) {
            When(Method(mockOneWire, read_bytes)
                .Using (_, kScratchPadSize))
                .Do ([](uint8_t* buffer, uint16_t count) {
                    memcpy(buffer, kScratchPad, kScratchPadSize);
                }); 
            When(Method(mockOneWire, crc8))
                .Return(inComputedCRC);
        }

        Mock <IOneWire> mockOneWire;
        OStringStream logStream;
        DS18S20 ds;
    };
}

TEST_CASE( "DS18S20 testers", "[base]" ) {
    Context ctx;

    SECTION("Sets sensor name to TEMP-<pin>-<address>") {
        REQUIRE (ctx.DoSuccessfulDetection ());
        REQUIRE_THAT (ctx.ds.GetName (), Catch::Matchers::Equals("TEMP-3-28-010203040506-A5"));
    }

    SECTION("Detects sensor address at detection") {
        REQUIRE (ctx.DoSuccessfulDetection ());

        std::string expectedAddress (kGoodAddress, kGoodAddress + kAddressSize);
        std::string actualAddress (ctx.ds.GetAddress (), ctx.ds.GetAddress () + kAddressSize);
        REQUIRE_THAT (actualAddress, Catch::Matchers::Equals(expectedAddress));
            REQUIRE_THAT (ctx.logStream.mString, Catch::Matchers::Contains(
            "Device found at pin 3: 28-010203040506-A5\n"
        ));
    }

    SECTION("Address is all zeroes when there is an address CRC error") {
        ctx.MockDetection (kGoodAddress, kGoodCrc8 + 1);
        REQUIRE_FALSE (ctx.ds.Detect ());

        REQUIRE (memcmp (kZeroAddress, ctx.ds.GetAddress (), kAddressSize) == 0);

        REQUIRE_THAT (ctx.logStream.mString, Catch::Matchers::Contains(
            "CRC is not valid for device at pin 3 (address 28-010203040506-A5, expected CRC 0xA6, got 0xA5).\n"
        ));
    }

    SECTION("Detection failure after successful detection resets address") {
        REQUIRE (ctx.DoSuccessfulDetection ());

        ctx.MockDetection (kGoodAddress, kGoodCrc8 + 1);
        REQUIRE_FALSE (ctx.ds.Detect ());

        REQUIRE (memcmp (kZeroAddress, ctx.ds.GetAddress (), kAddressSize) == 0);
    }

    SECTION("No devices on bus resets address") {
        When(Method(ctx.mockOneWire, search))
            .Return(0);

        REQUIRE_FALSE (ctx.ds.Detect ());

        REQUIRE (memcmp (kZeroAddress, ctx.ds.GetAddress (), kAddressSize) == 0);

        REQUIRE_THAT (ctx.logStream.mString, Catch::Matchers::Contains(
            "No devices found at pin 3.\n"
        ));
    }

    SECTION("Detect parasitic power") {
        ctx.MockWriteCommand (0xB4, 0);
        When(Method(ctx.mockOneWire, read))
            .Return(0);

        REQUIRE (ctx.DoSuccessfulDetection ());
        REQUIRE (ctx.ds.IsParasitePowered ());

        Verify(Method(ctx.mockOneWire,write)
            .Using(0xB4, 0))
            .Once ();

        REQUIRE_THAT (ctx.logStream.mString, Catch::Matchers::Contains(
            "Device 28-010203040506-A5 at pin 3 is parasite powered.\n"
        ));
    }

    SECTION("Detect VCC power") {
        ctx.MockWriteCommand (0xB4, 0);
        When(Method(ctx.mockOneWire, read))
            .Return(0xFF);

        REQUIRE (ctx.DoSuccessfulDetection ());
        REQUIRE_FALSE (ctx.ds.IsParasitePowered ());

        REQUIRE_THAT (ctx.logStream.mString, Catch::Matchers::Contains(
            "Device 28-010203040506-A5 at pin 3 is not parasite powered.\n"
        ));
    }

    SECTION("Start temperature conversion") {
        REQUIRE (ctx.DoSuccessfulDetection ());

        ctx.MockWriteCommand (0x44, 1);

        REQUIRE (ctx.ds.StartConversion ());
    }

    SECTION("Start temperature conversion returns false when reset fails") {
        REQUIRE (ctx.DoSuccessfulDetection ());

        When(Method(ctx.mockOneWire, reset))
            .Return(0);
        REQUIRE_FALSE (ctx.ds.StartConversion ());
    }

    SECTION("Read temperature") {
        REQUIRE (ctx.DoSuccessfulDetection ());

        ctx.MockWriteCommand (0xBE, 0);
        ctx.MockReadScratchpad (kGoodCrc8);

        float temperature;
        REQUIRE (ctx.ds.GetTemperature (temperature));

        const float kExpectedTemperature ((kScratchPad [0] + (kScratchPad [1] << 8)) / 16.0f);
        REQUIRE(temperature == Approx(kExpectedTemperature));

        Verify(Method(ctx.mockOneWire, write)
            .Using(0xBE, 0))
            .Once ();
    }

    SECTION("Read temperature CRC error returns false") {
        REQUIRE (ctx.DoSuccessfulDetection ());

        ctx.MockWriteCommand (0xBE, 0);
        ctx.MockReadScratchpad (kGoodCrc8 + 1);

        float temperature;
        REQUIRE_FALSE (ctx.ds.GetTemperature (temperature));

        REQUIRE_THAT (ctx.logStream.mString, Catch::Matchers::Contains(
            "Scratchpad CRC is not valid for device 28-010203040506-A5 at pin 3 (expected CRC 0xA6, got 0xA5).\n"
        ));
    }
}
