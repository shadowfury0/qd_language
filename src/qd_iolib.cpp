#include "qd_iolib.h"
#include "qd_log.h"

_QD_BEGIN

Logger* logger = Logger::getInstance();

std::map<std::string,std::fstream*> os_file;
//当前文件
std::fstream* cur_file = nullptr;

std::fstream* find_os(const std::string& name) {
    if ( os_file.find(name) != os_file.end()) return os_file[name];
    return nullptr;
}

size_t open(D_State* l) {
    size_t len = l->v_pos;

    if ( len < 1 || l->vars.size() < 1 ) {
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
        goto read_end;
    }

    fs = new std::fstream();

    if (!fs) {
        logger->warn("file is not exist ");
        D_STA_PUS_NUL
        goto read_end;
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
        goto read_end;
    }

    //当前文件
    os_file[filename] =  cur_file =  fs;

    l->vars.pop_front();
    --len;
    l->rets.push_back(var1.var.chv);

read_end:
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
        os_file.erase(filename);
    }
    cur_file = nullptr;

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
    if (l->v_pos > l->vars.size() ) {
        return 1;
    }

    if (!cur_file) {
        logger->warn("cur file is null");
        D_STA_PUS_NUL
        return 0;
    }

    size_t len = l->v_pos;

    while (len--)
    {
        D_OBJ& str = l->vars.front();
        switch (str.type)
        {
        case VE_BOOL:
            *cur_file << str.var.bv;
            break;
        case VE_INT:
            *cur_file << str.var.iv;
            break;
        case VE_FLT:
            *cur_file << str.var.dv;
            break;
        case VE_STR:
            *cur_file << str.var.chv;
            break;
        default:
            *cur_file << "";
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
    if (!cur_file) {
        logger->error("file is not open");
        D_STA_PUS_NUL
        return 0;
    }
    std::getline(*cur_file,str);

    D_OBJ obj;
    obj.alloc_str(str.data(),str.size());

    l->rets.push_back(obj);


    return 0;
}

size_t _switch(D_State* l) {
    size_t len = l->v_pos;

    if ( len != 1 ) {
        logger->warn("arg is incorrect");
        D_STA_PUS_NUL
        while (len--)
        {
            l->vars.pop_front();
        }
        return 0;
    }
    else if ( VE_STR != l->vars.front().type) {
        logger->error("arg is not string");
        D_STA_PUS_NUL
        while (len--)
        {
            l->vars.pop_front();
        }
        return 0;
    }

    cur_file = find_os(l->vars.front().var.chv);
    
    if (!cur_file) {
        logger->warn("switch file is not exist");
    }

    l->vars.pop_front();

    D_STA_PUS_NUL
    return 0;
}

size_t eof(D_State* l) {
    size_t len = l->v_pos;

    while (len--)
    {
        l->vars.pop_front();
    }

    if (!cur_file) {
        logger->warn("file is not exist in is_end() function");
        D_STA_PUS_NUL
        return 0;
    }

    D_OBJ end;
    if (cur_file->eof()) {
        end = true;
    }
    else{
        end = false;
    }

    l->rets.push_back(end);

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

    funs["switch"]= _switch;

    //文件状态
    funs["eof"] = eof;
}


_QD_END
