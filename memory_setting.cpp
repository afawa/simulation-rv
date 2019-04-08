#include "memory_setting.hpp"

memory sim_mem;

memory::memory(){
    memory_begin=(byte *)malloc(sizeof(byte)*MEM_SIZE);
    memset(memory_begin,0,MEM_SIZE);
}

memory::~memory(){
    free(memory_begin);
}

byte* memory::get_memory_point(memAddress offset){
    return memory_begin+offset;
}

memAddress memory::get_memory_offset(byte *now_point){
    return (now_point-memory_begin);
}

bool memory::set_memory_8(memAddress addrOffset,reg8 value){
    byte *point=get_memory_point(addrOffset);
    if (point==NULL){
        printf("error address\n");
        return false;
    }
    reg8 *reg8_point=(reg8*)point;
    *reg8_point=value;
    return true;
}

bool memory::set_memory_16(memAddress addrOffset,reg16 value){
    byte *point=get_memory_point(addrOffset);
    if (point==NULL){
        printf("error address\n");
        return false;
    }
    reg16 *reg16_point=(reg16*)point;
    *reg16_point=value;
    return true;
}

bool memory::set_memory_32(memAddress addrOffset,reg32 value){
    byte *point=get_memory_point(addrOffset);
    if (point==NULL){
        printf("error address\n");
        return false;
    }
    reg32 *reg32_point=(reg32*)point;
    *reg32_point=value;
    return true;
}

bool memory::set_memory_64(memAddress addrOffset,reg64 value){
    byte *point=get_memory_point(addrOffset);
    if (point==NULL){
        printf("error address\n");
        return false;
    }
    reg64 *reg64_point=(reg64*)point;
    *reg64_point=value;
    return true;
}

reg8 memory::get_memory_8(memAddress offset){
    byte* pointer=get_memory_point(offset);
    if(pointer==NULL){
        printf("error address\n");
        return -1;
    }
    reg8* reg8_point=(reg8*)pointer;
    return (*reg8_point);
}

reg16 memory::get_memory_16(memAddress offset){
    byte* pointer=get_memory_point(offset);
    if(pointer==NULL){
        printf("error address\n");
        return -1;
    }
    reg16* reg16_point=(reg16*)pointer;
    return (*reg16_point);
}

reg32 memory::get_memory_32(memAddress offset){
    byte* pointer=get_memory_point(offset);
    if(pointer==NULL){
        printf("error address\n");
        return -1;
    }
    reg32* reg32_point=(reg32*)pointer;
    return (*reg32_point);
}

reg64 memory::get_memory_64(memAddress offset){
    byte* pointer=get_memory_point(offset);
    if(pointer==NULL){
        printf("error address\n");
        return -1;
    }
    reg64* reg64_point=(reg64*)pointer;
    return (*reg64_point);
}

f32 memory::get_memory_f32(memAddress offset){
    byte* pointer=get_memory_point(offset);
    if(pointer==NULL){
        printf("error address\n");
        return -1;
    }
    f32* f32_point=(f32*)pointer;
    return (*f32_point);
}

f64 memory::get_memory_f64(memAddress offset){
    byte* pointer=get_memory_point(offset);
    if(pointer==NULL){
        printf("error address\n");
        return -1;
    }
    f64* f64_point=(f64*)pointer;
    return (*f64_point);
}

bool memory::set_memory_f32(memAddress offset,f32 value){
    byte* pointer=get_memory_point(offset);
    if(pointer==NULL){
        printf("error address\n");
        return false;
    }
    f32* f32_point=(f32*) pointer;
    *f32_point=value;
    return true;
}

bool memory::set_memory_f64(memAddress offset,f64 value){
    byte* pointer=get_memory_point(offset);
    if(pointer==NULL){
        printf("error address\n");
        return false;
    }
    f64* f64_point=(f64*) pointer;
    *f64_point=value;
    return true;
}
