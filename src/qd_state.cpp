#include "qd_state.h"

_QD_BEGIN

D_State::D_State() {
    this->pos = 0;
}

D_State::D_State(const D_State& s) {
    this->pos = s.pos;
    this->vars = s.vars;
}


D_State::~D_State() {

}



_QD_END
