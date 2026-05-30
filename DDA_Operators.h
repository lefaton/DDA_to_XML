#pragma once
#include "DDA_FileFormat.h"
// Applies the configured operator+value transform to a raw binary value.
// Single source of truth used by both the parser and the tests.
inline unsigned int ApplyOperator(unsigned int raw, SDDAParam::EOperator op, unsigned int val) {
    switch (op) {
        case SDDAParam::EOperator::add: return raw + val;
        case SDDAParam::EOperator::sub: return raw - val;
        case SDDAParam::EOperator::mul: return raw * val;
        case SDDAParam::EOperator::div: return (val != 0) ? raw / val : raw;
        case SDDAParam::EOperator::dif: return (raw == val) ? 0u : raw;
        default: return raw;
    }
}
