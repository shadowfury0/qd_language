#include "qd_main.h"
_USING_QD
using namespace std;

//72


int main(int argc, char **argv){
    QDMAIN pmain;
    int ret = pmain.qd_main(argc,argv);

    //错误判断 
    // if (ret)
    return ret;
}


