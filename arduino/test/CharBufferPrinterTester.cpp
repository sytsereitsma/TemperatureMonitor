#include "catch.hpp"
#include "CharBufferPrinter.h"

using namespace Catch::Matchers;

TEST_CASE("CharBufferPrinter tester", "[base]" ) {
    const size_t kSize (12);
    char buffer [kSize];

    SECTION("Constructor terminates buffer") {
        strcpy (buffer, "boo");
        CharBufferPrinter printer (buffer, kSize);
        REQUIRE_THAT (buffer, Catch::Matchers::Equals(""));
    }

    SECTION("Printing") {        
        CharBufferPrinter printer (buffer, kSize);
        printer.print("hoera");
        REQUIRE_THAT (buffer, Catch::Matchers::Equals("hoera"));

        printer.print("123");
        REQUIRE_THAT (buffer, Catch::Matchers::Equals("hoera123"));

    }

    SECTION("Clips print and zero terminates when buffer is full") {        
        CharBufferPrinter printer (buffer, kSize);
        printer.print("Frobozz electric");
        REQUIRE_THAT (buffer, Catch::Matchers::Equals("Frobozz ele"));
    }
}
