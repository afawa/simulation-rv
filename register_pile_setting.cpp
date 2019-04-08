#include "register_pile_setting.hpp"

registers_pile sim_register_pile;

registers_pile::registers_pile(){
    PC=NULL;
    fcsr=0;
    for (int i=0;i<32;++i){
        rrx[i]=0;
        frx[i]=0;
    }
}

void registers_pile::readReg(){
    for(int i=0;i<32;++i){
        if(i==8||i==2||i==3||i==4){
            printf("rrx%d: 0x%lx\t",i,rrx[i]);//地址用16进制
        }
        else{
            printf("rrx%d: %lu\t",i,rrx[i]);//数用十进制
        }
        if((i+1)%8==0)
            printf("\n");
    }
    printf("PC: 0x%lx\n\n",(reg64)PC);
}

reg64 registers_pile::readReg(regID regDst){
    return rrx[regDst];
}

bool registers_pile::writeReg(regID regDst, reg64 value){
    if(regDst<0||regDst>31){
        printf("Invaild register!\n");
        return false;
    }
    if(regDst==0){
        //printf("write reg0\n");
        //printf("%lu\n",value);
        return true;
    }
    rrx[regDst]=value;
    return true;
}

reg64 registers_pile::getPC(){
    return (reg64)PC;
}

bool registers_pile::setPC(memAddress newPC){
    PC=(byte *) newPC;
    return true;
}

void registers_pile::nextPC(){
    PC+=4;
}

void registers_pile::readFloatReg(){
    for(int i = 0;i<32;i++){
        printf("frx%d: %lf\t",i ,frx[i]);
        if(i%8==7)
            printf("\n");
    }
    printf("FCSR: 0x%x\n\n",fcsr);
}

f64 registers_pile::readFloatRegD(regID regDst){
    return frx[regDst];
}

f32 registers_pile::readFloatRegS(regID regDst){
    return *((f32 *)(&(frx[regDst])));
}

bool registers_pile::writeFloatRegD(regID regDst, f64 value){
    if(regDst <0 || regDst > 31 )
        return false;
    if(regDst == 0){
        return true;
    }
    frx[regDst] = value;
    return true;
    
}
bool registers_pile::writeFloatRegS(regID regDst, f32 value){
    if(regDst <0 || regDst > 31 )
        return false;
    if(regDst == 0){
        return true;
    }
    f32 * value_f32_ptr = (f32 *)(&(frx[regDst]));
    * value_f32_ptr = value;
    return true;
    
}

reg32 registers_pile::getFCSR(){
    return fcsr;
}

bool registers_pile::setFCSR(reg32 newFCSR){
    fcsr = newFCSR;
    return true;
}
