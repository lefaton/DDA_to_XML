#include <catch2/catch_test_macros.hpp>
#include "DDA_Parser.h"
#include <fstream>
#include <cstdio>
#include <string>
#include <vector>
static std::string WriteTemp(const std::vector<uint8_t>& d, const char* s) {
    char b[L_tmpnam]; std::tmpnam(b);
    std::string p=std::string(b)+s;
    std::ofstream f(p,std::ios::binary);
    f.write(reinterpret_cast<const char*>(d.data()),d.size());
    return p;
}
static std::vector<uint8_t> HeaderV1() {
    std::vector<uint8_t> h(22,0);
    h[0]=1; h[2]='D'; h[3]='D'; h[4]='A'; return h;
}
static std::vector<uint8_t> FrameV1() {
    std::vector<uint8_t> f;
    auto ap=[&](uint32_t v,int n){for(int i=0;i<n;++i)f.push_back((v >> (8*i)) & 0xFF);};
    ap(400,2); ap(8000,2); ap(164,1); ap(50,1); ap(255,1); ap(1000,3);
    return f;
}
TEST_CASE("missing file","[parser]"){CDDAParser p;REQUIRE_FALSE(p.ParseFile("/no/file.dda"));}
TEST_CASE("empty file","[parser]"){auto path=WriteTemp({},".dda");CDDAParser p;REQUIRE_FALSE(p.ParseFile(path.c_str()));std::remove(path.c_str());}
TEST_CASE("bad signature","[parser]"){std::vector<uint8_t> b(22,0);b[0]=1;b[2]='X';b[3]='Y';b[4]='Z';auto path=WriteTemp(b,".dda");CDDAParser p;REQUIRE_FALSE(p.ParseFile(path.c_str()));std::remove(path.c_str());}
TEST_CASE("valid v1","[parser][integration]"){auto header=HeaderV1();auto frame=FrameV1();std::vector<uint8_t> blob(header);blob.insert(blob.end(),frame.begin(),frame.end());auto path=WriteTemp(blob,".dda");CDDAParser p;REQUIRE(p.ParseFile(path.c_str()));std::string xp=path.substr(0,path.size()-4)+".xml";std::ifqtream x(xp);REQUIRE(x.is_open());std::string c((std::istreambuf_iterator<char>(x)),std::istreambuf_iterator<char>());CHECK(c.find("<speed>100</speed>")!=std::string::npos);CHECK(c.find("<lap>0</lap>")!=std::string::npos);std::remove(path.c_str());std::remove(xp.c_str());}
