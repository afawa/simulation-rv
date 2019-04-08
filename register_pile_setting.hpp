#ifndef register_hpp
#define register_hpp

#ifndef stdio
#define stdio
#include <stdio.h>
#endif

#include "system_setting.hpp"


#define zero 0  //	rrx[0]    zero    Hard-wired zero
#define ra 1    //	rrx[1]    ra      Return address
#define sp 2    //	rrx[2]    sp      Stack pointer
#define gp 3    //	rrx[3]    gp      Global pointer
#define tp 4    //	rrx[4]    tp      Thread pointer
#define t0 5    //	rrx[5]    t0      Temporaries
#define t1 6    //	rrx[6]    t1      ..
#define t2 7    //	rrx[7]    t2      ..
#define fp 8    //	rrx[8]    fp      Frame pointer
#define s0 8    //  rrx[8]    s0      Saved registers
#define s1 9    //	rrx[9]    s1      Saved registers
#define a0 10   //	rrx[10]   a0	  Function arguments& return value
#define a1 11   //	rrx[11]   a1 	  Function arguments& return value
#define a2 12   //	rrx[12]   a2 	  Function arguments
#define a3 13   //	rrx[13]   a3 	  ..
#define a4 14   //	rrx[14]   a4      ..
#define a5 15   //	rrx[15]   a5      ..
#define a6 16   //	rrx[16]   a6	  ..
#define a7 17   //	rrx[17]   a7 	  ..
#define s2 18   //	rrx[18]   s2	  Saved registers
#define s3 19   //	rrx[19]   s3      ..
#define s4 20   //	rrx[20]   s4      ..
#define s5 21   //	rrx[21]   s5      ..
#define s6 22   //	rrx[22]   s6      ..
#define s7 23   //	rrx[23]   s7      ..
#define s8 24   //	rrx[24]   s8      ..
#define s9 25   //	rrx[25]   s9      ..
#define s10 26   //	rrx[26]   s10	  ..
#define s11 27   //	rrx[27]   s11	  ..
#define t3 28   //	rrx[28]   t3      Temporaries
#define t4 29   //	rrx[29]   t4      ..
#define t5 30   //	rrx[30]   t5      ..
#define t6 31   //	rrx[31]   t6      ..


class registers_pile {
public:
    byte* PC;
    reg32 fcsr;
    reg64 rrx[32];
    f64 frx[32];
    
public:
    registers_pile();
    void readReg();                                   
    reg64 readReg(regID regDst);                    
    bool writeReg(regID regDst, reg64 value);      
    reg64 getPC();                                  
    bool setPC(memAddress newPC);                        
    void nextPC();                                   

    void readFloatReg();                             
    f64 readFloatRegD(regID regDst);                 
    f32 readFloatRegS(regID regDst);
    bool writeFloatRegD(regID regDst, f64 value);      
    bool writeFloatRegS(regID regDst, f32 value);      
    reg32 getFCSR();                                    
    bool setFCSR(reg32 newFCSR);

};

extern registers_pile sim_register_pile;

#endif

