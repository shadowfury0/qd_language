#include "qd_instruction.h"

_QD_BEGIN

Instruction::Instruction(){
    lfin = false;              
    rfin = false;
    pretype = OC_NIL; 
    sufftype = OC_NIL;   
    type = OC_NULL;         //类型
    ltype = OC_NULL;         //类型
    curpos = -1;
    lpos = -1;
    rpos = -1;
}
Instruction::Instruction(const Instruction& i){
    this->lfin = i.lfin;
    this->rfin = i.rfin;
    this->pretype = i.pretype;
    this->sufftype = i.sufftype;
    this->type = i.type;
    this->ltype = i.ltype;
    this->curpos = i.curpos;
    this->lpos = i.lpos;
    this->rpos = i.rpos;

    this->left = i.left;
    this->right = i.right;
}

Instruction::~Instruction(){

}


_QD_END