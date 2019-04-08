#ifndef memory_hpp
#define memory_hpp

#include <stdio.h>
#include <stdlib.h>
#include "system_setting.hpp"

#define MEM_SIZE 1<<28
#define STACK_BOTTOM 0x6000000  
#define STACK_TOP    0xfffffff 


class memory {
public:
    byte* memory_begin;
    
public:
    memory();
    ~memory();
    byte* get_memory_point(memAddress offset);
    memAddress get_memory_offset(byte *my_mem_Paddr);
    bool  set_memory_8(memAddress addrOffset, reg8 value);
    bool  set_memory_16(memAddress addrOffset, reg16 value);
    bool  set_memory_32(memAddress addrOffset, reg32 value);
    bool  set_memory_64(memAddress addrOffset, reg64 value);
    reg64 get_memory_64(memAddress offset);
    reg32 get_memory_32(memAddress offset);
    reg16 get_memory_16(memAddress offset);
    reg8  get_memory_8(memAddress offset);
    
    f32   get_memory_f32(memAddress offset);
    f64   get_memory_f64(memAddress offset);
    bool  set_memory_f32(memAddress offset,f32 value);
    bool  set_memory_f64(memAddress offset,f64 value);
    
};

extern memory sim_mem;

#endif


