#include "qd_instruction.h"

_QD_BEGIN


Instruction::Instruction(){
    pretype = OC_NIL; 
    sufftype = OC_NIL;   
    type = OC_NULL;         
    ltype = OC_NULL;         
    curpos = FIN_END;
    lpos = FIN_END;
    rpos = FIN_END;
}

Instruction::Instruction(const Instruction& i){
    this->pretype = i.pretype;
    this->sufftype = i.sufftype;
    this->type = i.type;
    this->ltype = i.ltype;
    this->curpos = i.curpos;
    this->lpos = i.lpos;
    this->rpos = i.rpos;

    this->left = i.left;
    this->right = i.right;
    this->res = i.res;
}

Instruction::~Instruction(){

}


_QD_END