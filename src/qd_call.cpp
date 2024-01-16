#include "qd_call.h"

_QD_BEGIN


CallInfo::CallInfo() {
    init();
}

CallInfo::CallInfo(const CallInfo& ca) {
    init();
    
    this->f = ca.f;
    this->sv = ca.sv;
    this->pos = ca.pos;
    this->loop = ca.loop;
    this->anonymous = ca.anonymous;
    this->ifstate = ca.ifstate;
}

CallInfo::~CallInfo() {
    if ( f != nullptr ) {
        f->clear();
        delete f;
        f = nullptr;
    }
}

void CallInfo::init() {
    this->f = nullptr;
    this->pos = 0;
    this->anonymous = false;
    this->ifstate = false;
    this->loop = false;
}

D_OBJ& CallInfo::v(const std::string name){
    return this->sv[name];
}


CallStack::CallStack() {
    init();
}

CallStack::CallStack(const CallStack& cal) {
    init();
}

CallStack::~CallStack() {
    //清空栈
    clear();
}

void CallStack::init(){

}

void CallStack::clear() {
    while ( !this->cs.empty() ) {
        delete top();
        this->cs.pop_back();
    }
}

CallInfo* CallStack::top() {
    if ( this->cs.empty() ) {
        return nullptr;
    }
    return this->cs.back();
}



_QD_END
