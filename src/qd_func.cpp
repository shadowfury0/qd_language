#include "qd_func.h"

_QD_BEGIN


FunHead::FunHead(){
    this->state = nullptr;
    this->state = new D_State();
}

FunHead::FunHead(const FunHead& head){
    this->state = new D_State(*head.state);

    this->args = head.args;
    this->codes = head.codes;

    for (size_t iter = 0 ; iter < head.lfuns.size() ; iter++) {
        FunHead* funstate = nullptr;
        funstate = new FunHead(*head.lfuns[iter]);
        this->lfuns.push_back(funstate);
    }
}

FunHead::~FunHead(){
    if ( this->state != nullptr ) {
        delete this->state;
        this->state = nullptr;
    }
}

void FunHead::clear() {
    if ( !lfuns.empty()) {
        //析构所有子函数的堆内存空间
        for ( std::vector<FunHead *>::iterator
        iter = lfuns.begin() ; iter != lfuns.end() ; iter++)
        {
            if (*iter != nullptr) {
                (*iter)->clear();
                delete *iter;
                *iter = nullptr;
            }
        }
    }
}

size_t FunHead::args_size(){
    return args.size();
}

size_t FunHead::state_var_size() {
    return this->state->vars.size();
}

size_t FunHead::state_var_pos() {
    return this->state->v_pos;
}

void FunHead::set_state_pos(const size_t& i) {
    this->state->v_pos = i;
}


void FunHead::state_push_var(const D_VAR& var) {
    this->state->vars.push_back(var);
}


_QD_END