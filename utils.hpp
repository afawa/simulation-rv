#ifndef utils_hpp
#define utils_hpp


#include "system_setting.hpp"
#include "memory_setting.hpp"
#include "register_pile_setting.hpp"
#include "instruction.hpp"
#include <iostream>
#include <elf.h>
#include <iomanip>
#include <sys/stat.h>

using namespace std;


bool load_elf(const char * file_name);
void print_ins_cnt_init();
void print_ins_cnt();

//using mask when decode
#define ONES(x,y)       (reg64) ((((unsigned long)1<<x)-1)+((unsigned long)1<<x) -(((unsigned long)1<<y)-1))
#define OPCODE          ONES(6,0)      // 7
#define FUNCT2          ONES(26,25)    // 2
#define FUNCT3          ONES(14,12)    // 3
#define FUNCT7          ONES(31,25)    // 7
#define RD              ONES(11,7)     // 5
#define RS1             ONES(19,15)    // 5
#define RS2             ONES(24,20)    // 5
#define RS3             ONES(31,27)    // 5
#define SHAMT           ONES(25,20)    // 6, RV64I
#define IMM_SIGN(inst)  ((inst>>31)&1) // sign bit of immediate





#endif