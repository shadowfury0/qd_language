#include "qd_bitset.h"

_QD_BEGIN

/*
    解释一下,
    int 8 ->  1
    int 9 ->  2
    int 17 ->  3

    该函数所需字节数
*/
#define NBYTES(nbits)	(((nbits) + BITSIZE - 1) / BITSIZE)

#define BIT2BYTE(ibit)	((ibit) / BITSIZE)
#define BIT2SHIFT(ibit)	((ibit) % BITSIZE)
#define BIT2MASK(ibit)	(1 << BIT2SHIFT(ibit))
#define BYTE2BIT(ibyte)	((ibyte) * BITSIZE)


BITSET::BITSET() {
    this->len = 0;
	this->bs = nullptr;
}

BITSET::BITSET(const BITSET& b) {
	this->len = 0;
	this->bs = nullptr;

	*this = b;
}

BITSET::~BITSET() {
	this->del_bitset();
}

int BITSET::new_bitset(int nbits)
{
	//如果有位图，创建一个新的位图
	if ( this->bs ) {
		del_bitset();
	}

	this->len = nbits;
	int nbytes = NBYTES(nbits);
	this->bs = (bitset)malloc(sizeof(char) * nbits);

    //空指针,无返回
	if ( this->bs == nullptr)
        return 1;
	
	// ss += nbytes;
	// while (--nbytes >= 0)
	// 	*--ss = 0;
    memset(this->bs,'\0',nbits);

	return 0;
}

void BITSET::del_bitset()
{
    if ( this->bs != nullptr ) {
		free(this->bs);
		this->bs = nullptr;
	}
	this->len = 0;
}

void BITSET::reset() {
	memset(this->bs,'\0',this->len);
}

int BITSET::bit_size() {
	return this->len;
}

int BITSET::add_bit(int ibit)
{
	int ibyte = BIT2BYTE(ibit);
	char mask = BIT2MASK(ibit);

	if ( ibit >=  this->len  )
		return 0;
	
	// if (this->bs[ibyte] & mask)
	// 	return 1; /* Bit already set */
		
	this->bs[ibyte] |= mask;
	return 1;
}

int BITSET::flip_bit(int ibit) {
	int ibyte = BIT2BYTE(ibit);
	char mask = BIT2MASK(ibit);

	if ( ibit >=  this->len  )
		return 0;

	this->bs[ibyte] ^= mask;
	
	return 0;
}

bool BITSET::test(int p) {
	return BIT_TEST(this->bs,p);
}


// void BITSET::merge(bitset bs, int nbits)
// {
// 	int i;
	
// 	for (i = NBYTES(nbits); --i >= 0; )
// 		*(this->bs++) |= *bs++;
// }

void BITSET::operator=(const BITSET& b) {
	this->new_bitset(b.len);

	char* p = b.bs;
	char* t = this->bs;

	int i;
	for (i = NBYTES(this->len); --i >= 0; )
	{
		*t ++ = *p ++;
	}
}

bool BITSET::operator==(const BITSET& b) 
{
	if ( b.len != this->len ) {
		return false;
	}

	char* p = b.bs;
	char* t = this->bs;

	int i;
	for (i = NBYTES(this->len); --i >= 0; )
	{
		if ( *p ++ != *t ++ )
			return false;	
	}

	return true;
}

bool BITSET::operator!=(const BITSET& b) 
{
	return *this == b;
}

std::ostream& operator<<(std::ostream& os, const BITSET& u)
{
	if ( u.len <= 0 ) {
		return os;
	}
	int size = NBYTES(u.len);
	
	int i = size - 1;
	int j;
    for (; i >= 0; --i) {
        char ch = u.bs[i];
        for (j = sizeof(bitset) - 1; j >= 0; --j) {
            os << ((ch >> j) & 1);
        }
        os << " ";
    }
    return os;
}


_QD_END

