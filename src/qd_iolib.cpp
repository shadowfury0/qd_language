#include "qd_iolib.h"

_QD_BEGIN

std::fstream os;

size_t open(D_State* l) {
    if (l->v_pos < 1 || l->vars.size() < 1) {
        return 1;
    }

    D_OBJ& var1 = l->vars.front();

    if ( VE_STR != var1.type ) {
        std::cout << "args is not correct \n";
        return 1;
    }

    //第二个参数
    // D_OBJ& var2 =  *(++ l->vars.begin());

    // if ( VE_STR != var2.type ) {

    // }

    std::string filename = var1.var.chv;

    os.open(filename,std::ios::in | std::ios::out | std::ios::ate );

    if (!os.is_open()) {
        std::cout << "file open error \n";
        return 1;
    }

    l->vars.pop_front();

    D_STA_PUS_NUL

    return 0;
}

size_t close(D_State* l) {
    if (os.is_open()) {
        os.close();
    }

    D_STA_PUS_NUL

    return 0;
}

size_t write(D_State* l) {
    size_t len = l->v_pos;
    while (len--)
    {
        D_OBJ& str = l->vars.front();
        switch (str.type)
        {
        case VE_BOOL:
            os << str.var.bv;
            break;
        case VE_INT:
            os << str.var.iv;
            break;
        case VE_FLT:
            os << str.var.dv;
            break;
        case VE_STR:
            os << str.var.chv;
            break;
        default:
            os << "";
            break;
        }
        l->vars.pop_front();
    }

    D_STA_PUS_NUL

    return 0;
}

// 按行读取
size_t read(D_State* l) {
    size_t len = l->v_pos;

    //read不需要参数
    while (len--)
    {
        l->vars.pop_front();
    }

    std::string str;
    std::getline(os,str);

    D_OBJ obj;
    obj.alloc_str(str.data(),str.size());

    l->rets.push_back(obj);

    return 0;
}

IO_LIB::IO_LIB() {

}

IO_LIB::~IO_LIB() {
    if (os.is_open()) {
        os.close();
    }
}

void IO_LIB::load_lib() {
    funs["open"]  = open;
    funs["close"] = close;
    funs["read"]  = read;
    funs["write"] = write;
}


_QD_END
