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
    this->anonymous = ca.anonymous;
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
}

CallInfo::Stack_V& CallInfo::v(const std::string name){
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
    while ( !this->cs.empty() ) {
        delete top();
        this->cs.pop_back();
    }
}

void CallStack::init(){
 
}

CallInfo* CallStack::top() {
    if ( this->cs.empty() ) {
        return nullptr;
    }
    return this->cs.back();
}



_QD_END
