#ifndef system_hpp
#define system_hpp

#include <cstdio>
#include <cstdio>
#include <cstring>

#define RRXLEN 64
const float FDIFF = 0.00001;
const int HOW_MANY_INSTS = 120;


typedef unsigned long int   dword;
typedef unsigned int	 	word;
typedef unsigned short	 	halfword;
typedef unsigned char       byte;

typedef unsigned long int   bytes_8;
typedef unsigned int        bytes_4;
typedef unsigned short      bytes_2;
typedef unsigned char       bytes_1;

typedef unsigned long int	reg64;
typedef unsigned int	 	reg32;
typedef unsigned short int  reg16;
typedef unsigned char       reg8;
typedef unsigned char       regID;

typedef float	f32;
typedef double	f64;

typedef signed long int     signed64;
typedef signed int          signed32;
typedef signed short int    signed16;
typedef signed char         signed8;

typedef unsigned int        ins;
typedef signed long int     imm;
typedef unsigned char       xcode;
typedef unsigned char       insType;
typedef unsigned long int   memAddress;


#endif

