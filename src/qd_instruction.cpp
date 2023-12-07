#include "qd_instruction.h"

_QD_BEGIN


Instruction::Instruction(){
    restype = OC_NIL;
    type = OC_NULL;         
    curpos = FIN_END;
    lpos = FIN_END;
    rpos = FIN_END;
}

Instruction::Instruction(const Instruction& i){
    *this = i;
}

void Instruction::operator=(const Instruction& i){
    this->restype = i.restype;
    this->type = i.type;
    this->curpos = i.curpos;
    this->lpos = i.lpos;
    this->rpos = i.rpos;

    this->left = i.left;
    this->right = i.right;
}

Instruction::~Instruction(){

}

std::ostream& operator<<(std::ostream& os, const Instruction& p){
    os << " Instruction :  " 
    << " | restype : " << (int)p.restype 
    << " | type : " << (int)p.type 
    << " | curpos : " << p.curpos 
    << " | lpos : " << p.lpos 
    << " | rpos : " << p.rpos 
    << " | left value is " << p.left 
    << " | right value is " << p.right ;
    return os;
}


_QD_END