#include "instruction.hpp"

//decode function

bool instruction::getType(ins inst){
    if(inst == 0x73){
        optype = SCALL;
        tag = 0;
        return true;
    }
    opcode = inst&OPCODE;
    switch(opcode){
        case 0x3B:      // b0111011
        case 0x33:      // b0110011
        case 0x53:      // b1010011 -- F extension
            optype =  R_TYPE;
            tag = 62;   // b111110 | 32+16+8+4+2
            return true;
            
        case 0x67:      // b1100111
        case 0x03:      // b0000011
        case 0x13:      // b0010011
        case 0x1B:
        case 0x07:      // -- F extension
            //case 0x73:      // b1110011
            optype =  I_TYPE;
            tag = 27;    // b011011 | 16+8+2+1
            return true;
            
        case 0x23:      // b0100011
        case 0x27:      // b0100111 -- F extension
            optype =  S_TYPE;
            tag = 51;    // b110011 | 32+16+2+1
            return true;
            
        case 0x63:      // b1100011
            optype =  SB_TYPE;
            tag = 51;    // b110011
            return true;
            
        case 0x37:      // b0110111
        case 0x17:      // b0010111
            optype =  U_TYPE;
            tag = 9;    // b001001 | 8+1
            return true;
            
        case 0x6F:      // b1101111
            optype =  UJ_TYPE;
            tag = 9;    // b001001
            return true;
        case 0x43:
        case 0x47:
        case 0x4B:
        case 0x4F:
            optype = R4_TYPE;
            tag = 250;     // b11111010
            return true;
        default:
            return false;
    }
}

bool instruction::setIMM(ins inst){
    switch(optype){
            /*R,R4,SCALL type
             no immediate*/
        case R_TYPE:
        case SCALL:
        case R4_TYPE:
            return true;
            /*I type
             31----------20 imm[0]~imm[11]
             shamt SRAI SRLI SLLI*/
        case I_TYPE:
            immediate = ((inst&ONES(31,20)) >> 20) | (IMM_SIGN(inst)*ONES(63, 11));
            return true;
            /*S type
             31-----25-------------11---7
             imm[11]~imm[5]        imm[4]~imm[0]*/
        case S_TYPE:
            immediate = ((inst&ONES(11,7)) >> 7) | ((inst&ONES(31,25)) >> 20) | (IMM_SIGN(inst)*ONES(63, 11));
            return true;
            /*SB type
             31 imm[12] 30----25 imm[10]~imm[5] 11----8 imm[4]~imm[1] 7 imm[11]*/
        case SB_TYPE:
            immediate = ((inst&ONES(11,8))>>7) | ((inst&ONES(30,25))>>20) | ((inst&ONES(7,7))<<4)| (IMM_SIGN(inst)*ONES(63, 12));
            return true;
            /*U type
             31----12 imm[31]~imm[12]*/
        case U_TYPE:
            immediate = (inst&ONES(31,12))| (IMM_SIGN(inst)*ONES(63, 31));
            return true;
            /*UJ type
             31 imm[20] 30----21 imm[10]~imm[1] 20 imm[11] 19----12 imm[19]~imm[12]*/
        case UJ_TYPE:
            immediate = ((inst&ONES(30,21))>>20) | ((inst&ONES(20,20))>>9) | (inst&ONES(19,12))| (IMM_SIGN(inst)*ONES(63, 20));
            return true;
        default:
            return false;
    }
}


//产生指令类型，将数据读入寄存器堆
bool instruction::decode(ins inst){
    if(getType(inst) != true || setIMM(inst) != true){
        printf("DECODE ERROR in FIRST STAGE! Oops!\n");
        return false;
    }
    switch(optype){
        case SCALL:
            return true;
            /* rs2,rs1,rd,func7,func3*/
        case R4_TYPE:
            func3 = ((inst&FUNCT3) >> 12);
            func2 = ((inst&FUNCT2) >> 25);
            rd = ((inst&RD) >> 7);
            rs1 = ((inst&RS1) >> 15);
            rs2 = ((inst&RS2) >> 20);
            rs3 = ((inst&RS3) >> 27);
            return true;
        case R_TYPE:
            func3 = ((inst&FUNCT3) >> 12);
            func7 = ((inst&FUNCT7) >> 25);
            rd = ((inst&RD) >> 7);
            rs1 = ((inst&RS1) >> 15);
            rs2 = ((inst&RS2) >> 20);
            return true;
            /*rs1,func3,rd*/
        case I_TYPE:
            func3 = ((inst&FUNCT3) >> 12);
            rd = ((inst&RD) >> 7);
            rs1 = ((inst&RS1) >> 15);
            return true;
            /*rs2,rs1,func3*/
        case S_TYPE:
            func3 = ((inst&FUNCT3) >> 12);
            rs1 = ((inst&RS1) >> 15);
            rs2 = ((inst&RS2) >> 20);
            return true;
            /*rs2,rs1,func3*/
        case SB_TYPE:
            func3 = ((inst&FUNCT3) >> 12);
            rs1 = ((inst&RS1) >> 15);
            rs2 = ((inst&RS2) >> 20);
            return true;
            /*rd*/
        case U_TYPE:
            rd = ((inst&RD) >> 7);
            return true;
            /*rd*/
        case UJ_TYPE:
            rd = ((inst&RD) >> 7);
            return true;
        default:
            return false;
    }
}
