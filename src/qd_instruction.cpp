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
    // this->right = i.right;
    this->right = i.right;
}

Instruction::~Instruction(){

}


_QD_END