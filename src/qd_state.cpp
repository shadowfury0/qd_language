#include "qd_state.h"

_QD_BEGIN

D_State::D_State() {
    this->v_pos = 0;
}

D_State::D_State(const D_State& s) {
    this->v_pos = s.v_pos;
    this->vars = s.vars;
}


D_State::~D_State() {

}



_QD_END
