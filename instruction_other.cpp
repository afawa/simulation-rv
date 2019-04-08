#include "instruction.hpp"

//generate instruction

instruction::instruction(){
    opcode=0;
    optype=0;
    tag=0;
    immediate=0;
    func2=0;
    func3=0;
    func7=0;
    rd=0;
    rs1=0;
    rs2=0;
    rs3=0;
}

instruction::instruction(ins inst){
    this->inst=inst;
    opcode=0;
    optype=0;
    tag=0;
    immediate=0;
    func2=0;
    func3=0;
    func7=0;
    rd=0;
    rs1=0;
    rs2=0;
    rs3=0;
}

//some printf functions

void instruction::print_ins(const char* inst_name, regID rd, regID rs1, regID rs2, regID rs3){
    // FMADD.D
    printf("instruction:\t %s $%d, $%d, $%d, $%d\n", inst_name, rd, rs1, rs2, rs3);
    sim_register_pile.readReg();
    sim_register_pile.readFloatReg();
}

void instruction::print_ins(const char* inst_name, regID rd, regID rs1, regID rs2){
    // ADD, SLL, SLT, SLTU, XOR, SRL, OR, AND, SUB, SRA, MUL, MULH, MULHSH, MULHU, DIV, 
    //DIVU, REM, REMU, ADDW, SLLW, SRLW, SUBW, SRAW, MULW,DIVW, DIVUW, REMW, REMUW
    // FADD.S FADD.D FSUB.S FSUB.D FMUL.S FMUL.D FDIV.S FDIV.D FEQ.S, FLT.S, FLE.S FEQ.D, FLT.D, FLE.D
    printf("instruction:\t %s $%d, $%d, $%d\n", inst_name, rd, rs1, rs2);
    sim_register_pile.readReg();
    sim_register_pile.readFloatReg();
    
}
void instruction::print_ins(const char* inst_name, regID r1, regID r2, imm imm0){
    // JALR, LB, LH, LW, LBU, LHU, LWU, LD, ADDI, SLTI, SLTIU, XORI, ORI, 
    // ANDI, SLLI, SRLI, SRAI, ADDIW, SLLIW, SRLIW, SRAIW, SB,SH,SW,SD, BEQ,BNE,BLT,BGE,BLTU,BGEU
    // FLW, FLD, FSW, FSD
    printf("instruction:\t %s $%d, $%d, %ld\n", inst_name, r1, r2, imm0);
    sim_register_pile.readReg();
    sim_register_pile.readFloatReg();
}
void instruction::print_ins(const char* inst_name, regID r1, regID r2){
    //FSGNJ.D FSGNJN.D FCVT.S.D FCVT.D.S FCVT.W.S FCVT.WU.S FCVT.W.D 
    //FCVT.WU.D FCVT.S.W FCVT.S.WU FCVT.D.W FCVT.D.WU FMV.X.D FMV.D.X
    printf("instruction:\t %s $%d, $%d\n", inst_name, r1, r2);
    sim_register_pile.readReg();
    sim_register_pile.readFloatReg();
}

void instruction::print_ins(const char* inst_name, regID rx, imm imm0){
    // LUI, AUIPC, JAL
    printf("instruction:\t %s $%d, 0x%lx\n", inst_name, rx, imm0);
    sim_register_pile.readReg();
    sim_register_pile.readFloatReg();
}
void instruction::print_ins(const char* inst_name, regID rx){
    printf("instruction:\t %s $%d\n", inst_name, rx);
    sim_register_pile.readReg();
    sim_register_pile.readFloatReg();
}
void instruction::print_ins(const char* inst_name){
    // READ ERITE GETTIMEOFDAY
    printf("instruction:\t %s\n", inst_name);
    sim_register_pile.readReg();
    sim_register_pile.readFloatReg();
}

imm instruction:: getImm(){
    if(!(tag&1))
        printf("Warning: Invalid immediate in instruction is used!\n");
    return immediate;
}
xcode instruction:: getfunc2(){
    if(!(tag&64))
        printf("Warning: Invalid func2 in instruction is used!\n");
    return func2;
}

xcode instruction:: getfunc3(){
    if(!(tag&2))
        printf("Warning: Invalid func3 in instruction is used!\n");
    return func3;
}
xcode instruction:: getfunc7(){
    if(!(tag&4))
        printf("Warning: Invalid func7 in instruction is used!\n");
    return func7;
}
regID instruction:: getrd(){
    if(!(tag&8))
        printf("Warning: Invalid rd in instruction is used!\n");
    return rd;
}

regID instruction:: getrs1(){
    if(!(tag&16))
        printf("Warning: Invalid rs1 in instruction is used!\n");
    return rs1;
}

regID instruction:: getrs2(){
    if(!(tag&32))
        printf("Warning: Invalid rs2 in instruction is used!\n");
    return rs2;
}
regID instruction:: getrs3(){
    if(!(tag&128))
        printf("Warning: Invalid rs3 in instruction is used!\n");
    return rs3;
}
