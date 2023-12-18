#include "qd_lib.h"

_QD_BEGIN


BASE_LIB::BASE_LIB() {
    this->g = nullptr;
}

BASE_LIB::BASE_LIB(const BASE_LIB& d) {
    this->g = d.g;
}


BASE_LIB::~BASE_LIB() {
    this->g = nullptr;
}


_QD_END
