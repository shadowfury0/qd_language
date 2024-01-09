#include "qd_iolib.h"
#include "qd_log.h"

_QD_BEGIN

std::map<std::string,std::fstream*> os_file;
Logger* logger = Logger::getInstance();

std::fstream* find_os(const std::string& name) {
    if ( os_file.find(name) != os_file.end()) return os_file[name];
    return nullptr;
}

size_t open(D_State* l) {
    size_t len = l->v_pos;

    if ( len < 1 || l->vars.size() < 1) {
        return 1;
    }

    D_OBJ var1 = l->vars.front();

    if ( VE_STR != var1.type ) {
        logger->error("args is not correct ");
        return 1;
    }

    std::string filename = var1.var.chv;
    
    std::fstream* fs = nullptr;
    fs = find_os(filename);
    if (fs) {
        logger->warn("file is opening");

        l->rets.push_back(var1.var.chv);
        return 0;
    }

    fs = new std::fstream();


    if (!fs) {
        logger->warn("file is not exist ");
        D_STA_PUS_NUL
        return 0;
    }

    //第二个参数
    // D_OBJ& var2 =  *(++ l->vars.begin());
    // if ( VE_STR != var2.type ) {
    // }

    //打开文件
    fs->open(filename,std::ios::in | std::ios::out );

    if (!fs->is_open()) {
        logger->error("file open error ");
        D_STA_PUS_NUL
        return 0;
    }

    os_file[filename] = fs;

    l->vars.pop_front();
    --len;
    l->rets.push_back(var1.var.chv);


    while (len--)
    {
        l->vars.pop_front();
    }


    return 0;
}

size_t close(D_State* l) {
    size_t len = l->v_pos;

    if (len < 1 || l->vars.size() < 1) {
        return 1;
    }

    D_OBJ& var1 = l->vars.front();

    if ( VE_STR != var1.type ) {
        logger->error("args is not correct ");
        return 1;
    }

    std::string filename = var1.var.chv;

    std::fstream* fs = find_os(filename);
    if (!fs) {
        logger->warn("file is not exist");
    }
    else if (!fs->is_open()) {
        logger->warn("file is not opening");
    }
    else {
        fs->close();
        logger->debug("file is closing");
    }

    l->vars.pop_front();
    --len;

    while (len--)
    {
        l->vars.pop_front();
    }


    D_STA_PUS_NUL

    return 0;
}

size_t write(D_State* l) {
    if (l->v_pos < 1 || l->vars.size() < 1) {
        return 1;
    }

    size_t len = l->v_pos;

    while (len--)
    {
        // D_OBJ& str = l->vars.front();
        // switch (str.type)
        // {
        // case VE_BOOL:
        //     os << str.var.bv;
        //     break;
        // case VE_INT:
        //     os << str.var.iv;
        //     break;
        // case VE_FLT:
        //     os << str.var.dv;
        //     break;
        // case VE_STR:
        //     os << str.var.chv;
        //     break;
        // default:
        //     os << "";
        //     break;
        // }
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
    // std::getline(os,str);

    D_OBJ obj;
    obj.alloc_str(str.data(),str.size());

    l->rets.push_back(obj);

    


    return 0;
}

IO_LIB::IO_LIB() {

}

IO_LIB::~IO_LIB() {
    //清空所有文件
    for (auto i : os_file) {
        delete i.second;
        i.second = nullptr;
    }
}

void IO_LIB::load_lib() {
    funs["open"]  = open;
    funs["close"] = close;
    funs["read"]  = read;
    funs["write"] = write;
}


_QD_END
