#ifndef __QD_BITSET_H__
#define __QD_BITSET_H__

#include "qd_header.h"

_QD_BEGIN

#define bitset char*
#define BITSIZE	(8*sizeof(char))


//判断bit位置是否设置值
#define BIT_TEST(ss, ibit) (((ss)[BIT2BYTE(ibit)] & BIT2MASK(ibit)) != 0)


struct BITSET
{
    BITSET();
    BITSET(const BITSET& b);
    ~BITSET();

    int new_bitset(int nbits);
    void del_bitset();
    void reset();

    //下标从0开始,潜在问题就是ibit超出范围
    //已经设置的不会重复设置,0正确 1 错误
    int add_bit(int ibit);
    int flip_bit(int ibit);
    int bit_size();
    bool test(int p);

    //nbits指的是前多少位
    // void merge(bitset bs, int nbits);
    void operator=(const BITSET&);

    bool operator==(const BITSET&);
    bool operator!=(const BITSET&);

    friend  std::ostream& operator<<(std::ostream& os, const BITSET& u);


    int len;   //位图长度
    bitset bs;
};


_QD_END


#endif

