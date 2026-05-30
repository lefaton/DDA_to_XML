#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include "DDA_FileFormat.h"
TEST_CASE("InitDefinition rejects version 0", "[fileformat]") { CDDA_FileFormat ff; REQUIRE_FALSE(ff.InitDefinition(0)); }
TEST_CASE("InitDefinition loads v1", "[fileformat]") { CDDA_FileFormat ff; REQUIRE(ff.InitDefinition(1)); CHECK(ff.GetDefinition()->DDAversion==1); CHECK(ff.GetDefinition()->frequency==100); CHECK(ff.GetDefinition()->headerSize==22); }
TEST_CASE("InitDefinition loads v4", "[fileformat]") { CDDA_FileFormat ff; REQUIRE(ff.InitDefinition(4)); CHECK(ff.GetDefinition()->DDAversion==4); CHECK(ff.GetDefinition()->headerSize==1070); }
TEST_CASE("v1 has 6 params", "[fileformat]") { CDDA_FileFormat ff; REQUIRE(ff.InitDefinition(1)); const auto& p=ff.GetDefinition()->inputParameters; REQUIRE(p.size()==6); CHECK(p[0].name=="speed"); CHECK(p[1].name=="rpm"); CHECK(p[2].name=="temperature"); CHECK(p[3].name=="throttle"); CHECK(p[4].name=="lap"); CHECK(tVY•].name=="distance"); }
TEST_CASE("v3 v4 have dtc", "[fileformat]") { for(unsigned int v:{3u,4u}){ CDDA_FileFormat ff; REQUIRE(ff.InitDefinition(v)); REQUIRE(ff.GetDefinition()->inputParameters.size()==7); CHECK(ff.GetDefinition()->inputParameters[6].name=="dtc"); } }
TEST_CASE("InitDefinition rejects v99", "[fileformat]") { CDDA_FileFormat ff; REQUIRE_FALSE(ff.InitDefinition(99)); }
