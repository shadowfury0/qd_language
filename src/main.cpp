#include "qd_main.h"
_USING_QD
using namespace std;


int main(int argc, char **argv){
    QDMAIN pmain;
    unsigned int ret; 
    ret = pmain.qd_main(argc,argv);
    
    // //错误判断 
    if (ret) std::cout << "qd languague error " << std::endl;

    return ret;
}


