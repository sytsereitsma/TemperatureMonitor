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

        void MockWriteCommand (const void* inExpectedAddress, uint8_t inCommand, uint8_t inPower) {
            When(Method(mockOneWire, reset))
                .Return(1);

            auto compare = [inExpectedAddress] (const void* inAddress) {
                return memcmp (inExpectedAddress, inAddress, kAddressSize) == 0;
            };
            When(Method(mockOneWire, select)
                .Matching(compare))
                .Return();
            When(Method(mockOneWire, write)
                .Using(inCommand, inPower))
                .Return();
        }

        void MockWriteCommand (const DS18S20& ioDS, uint8_t inCommand, uint8_t inPower) {
            MockWriteCommand (ioDS.GetAddress (), inCommand, inPower);
        }

        Mock <IOneWire> mockOneWire;
    };
}

TEST_CASE( "DS18S20 testers", "[base]" ) {
    StringStream logStream;

    SECTION("Detects sensor address at construction") {
        Context ctx;
        ctx.MockDetection (kGoodAddress, kGoodCrc8);

        DS18S20 ds(ctx.mockOneWire.get (), logStream);

        std::string expectedAddress (kGoodAddress, kGoodAddress + kAddressSize);
        std::string actualAddress (ds.GetAddress (), ds.GetAddress () + kAddressSize);
        REQUIRE_THAT (actualAddress, Catch::Matchers::Equals(expectedAddress));
        
        REQUIRE_THAT (logStream.mStream.str(), Catch::Matchers::Contains(
            "Device found at pin 3: 28-010203040506-A5\n"
        ));
    }

    SECTION("Address is all zeroes when there is an address CRC error") {
        Context ctx;
        ctx.MockDetection (kGoodAddress, 0xDD);
        DS18S20 ds(ctx.mockOneWire.get (), logStream);

        REQUIRE (memcmp (kZeroAddress, ds.GetAddress (), kAddressSize) == 0);

        REQUIRE_THAT (logStream.mStream.str(), Catch::Matchers::Contains(
            "CRC is not valid for device at pin 3 (address 28-010203040506-A5, expected CRC 0xDD, got 0xA5).\n"
        ));
    }

    SECTION("Detection failure after successful detection resets address") {
        Context ctx;
        ctx.MockDetection (kGoodAddress, kGoodCrc8);

        DS18S20 ds(ctx.mockOneWire.get (), logStream);

        ctx.MockDetection (kGoodAddress, kGoodCrc8 + 1);
        REQUIRE (!ds.Detect ());

        REQUIRE (memcmp (kZeroAddress, ds.GetAddress (), kAddressSize) == 0);
    }

    SECTION("No devices on bus resets address") {
        Context ctx;

        When(Method(ctx.mockOneWire, search))
            .Return(0);

        DS18S20 ds(ctx.mockOneWire.get (), logStream);
        REQUIRE (memcmp (kZeroAddress, ds.GetAddress (), kAddressSize) == 0);

        REQUIRE_THAT (logStream.mStream.str(), Catch::Matchers::Contains(
            "No devices found at pin 3.\n"
        ));
    }

    SECTION("Detect parasitic power") {
        Context ctx;
        ctx.MockDetection (kGoodAddress, kGoodCrc8);
        ctx.MockWriteCommand (kGoodAddress, 0xB4, 0);

        When(Method(ctx.mockOneWire, read))
            .Return(0);

        DS18S20 ds(ctx.mockOneWire.get (), logStream);
        REQUIRE (ds.IsParasitePowered ());

        Verify(Method(ctx.mockOneWire,write)
            .Using(0xB4, 0))
            .Once ();

        REQUIRE_THAT (logStream.mStream.str(), Catch::Matchers::Contains(
            "Device 28-010203040506-A5 at pin 3 is parasite powered.\n"
        ));
    }

    SECTION("Detect VCC power") {
        Context ctx;
        ctx.MockDetection (kGoodAddress, kGoodCrc8);
        When(Method(ctx.mockOneWire, read))
            .Return(0xFF);

        DS18S20 ds(ctx.mockOneWire.get (), logStream);
        REQUIRE_FALSE (ds.IsParasitePowered ());

        //write parameter checked by "Detect parasitic power" test case

        REQUIRE_THAT (logStream.mStream.str(), Catch::Matchers::Contains(
            "Device 28-010203040506-A5 at pin 3 is not parasite powered.\n"
        ));
    }

    SECTION("Start temperature conversion") {
        Context ctx;
        ctx.MockDetection (kGoodAddress, kGoodCrc8);
        DS18S20 ds(ctx.mockOneWire.get (), logStream);

        ctx.MockWriteCommand (ds, 0x44, 1);

        REQUIRE (ds.StartConversion ());
    }

    SECTION("Start temperature conversion returns false when reset fails") {
        Context ctx;
        ctx.MockDetection (kGoodAddress, kGoodCrc8);
        DS18S20 ds(ctx.mockOneWire.get (), logStream);

        When(Method(ctx.mockOneWire, reset))
            .Return(0);
        REQUIRE_FALSE (ds.StartConversion ());
    }

    SECTION("Read temperature") {
        Context ctx;
        ctx.MockDetection (kGoodAddress, kGoodCrc8);
        DS18S20 ds(ctx.mockOneWire.get (), logStream);

        ctx.MockWriteCommand (ds, 0xBE, 0);

        constexpr uint8_t kScratchPadSize {9u};
        const uint8_t kScratchPad [kScratchPadSize] {0x12, 0x34, 2, 3, 4, 5, 6, 7, kGoodCrc8};
        {//Read scratchpad bytes
            When(Method(ctx.mockOneWire, read_bytes)
                .Using (_, kScratchPadSize))
                .Do ([kScratchPad](uint8_t* buffer, uint16_t count) {
                    memcpy(buffer, kScratchPad, kScratchPadSize);
                }); 
            When(Method(ctx.mockOneWire, crc8))
                .Return(kGoodCrc8);
        }

        float temperature;
        REQUIRE (ds.GetTemperature (temperature));

        const float kExpectedTemperature ((kScratchPad [0] + (kScratchPad [1] << 8)) / 16.0f);
        REQUIRE(temperature == Approx(kExpectedTemperature));
    }

    SECTION("Read temperature CRC error") {
        Context ctx;
        ctx.MockDetection (kGoodAddress, kGoodCrc8);
        DS18S20 ds(ctx.mockOneWire.get (), logStream);

        ctx.MockWriteCommand (ds, 0xBE, 0);

        constexpr uint8_t kScratchPadSize {9u};
        const uint8_t kScratchPad [kScratchPadSize] {0x12, 0x34, 2, 3, 4, 5, 6, 7, kGoodCrc8};
        {//Read scratchpad bytes
            When(Method(ctx.mockOneWire, read_bytes)
                .Using (_, kScratchPadSize))
                .Do ([kScratchPad](uint8_t* buffer, uint16_t count) {
                    memcpy(buffer, kScratchPad, kScratchPadSize);
                }); 
            When(Method(ctx.mockOneWire, crc8))
                .Return(kGoodCrc8);
        }

        float temperature;
        REQUIRE (ds.GetTemperature (temperature));

        const float kExpectedTemperature ((kScratchPad [0] + (kScratchPad [1] << 8)) / 16.0f);
        REQUIRE(temperature == Approx(kExpectedTemperature));

        Verify(Method(ctx.mockOneWire,write)
            .Using(0xBE, 0))
            .Once ();
    }
}
