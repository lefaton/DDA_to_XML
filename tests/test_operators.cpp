#include <catch2/catch_test_macros.hpp>
#include "DDA_Operators.h"
TEST_CASE("nil","[operators]"){CHECK(ApplyOperator(1234,SDDAParam::EOperator::nil,0)==1234);}
TEST_CASE("add","[operators]"){CHECK(ApplyOperator(100,SDDAParam::EOperator::add,10)==110);}
TEST_CASE("sub","[operators]"){CHECK(ApplyOperator(164,SDDAParam::EOperator::sub,64)==100);}
TEST_CASE("mul","[operators]"){CHECK(ApplyOperator(5,SDDAParam::EOperator::mul,3)==15);}
TEST_CASE("div","[operators]"){CHECK(ApplyOperator(400,SDDAParam::EOperator::div,4)==100);}
TEST_CASE("div by zero","[operators]"){CHECK(ApplyOperator(42,SDDAParam::EOperator::div,0)==42);}
TEST_CASE("dif","[operators]"){CHECK(ApplyOperator(255,SDDAParam::EOperator::dif,255)==0);CHECK(ApplyOperator(3,SDDAParam::EOperator::dif,255)==3);}
