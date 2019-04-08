#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include "instruction.hpp"

void instruction::execute_SCALL(){
    if(sim_register_pile.readReg(a7) == 93 && sim_register_pile.readReg(a1) == 0 && sim_register_pile.readReg(a2) == 0 && sim_register_pile.readReg(a3) == 0){
        // exit_program
        IS_TO_EXIT = true;
        if(verbose) print_ins("\nProg Exited!");
        else{
            if(GDB_MODE)    printf("> ");
            printf("\nProg Exited!\n");
        }
    }
    else if(sim_register_pile.readReg(a7) == 62 &&  sim_register_pile.readReg(a3) == 0){
        //lseek
        write_reg=true;
        write_reg_type=1;
        write_reg_id=a0;
        reg_value.int_value=lseek((int)sim_register_pile.readReg(a0), (off_t)sim_register_pile.readReg(a1), (int)sim_register_pile.readReg(a2));
    }
    else if(sim_register_pile.readReg(a7) == 63 && sim_register_pile.readReg(a3) == 0){
        // read
        write_reg=true;
        write_reg_type=1;
        int fd = (int)sim_register_pile.readReg(a0);
        void * buf = (void*)sim_register_pile.readReg(a1);
        size_t count = (size_t)sim_register_pile.readReg(a2);
        buf = sim_mem.get_memory_point((reg64)buf);
        count = read(fd, buf, count);
        write_reg_id=a0;
        reg_value.int_value=count;
        if(verbose) print_ins("READ");
    }
    else if(sim_register_pile.readReg(a7) == 64 && sim_register_pile.readReg(a3) == 0){
        // write
        write_reg=true;
        write_reg_type=1;
        int fd = (int)sim_register_pile.readReg(a0);
        void * buf = (void*)sim_register_pile.readReg(a1);
        size_t count = (size_t)sim_register_pile.readReg(a2);
        buf = sim_mem.get_memory_point((reg64)buf);
        count = write(fd, buf, count);
        write_reg_id=a0;
        reg_value.int_value=count;
        fflush(stdout);
        if(verbose) print_ins("WRITE");
    }
    else if(sim_register_pile.readReg(a7) == 169 &&  sim_register_pile.readReg(a1) == 0 && sim_register_pile.readReg(a2) == 0 && sim_register_pile.readReg(a3) == 0){
        // gettimeofday
        write_reg=true;
        write_reg_type=1;
        struct  timeval  *tv_p = (struct timeval *)sim_mem.get_memory_point(sim_register_pile.readReg(a0));
        write_reg_id=a0;
        reg_value.int_value=gettimeofday(tv_p,NULL);
        if(verbose) print_ins("GETTIMEOFDAY");
    }else if(sim_register_pile.readReg(a7) == 214 &&  sim_register_pile.readReg(a1) == 0 && sim_register_pile.readReg(a2) == 0 && sim_register_pile.readReg(a3) == 0){
        //sbrk
    }else if(sim_register_pile.readReg(a7) == 57 &&  sim_register_pile.readReg(a1) == 0 && sim_register_pile.readReg(a2) == 0 && sim_register_pile.readReg(a3) == 0){
        //close
    }else if(sim_register_pile.readReg(a7) == 80 && sim_register_pile.readReg(a2) == 0 && sim_register_pile.readReg(a3) == 0){
        //fstat
    }
    else
        printf("Undefined scall\n");
    return;
}


void instruction::execute(){
    if(optype == SCALL){
        COUNTS["ECALL"]++;
        // _exit, read, write, gettimeofday, sbrk, (fstat), (lseek), (close)
        execute_SCALL();
        return;
    }
    
    if(optype == R4_TYPE){
        /*------RV64F-I.-----*/
        execute_R4();
        return;
    }
    switch(opcode){
            /*---- RV32I-----*/
        case 0x23:      // b0100011,SB,SH,SW,SD
            execute_SX();
            break;
        case 0x33:      // b0110011,ADD,SUB,SLL,SLT,SLTU,XOR,SRL,SRA,OR,AND
            // MUL, MULH, MULHSH, MULHU, DIV, DIVU, REM, REMU
            execute_R();
            break;
        case 0x37:      // b0110111, LUI
        case 0x17:      // b0010111, AUIPC
            execute_O();//other
            break;
        case 0x1B:      // b0011011,ADDIW,SLLIW,SRLIW,SRAIW
        case 0x13:      // b0010011,SLLI,SRLI,SRAI,ADDI,SLTI,SLTIU,XORI,ORI,ANDI,SLLI,SRLI,SRAI
        case 0x03:      // b0000011,LB,LH,LW,LBU,LHU,LWU,LD
        case 0x67:      // b1100111,JALR
        case 0x6F:      // b1101111,JAL
            execute_I();
            break;
        case 0x3B:      // b0111011,ADDW,SUBW,SLLW,SRLW,SRAW
            // MULW,DIVW, DIVUW, REMW, REMUW
            execute_R64();
            break;
        case 0x63:      // b1100011,BEQ,BNE,BLT,BGE,BLTU,BGEU
            execute_UX();
            break;
            
            /*------RV64F-II.-----*/
        case 0x07:
        case 0x27:
        case 0x53:
            execute_FExt();
            break;
        default:;
    }
    
}

void instruction::execute_O(){
    switch(opcode){
        case 0x37://LUI
            write_reg=true;
            write_reg_type=1;
            write_reg_id=getrd();
            reg_value.int_value=immediate;
            if(verbose) print_ins("LUI", getrd(), immediate);
            COUNTS["LUI"]++;
            break;
        case 0x17://AUIPC
            write_reg=true;
            write_reg_type=1;
            write_reg_id=getrd();
            reg_value.int_value=(signed64)sim_register_pile.getPC()-4+immediate-4;
            if(verbose) print_ins("AUIPC", getrd(), immediate);
            COUNTS["AUIPC"]++;
            break;
        default:
            printf("undefined instruction in execute_O\n");
            
    }
}
//execute R type
void instruction::execute_R(){
    if(getfunc7() == 0){
        switch(getfunc3()){
            case 0x00: //b000 add
                write_reg=true;
                write_reg_type=1;
                write_reg_id=getrd();
                reg_value.int_value=(signed64)sim_register_pile.readReg(getrs1())+(signed64)sim_register_pile.readReg(getrs2());
                if(verbose) print_ins("ADD", getrd(), getrs1(), getrs2());
                COUNTS["ADD"]++;
                break;
            case 0x01: //b001 sll
                write_reg=true;
                write_reg_type=1;
                write_reg_id=getrd();
                reg_value.int_value=sim_register_pile.readReg(getrs1()) << ((sim_register_pile.readReg(getrs2())&0x3f));
                if(verbose)
                    print_ins("SLL", getrd(), getrs1(), getrs2());
                COUNTS["SLL"]++;
                break;
            case 0x02: //b010 slt
                write_reg=true;
                write_reg_type=1;
                write_reg_id=getrd();
                if((signed64)sim_register_pile.readReg(getrs1()) < (signed64)sim_register_pile.readReg(getrs2()))
                    reg_value.int_value=1;
                else 
                    reg_value.int_value=0;
                if(verbose) print_ins("SLT", getrd(), getrs1(), getrs2());
                COUNTS["SLT"]++;
                break;
            case 0x03: //b011 sltu
                write_reg=true;
                write_reg_type=1;
                write_reg_id=getrd();
                if(sim_register_pile.readReg(getrs1()) < sim_register_pile.readReg(getrs2()))
                    reg_value.int_value=1;
                else
                    reg_value.int_value=0;
                if(verbose) print_ins("SLTU", getrd(), getrs1(), getrs2());
                COUNTS["SLTU"]++;
                break;
            case 0x04: //b100 xor
                write_reg=true;
                write_reg_type=1;
                write_reg_id=getrd();
                reg_value.int_value=sim_register_pile.readReg(getrs1()) ^ sim_register_pile.readReg(getrs2());
                if(verbose) print_ins("XOR", getrd(), getrs1(), getrs2());
                COUNTS["XOR"]++;
                break;
            case 0x05: //b101 srl
                write_reg=true;
                write_reg_type=1;
                write_reg_id=getrd();
                reg_value.int_value=sim_register_pile.readReg(getrs1()) >> (sim_register_pile.readReg(getrs2())&0x3f);
                if(verbose) print_ins("SRL", getrd(), getrs1(), getrs2());
                COUNTS["SRL"]++;
                break;
            case 0x06: //b110 or
                write_reg=true;
                write_reg_type=1;
                write_reg_id=getrd();
                reg_value.int_value=sim_register_pile.readReg(getrs1()) | sim_register_pile.readReg(getrs2());
                if(verbose) print_ins("OR", getrd(), getrs1(), getrs2());
                COUNTS["OR"]++;
                break;
            case 0x07: //b111 and
                write_reg=true;
                write_reg_type=1;
                write_reg_id=getrd();
                reg_value.int_value=sim_register_pile.readReg(getrs1()) & sim_register_pile.readReg(getrs2());
                if(verbose) print_ins("AND", getrd(), getrs1(), getrs2());
                COUNTS["AND"]++;
                break;
            default:
                printf("undefined instruction with opcode=011 0011, func7=0\n");
                break;
        }
    }
    else if(getfunc7()==0x20){
        switch(getfunc3()){
            case 0x00: //b000 sub
                write_reg=true;
                write_reg_type=1;
                write_reg_id=getrd();
                reg_value.int_value=(signed64)sim_register_pile.readReg(getrs1()) - (signed64)sim_register_pile.readReg(getrs2());
                if(verbose) print_ins("SUB", getrd(), getrs1(), getrs2());
                COUNTS["SUB"]++;
                break;
            case 0x05: //b101 sra
                write_reg=true;
                write_reg_type=1;
                write_reg_id=getrd();
                reg_value.int_value=(long)(sim_register_pile.readReg(getrs1())) >> (sim_register_pile.readReg(getrs2())&0x3f);
                if(verbose) print_ins("SRA", getrd(), getrs1(), getrs2());
                COUNTS["SRA"]++;
                break;
            default:
                printf("undefined instruction with opcode=011 0011, func7=0x20\n");
                break;
        }
    }
    else if(getfunc7()==0x01){
        // MUL, MULH, MULHSH, MULHU, DIV, DIVU, REM, REMU
        reg64 reg1_l = 0;
        reg64 reg1_h = 0;
        reg64 reg2_l = 0;
        reg64 reg2_h = 0;
        reg64 mul_h = 0;
        reg64 mul_m = 0;
        reg64 mul_l = 0;
        signed32 neg = 0;
        reg64 reg1 = 0,reg2 = 0;
        switch(getfunc3()){
            case 0x00: //b000 mul
                write_reg=true;
                write_reg_type=1;
                write_reg_id=getrd();
                reg_value.int_value=sim_register_pile.readReg(getrs1())*sim_register_pile.readReg(getrs2());
                if(verbose) print_ins("MUL", getrd(), getrs1(), getrs2());
                COUNTS["MUL"]++;
                break;
            case 0x01: //b001 mulh ?
                write_reg=true;
                write_reg_type=1;
                write_reg_id=getrd();
                if((signed64)sim_register_pile.readReg(getrs1()) < 0){
                    reg1 = ~sim_register_pile.readReg(getrs1())+1;
                    neg++;
                }
                else
                    reg1 = sim_register_pile.readReg(getrs1());
                if((signed64)sim_register_pile.readReg(getrs2()) < 0){
                    reg2 = ~sim_register_pile.readReg(getrs2())+1;
                    neg++;
                }
                else
                    reg2 = sim_register_pile.readReg(getrs2());
                reg1_l = (reg32)reg1;
                reg1_h = reg1 >> 32;
                reg2_l = (reg32)reg2;
                reg2_h = reg2 >> 32;
                mul_h = reg1_h * reg2_h;
                mul_m = reg1_h * reg2_l + reg1_l * reg2_h;
                mul_l = reg1_l * reg2_l;
                if(neg == 0 || neg == 2)
                    reg_value.int_value=(reg64) (mul_h + (mul_m >> 32));
                else if(neg == 1){
                    if((reg64) (mul_l + (mul_m << 32)) == 0)
                        reg_value.int_value=(reg64) (~((mul_h + (mul_m >> 32))-1));
                    else
                        reg_value.int_value=(reg64)(~(mul_h + (mul_m >> 32)));
                }
                else;
                if(verbose) print_ins("MULH", getrd(), getrs1(), getrs2());
                COUNTS["MULH"]++;
                break;
            case 0x02: //b010 mulhsu
                write_reg=true;
                write_reg_type=1;
                write_reg_id=getrd();
                reg1_l = (reg32)sim_register_pile.readReg(getrs1());
                reg1_h = sim_register_pile.readReg(getrs1()) >> 32;
                reg2_l = (reg32)sim_register_pile.readReg(getrs2());
                reg2_h = sim_register_pile.readReg(getrs2()) >> 32;
                mul_h = reg1_h * reg2_h;
                mul_m = reg1_h * reg2_l + reg1_l * reg2_h;
                reg_value.int_value=(mul_h + (mul_m >> 32));
                if(verbose) print_ins("MULHSU", getrd(), getrs1(), getrs2());
                COUNTS["MULHSU"]++;
                break;
            case 0x03: //b011 mulhu
                write_reg=true;
                write_reg_type=1;
                write_reg_id=getrd();
                reg1_l = (reg32)sim_register_pile.readReg(getrs1());
                reg1_h = sim_register_pile.readReg(getrs1()) >> 32;
                reg2_l = (reg32)sim_register_pile.readReg(getrs2());
                reg2_h = sim_register_pile.readReg(getrs2()) >> 32;
                mul_h = reg1_h * reg2_h;
                mul_m = reg1_h * reg2_l + reg1_l * reg2_h;
                reg_value.int_value=(mul_h + (mul_m >> 32));
                if(verbose) print_ins("MULHU", getrd(), getrs1(), getrs2());
                COUNTS["MULHU"]++;
                break;
            case 0x04: //b100 div
                write_reg=true;
                write_reg_type=1;
                write_reg_id=getrd();
                reg_value.int_value=(signed64)sim_register_pile.readReg(getrs1()) / (signed64)sim_register_pile.readReg(getrs2());
                if(verbose) print_ins("DIV", getrd(), getrs1(), getrs2());
                COUNTS["DIV"]++;
                break;
            case 0x05: //b101 divu
                write_reg=true;
                write_reg_type=1;
                write_reg_id=getrd();
                reg_value.int_value= sim_register_pile.readReg(getrs1()) / (sim_register_pile.readReg(getrs2())&0x3f);
                if(verbose) print_ins("DIVU", getrd(), getrs1(), getrs2());
                COUNTS["DIVU"]++;
                break;
            case 0x06: //b110 rem
                write_reg=true;
                write_reg_type=1;
                write_reg_id=getrd();
                reg_value.int_value=(signed64)sim_register_pile.readReg(getrs1()) % (signed64)sim_register_pile.readReg(getrs2());
                if(verbose) print_ins("REM", getrd(), getrs1(), getrs2());
                COUNTS["REM"]++;
                break;
            case 0x07: //b111 remu
                write_reg=true;
                write_reg_type=1;
                write_reg_id=getrd();
                reg_value.int_value=sim_register_pile.readReg(getrs1()) % sim_register_pile.readReg(getrs2());
                if(verbose) print_ins("REMU", getrd(), getrs1(), getrs2());
                COUNTS["REMU"]++;
                break;
            default:
                printf("undefined instruction with opcode=011 0011, func7=1\n");
                break;
        }
    }
    else{
        printf("undefined instruction in R-type\n");
    }
}

// opcode = 3B
void instruction::execute_R64(){
    if(getfunc7()==0x00){
        switch(getfunc3()){
            case 0x00: //b000 addw
                write_reg=true;
                write_reg_type=1;
                write_reg_id=getrd();
                reg_value.int_value=(signed32)sim_register_pile.readReg(getrs1()) + (signed32)sim_register_pile.readReg(getrs2());
                if(verbose) print_ins("ADDW", getrd(), getrs1(), getrs2());
                COUNTS["ADDW"]++;
                break;
            case 0x01: //b001 sllw
                write_reg=true;
                write_reg_type=1;
                write_reg_id=getrd();
                reg_value.int_value=(signed32)sim_register_pile.readReg(getrs1()) << (sim_register_pile.readReg(getrs2())&0x3f);
                if(verbose) print_ins("SLLW", getrd(), getrs1(), getrs2());
                COUNTS["SLLW"]++;
                break;
            case 0x05: //b101 srlw
                write_reg=true;
                write_reg_type=1;
                write_reg_id=getrd();
                reg_value.int_value=(signed32)sim_register_pile.readReg(getrs1()) >> (sim_register_pile.readReg(getrs2())&0x3f);
                if(verbose) print_ins("SRLW", getrd(), getrs1(), getrs2());
                COUNTS["SRLW"]++;
                break;
            default:
                printf("undefined instruction with opcode =011 1011, func7=0\n");
                break;
        }
    }
    else if(getfunc7()==0x20){
        switch(getfunc3()){
            case 0x00: //b000 subw
                write_reg=true;
                write_reg_type=1;
                write_reg_id=getrd();
                reg_value.int_value=(signed32)sim_register_pile.readReg(getrs1()) - (signed32)sim_register_pile.readReg(getrs2());
                if(verbose) print_ins("SUBW", getrd(), getrs1(), getrs2());
                COUNTS["SUBW"]++;
                break;
            case 0x05: //b101 sraw
                write_reg=true;
                write_reg_type=1;
                write_reg_id=getrd();
                reg_value.int_value=(signed32)(sim_register_pile.readReg(getrs1())) >> (sim_register_pile.readReg(getrs2())&0x3f);
                if(verbose) print_ins("SRAW", getrd(), getrs1(), getrs2());
                COUNTS["SRAW"]++;
                break;
            default:
                printf("undefined instruction with opcode = 011 1011, func7=0x20\n");
                break;
        }
    }
    else if(getfunc7()==0x01){
        // MULW,DIVW, DIVUW, REMW, REMUW
        switch(getfunc3()){
            case 0x00: //b000 mulw
                write_reg=true;
                write_reg_type=1;
                write_reg_id=getrd();
                reg_value.int_value=(reg64)((signed32)sim_register_pile.readReg(getrs1()) * (signed32)sim_register_pile.readReg(getrs2()));
                if(verbose) print_ins("MULW", getrd(), getrs1(), getrs2());
                COUNTS["MULW"]++;
                break;
            case 0x04: //b100 divw
                write_reg=true;
                write_reg_type=1;
                write_reg_id=getrd();
                reg_value.int_value=(reg64)((signed32)sim_register_pile.readReg(getrs1()) / (signed32)sim_register_pile.readReg(getrs2()));
                if(verbose) print_ins("DIVW", getrd(), getrs1(), getrs2());
                COUNTS["DIVW"]++;
                break;
            case 0x05: //b101 divuw
                write_reg=true;
                write_reg_type=1;
                write_reg_id=getrd();
                reg_value.int_value=(reg64)((reg32)sim_register_pile.readReg(getrs1()) / (reg32)sim_register_pile.readReg(getrs2()));
                if(verbose) print_ins("DIVUW", getrd(), getrs1(), getrs2());
                COUNTS["DIVUW"]++;
                break;
            case 0x06: //b110 remw
                write_reg=true;
                write_reg_type=1;
                write_reg_id=getrd();
                reg_value.int_value=(reg64)((signed32)sim_register_pile.readReg(getrs1()) % (signed32)sim_register_pile.readReg(getrs2()));
                if(verbose) print_ins("REMW", getrd(), getrs1(), getrs2());
                COUNTS["REMW"]++;
                break;
            case 0x07: //b111 remuw
                write_reg=true;
                write_reg_type=1;
                write_reg_id=getrd();
                reg_value.int_value=(reg64)((reg32)sim_register_pile.readReg(getrs1()) % (reg32)sim_register_pile.readReg(getrs2()));
                if(verbose) print_ins("REMUW", getrd(), getrs1(), getrs2());
                COUNTS["REMUW"]++;
                break;
            default:
                printf("undefined instruction with opcode =011 1011, func7=1\n");
                break;
        }
    }
    else
        printf("undefined instruction in R64-TYPE\n");
}

void instruction::execute_I(){
    // contain opcode, func3, rd, rs1, imm
    switch (opcode) {
        case 0x6F: //JAL rd,imm
            write_reg=true;
            instruction_decode=false;
            try_fetch=false;
            write_reg_type=3;
            write_reg_id=getrd();
            reg32 newPC;
            reg_value.int_value=sim_register_pile.getPC()-4;
            newPC = (reg32)(sim_register_pile.getPC()-4+immediate-4);
            reg_value.address=newPC;
            if(verbose) print_ins("JAL", getrd(), immediate);
            COUNTS["JAL"]++;
            break;
        case 0x67: // b110 0111
            // JALR rd, rs1, imm
            switch (getfunc3()){
                case 0:
                    write_reg=true;
                    instruction_decode=false;
                    try_fetch=false;
                    write_reg_type=3;
                    write_reg_id=getrd();
                    reg_value.int_value=sim_register_pile.getPC()-4;
                    reg_value.address=(reg32)((sim_register_pile.readReg(getrs1())+immediate)&~1);
                    if(verbose) print_ins("JALR", getrd(), getrs1(), immediate);
                    COUNTS["JALR"]++;
                    break;
                default:
                    printf("Undefined instruction with opcode = 110 0111\n");
                    break;
            }
            break;
        case 0x03: // b000 0011
            // LB, LH, LW, LBU, LHU rd, rs1, imm
            // LWU, LD rd, rs1, imm
            //memAddress mem_addr;
            switch (getfunc3()) {
                case 0:             // LB rd, rs1, imm
                    use_memory=true;
                    load_save_type=0;
                    get_length=8;
                    memory_reg=getrd();
                    memory_signed=true;
                    address=(signed64)sim_register_pile.readReg(getrs1())+ immediate;
                    if(verbose) print_ins("LB", getrd(), getrs1(), immediate);
                    COUNTS["LB"]++;
                    break;
                case 1:             // LH rd, rs1, imm
                    use_memory=true;
                    load_save_type=0;
                    get_length=16;
                    memory_reg=getrd();
                    memory_signed=true;
                    address=(signed64)sim_register_pile.readReg(getrs1())+ immediate;
                    if(verbose) print_ins("LH", getrd(), getrs1(), immediate);
                    COUNTS["LH"]++;
                    break;
                case 2:             // LW rd, rs1, imm
                    use_memory=true;
                    load_save_type=0;
                    get_length=32;
                    memory_reg=getrd();
                    memory_signed=true;
                    address=(signed64)sim_register_pile.readReg(getrs1())+ immediate;
                    if(verbose) print_ins("LW", getrd(), getrs1(), immediate);
                    COUNTS["LW"]++;
                    break;
                case 4:             // LBU rd, rs1, imm
                    use_memory=true;
                    load_save_type=0;
                    get_length=8;
                    memory_reg=getrd();
                    memory_signed=false;
                    address=(signed64)sim_register_pile.readReg(getrs1())+ immediate;
                    if(verbose) print_ins("LBU", getrd(), getrs1(), immediate);
                    COUNTS["LBU"]++;
                    break;
                case 5:             // LHU rd, rs1, imm
                    use_memory=true;
                    load_save_type=0;
                    get_length=16;
                    memory_reg=getrd();
                    memory_signed=false;
                    address=(signed64)sim_register_pile.readReg(getrs1())+ immediate;
                    if(verbose) print_ins("LHU", getrd(), getrs1(), immediate);
                    COUNTS["LHU"]++;
                    break;
                case 6:             // LWU rd, rs1, imm
                    use_memory=true;
                    load_save_type=0;
                    get_length=32;
                    memory_reg=getrd();
                    memory_signed=false;
                    address=(signed64)sim_register_pile.readReg(getrs1())+ immediate;
                    if(verbose) print_ins("LWU", getrd(), getrs1(), immediate);
                    COUNTS["LWU"]++;
                    break;
                case 3:             // LD rd, rs1, imm
                    use_memory=true;
                    load_save_type=0;
                    get_length=64;
                    memory_reg=getrd();
                    address=(signed64)sim_register_pile.readReg(getrs1())+ immediate;
                    if(verbose) print_ins("LD", getrd(), getrs1(), immediate);
                    COUNTS["LD"]++;
                    break;
                default:
                    printf("Undefined instruction with opcode = 000 0011\n");
                    break;
            }
            break;
        case 0x13: // b001 0011
            // ADDI, SLTI, SLTIU, XORI, ORI, ANDI rd, rs1, imm
            // SLLI, SRLI, SRAI rd, rs1, shamt
            //signed64 ALUZ64;
            byte shamt;
            switch (getfunc3()) {
                case 0:             // ADDI rd, rs1, imm
                    write_reg=true;
                    write_reg_type=1;
                    write_reg_id=getrd();
                    reg_value.int_value=sim_register_pile.readReg(getrs1())+immediate;
                    if(verbose) print_ins("ADDI", getrd(), getrs1(), immediate);
                    COUNTS["ADDI"]++;
                    break;
                case 2:             // SLTI rd, rs1, imm
                    write_reg=true;
                    write_reg_type=1;
                    write_reg_id=getrd();
                    if((signed64)sim_register_pile.readReg(getrs1())<immediate)
                        reg_value.int_value=1;
                    else
                        reg_value.int_value=0;
                    if(verbose) print_ins("SLTI", getrd(), getrs1(), immediate);
                    COUNTS["SLTI"]++;
                    break;
                case 3:             // SLTIU rd, rs1, imm
                    write_reg=true;
                    write_reg_type=1;
                    write_reg_id=getrd();
                    if(sim_register_pile.readReg(getrs1())<(reg64)immediate)
                        reg_value.int_value=1;
                    else
                        reg_value.int_value=0;
                    if(verbose) print_ins("SLTIU", getrd(), getrs1(), immediate);
                    COUNTS["SLTIU"]++;
                    break;
                case 4:             // XORI rd, rs1, imm
                    write_reg=true;
                    write_reg_type=1;
                    write_reg_id=getrd();
                    reg_value.int_value=sim_register_pile.readReg(getrs1())^immediate;
                    if(verbose) print_ins("XORI", getrd(), getrs1(), immediate);
                    COUNTS["XORI"]++;
                    break;
                case 6:             // ORI rd, rs1, imm
                    write_reg=true;
                    write_reg_type=1;
                    write_reg_id=getrd();
                    reg_value.int_value=sim_register_pile.readReg(getrs1())|immediate;
                    if(verbose) print_ins("ORI", getrd(), getrs1(), immediate);
                    COUNTS["ORI"]++;
                    break;
                case 7:             // ANDI rd, rs1, imm
                    write_reg=true;
                    write_reg_type=1;
                    write_reg_id=getrd();
                    reg_value.int_value=sim_register_pile.readReg(getrs1())&immediate;
                    if(verbose) print_ins("ANDI", getrd(), getrs1(), immediate);
                    COUNTS["ANDI"]++;
                    break;
                case 1:             // SLLI rd, rs1, shamt
                    write_reg=true;
                    write_reg_type=1;
                    write_reg_id=getrd();
                    shamt = immediate & ONES(5, 0);
                    reg_value.int_value=sim_register_pile.readReg(getrs1()) << shamt;
                    if(verbose) print_ins("SLLI", getrd(), getrs1(), shamt);
                    COUNTS["SLLI"]++;
                    break;
                case 5:             // SRLI, SRAI rd, rs1, shamt
                    write_reg=true;
                    write_reg_type=1;
                    write_reg_id=getrd();
                    shamt = immediate & ONES(5, 0);
                    if(immediate & ~ONES(5,0)){ //SRAI
                        reg_value.int_value=((signed64)sim_register_pile.readReg(getrs1())) >> shamt;
                        COUNTS["SRAI"]++;
                    }
                    else{                   // SRLI
                        reg_value.int_value=sim_register_pile.readReg(getrs1()) >> shamt;
                        COUNTS["SRLI"]++;
                    }
                    if(verbose){
                        if(immediate & ~ONES(5,0)){
                            print_ins("SRAI", getrd(), getrs1(), shamt);
                        }else{
                            print_ins("SRLI", getrd(), getrs1(), shamt);
                        }
                    }
                    break;
                default:
                    printf("Undefined instruction with opcode = 001 0011\n");
                    break;
            }
            break;
        case 0x1B:  //b001 1011
            // ADDIW, SLLIW, SRLIW, SRAIW rd, rs1, shamt[longer]
            //signed32 ALUZ32;
            switch (getfunc3()) {
                case 0:             // ADDIW rd, rs1, imm
                    write_reg=true;
                    write_reg_type=1;
                    write_reg_id=getrd();
                    reg_value.int_value=(signed32)sim_register_pile.readReg(getrs1())+(signed32)immediate;
                    if(verbose) print_ins("ADDIW", getrd(), getrs1(), immediate);
                    COUNTS["ADDIW"]++;
                    break;
                case 1:             // SLLIW rd, rs1, shamt
                    write_reg=true;
                    write_reg_type=1;
                    write_reg_id=getrd();
                    shamt = immediate & ONES(4, 0);
                    reg_value.int_value=((signed32)sim_register_pile.readReg(getrs1())) << shamt;
                    if(verbose) print_ins("SLLIW", getrd(), getrs1(), shamt);
                    COUNTS["SLLIW"]++;
                    break;
                case 5:             // SRLIW, SRAIW rd, rs1, shamt
                    write_reg=true;
                    write_reg_type=1;
                    write_reg_id=getrd();
                    shamt = immediate & ONES(4, 0);
                    if(immediate & ~ONES(4,0)){ //SRAIW
                        reg_value.int_value=((signed32)sim_register_pile.readReg(getrs1()) >> shamt);
                        COUNTS["SRAIW"]++;
                    }
                    else{                       //SRLIW
                        reg_value.int_value=((reg32)sim_register_pile.readReg(getrs1())) >> shamt;
                        COUNTS["SRLIW"]++;
                    }
                    if(verbose){
                        if(immediate & ~ONES(4,0))
                            print_ins("SRAIW", getrd(), getrs1(), shamt);
                        else
                            print_ins("SRLIW", getrd(), getrs1(), shamt);
                    }
                    break;
                default:
                    printf("Undefined instruction with opcode = 001 1011\n");
                    break;
            }
            break;
        default:
            printf("Undefined instruction in excute_I\n");
    }
    
}
//SB,SH,SW,SD rs1,rs2,imm
void instruction::execute_SX(){
    switch(getfunc3()){
        case 0://SB rs1, rs2, imm
            use_memory=true;
            load_save_type=1;
            get_length=8;
            address=(signed64)sim_register_pile.readReg(getrs1())+ immediate;
            memory_reg=getrs2();
            if(verbose) print_ins("SB", getrs1(), getrs2(), immediate);
            COUNTS["SB"]++;
            break;
        case 1://SH
            use_memory=true;
            load_save_type=1;
            get_length=16;
            address=(signed64)sim_register_pile.readReg(getrs1())+ immediate;
            memory_reg=getrs2();
            if(verbose) print_ins("SH", getrs1(), getrs2(), immediate);
            COUNTS["SH"]++;
            break;
        case 2://SW
            use_memory=true;
            load_save_type=1;
            get_length=32;
            address=(signed64)sim_register_pile.readReg(getrs1())+ immediate;
            memory_reg=getrs2();
            if(verbose) print_ins("SW", getrs1(), getrs2(), immediate);
            COUNTS["SW"]++;
            break;
        case 3://SD rs1, rs2, imm
            use_memory=true;
            load_save_type=1;
            get_length=64;
            address=(signed64)sim_register_pile.readReg(getrs1())+ immediate;
            memory_reg=getrs2();
            if(verbose) print_ins("SD", getrs1(), getrs2(), immediate);
            COUNTS["SD"]++;
            break;
        default:
            printf("Undefined excute in excute_SX\n");
    }
    
}
//BEQ,BNE,BLT,BGE,BLTU,BGEU rs1,rs2,imm
void instruction::execute_UX(){
    switch(getfunc3()){
        case 0://BEQ
            if((signed64)sim_register_pile.readReg(getrs1())  == (signed64)sim_register_pile.readReg(getrs2())){
                write_reg=true;
                instruction_decode=false;
                try_fetch=false;
                write_reg_type=2;
                reg32 newPC;
                newPC = (reg32)(sim_register_pile.getPC()-4+immediate-4);//!!
                reg_value.address=newPC;
            }
            if(verbose) print_ins("BEQ", getrs1(), getrs2(), immediate);
            COUNTS["BEQ"]++;
            break;
        case 1://BNE
            if((signed64)sim_register_pile.readReg(getrs1())  != (signed64)sim_register_pile.readReg(getrs2())){
                write_reg=true;
                instruction_decode=false;
                try_fetch=false;
                write_reg_type=2;
                reg32 newPC;
                newPC = (reg32)(sim_register_pile.getPC()-4+immediate-4);//!!
                reg_value.address=newPC;
            }
            if(verbose) print_ins("BNE", getrs1(), getrs2(), immediate);
            COUNTS["BNE"]++;
            break;
        case 4://BLT
            if((signed64)sim_register_pile.readReg(getrs1())  < (signed64)sim_register_pile.readReg(getrs2())){
                write_reg=true;
                instruction_decode=false;
                try_fetch=false;
                write_reg_type=2;
                reg32 newPC;
                newPC = (reg32)(sim_register_pile.getPC()-4+immediate-4);//!!
                reg_value.address=newPC;
            }
            if(verbose) print_ins("BLT", getrs1(), getrs2(), immediate);
            COUNTS["BLT"]++;
            break;
        case 5://BGE
            if((signed64)sim_register_pile.readReg(getrs1())  >= (signed64)sim_register_pile.readReg(getrs2())){
                write_reg=true;
                instruction_decode=false;
                try_fetch=false;
                write_reg_type=2;
                reg32 newPC;
                newPC = (reg32)(sim_register_pile.getPC()-4+immediate-4);//!!
                reg_value.address=newPC;
            }
            if(verbose) print_ins("BGE", getrs1(), getrs2(), immediate);
            COUNTS["BGE"]++;
            break;
        case 6://BLTU
            if(sim_register_pile.readReg(getrs1())  < sim_register_pile.readReg(getrs2())){
                write_reg=true;
                instruction_decode=false;
                try_fetch=false;
                write_reg_type=2;
                reg32 newPC;
                newPC = (reg32)(sim_register_pile.getPC()-4+immediate-4);//!!
                reg_value.address=newPC;
            }
            if(verbose) print_ins("BLTU", getrs1(), getrs2(), immediate);
            COUNTS["BLTU"]++;
            break;
        case 7://BGEU
            if(sim_register_pile.readReg(getrs1())  >= sim_register_pile.readReg(getrs2())){
                write_reg=true;
                instruction_decode=false;
                try_fetch=false;
                write_reg_type=2;
                reg32 newPC;
                newPC = (reg32)(sim_register_pile.getPC()-4+immediate-4);//!!
                reg_value.address=newPC;
            }
            if(verbose) print_ins("BGEU", getrs1(), getrs2(), immediate);
            COUNTS["BGEU"]++;
            break;
        default:
            printf("Undefined excute in excute_SX\n");
    }
}



void instruction::execute_R4(){
    //FMADD.D rs1 * rs2 + rs3;
    if(getfunc2() == 1){
        write_reg=true;
        write_reg_type=8;
        f64 f64Z = sim_register_pile.readFloatRegD(getrs1())* sim_register_pile.readFloatRegD(getrs2())+ sim_register_pile.readFloatRegD(getrs3());
        write_reg_id=getrd();
        reg_value.double_value=f64Z;
        if(verbose) print_ins("FMADD.D", getrd(), getrs1(), getrs2(), getrs3());
        COUNTS["FMADD_D"]++;
    }
    else{
        printf("Warning: undefined instruction in R4 type\n");
    }
    
}


void instruction::execute_FExt(){
    if(opcode == 0x07){
        // FLW, FLD rd, rs1, imm, from mem to rd
        if(getfunc3()==2){//FLW
            use_memory=true;
            load_save_type=2;
            get_length=32;
            memory_reg=getrd();
            address=sim_register_pile.readReg(getrs1())+immediate;
            if(verbose) print_ins("FLW", getrd(), getrs1(), immediate);
            COUNTS["FLW"]++;
        }
        else if(getfunc3()==3){
            use_memory=true;
            load_save_type=2;
            get_length=64;
            memory_reg=getrd();
            address=sim_register_pile.readReg(getrs1())+immediate;
            if(verbose) print_ins("FLD", getrd(), getrs1(), immediate);
            COUNTS["FLD"]++;
        }
        else{
            printf("undefined instruction with opcode = 0x07\n");
        }
    }
    else if(opcode == 0x27){
        // FSW, FSD rs1, rs2, imm, from rs2 to mem
        if(getfunc3()==2){//FSW
            use_memory=true;
            load_save_type=3;
            get_length=32;
            address=sim_register_pile.readReg(getrs1())+immediate;
            memory_reg=getrs2();
            if(verbose) print_ins("FSW", getrs1(), getrs2(), immediate);
            COUNTS["FSW"]++;
        }
        else if(getfunc3()==3){//FSD
            use_memory=true;
            load_save_type=3;
            get_length=64;
            address=sim_register_pile.readReg(getrs1())+immediate;
            memory_reg=getrs2();
            if(verbose) print_ins("FSD", getrs1(), getrs2(), immediate);
            COUNTS["FSD"]++;
        }
        else{
            printf("undefined instruction with opcode = 0x27\n");
        }
    }
    else if(opcode == 0x53){
        // XXX rd, rs1, rs2
        switch (getfunc7()) {
            case 0:
                // FADD.S
                write_reg=true;
                write_reg_type=4;
                write_reg_id=getrd();
                reg_value.float_value=sim_register_pile.readFloatRegS(getrs1())+sim_register_pile.readFloatRegS(getrs2());
                if(verbose) print_ins("FADD.S", getrd(), getrs1(), getrs2());
                COUNTS["FADD_S"]++;
                break;
            case 1:
                // FADD.D
                write_reg=true;
                write_reg_type=8;
                write_reg_id=getrd();
                reg_value.double_value=sim_register_pile.readFloatRegD(getrs1())+sim_register_pile.readFloatRegD(getrs2());
                if(verbose) print_ins("FADD.D", getrd(), getrs1(), getrs2());
                COUNTS["FADD_D"]++;
                break;
            case 4:
                // FSUB.S
                write_reg=true;
                write_reg_type=4;
                write_reg_id=getrd();
                reg_value.float_value=sim_register_pile.readFloatRegS(getrs1())-sim_register_pile.readFloatRegS(getrs2());
                if(verbose) print_ins("FSUB.S", getrd(), getrs1(), getrs2());
                COUNTS["FSUB_S"]++;
                break;
            case 5:
                // FSUB.D
                write_reg=true;
                write_reg_type=8;
                write_reg_id=getrd();
                reg_value.double_value=sim_register_pile.readFloatRegD(getrs1())-sim_register_pile.readFloatRegD(getrs2());
                if(verbose) print_ins("FSUB.D", getrd(), getrs1(), getrs2());
                COUNTS["FSUB_D"]++;
                break;
            case 8:
                // FMUL.S
                write_reg=true;
                write_reg_type=4;
                write_reg_id=getrd();
                reg_value.float_value=sim_register_pile.readFloatRegS(getrs1())*sim_register_pile.readFloatRegS(getrs2());
                if(verbose) print_ins("FMUL.S", getrd(), getrs1(), getrs2());
                COUNTS["FMUL_S"]++;
                break;
            case 9:
                // FMUL.D
                write_reg=true;
                write_reg_type=8;
                write_reg_id=getrd();
                reg_value.double_value=sim_register_pile.readFloatRegD(getrs1())*sim_register_pile.readFloatRegD(getrs2());
                if(verbose) print_ins("FMUL.D", getrd(), getrs1(), getrs2());
                COUNTS["FMUL_D"]++;
                break;
            case 12:
                // FDIV.S
                write_reg=true;
                write_reg_type=4;
                write_reg_id=getrd();
                reg_value.float_value=sim_register_pile.readFloatRegS(getrs1())/sim_register_pile.readFloatRegS(getrs2());
                if(verbose) print_ins("FDIV.S", getrd(), getrs1(), getrs2());
                COUNTS["FDIV_S"]++;
                break;
            case 13:
                // FDIV.D
                write_reg=true;
                write_reg_type=8;
                write_reg_id=getrd();
                reg_value.double_value=sim_register_pile.readFloatRegD(getrs1())/sim_register_pile.readFloatRegD(getrs2());
                if(verbose) print_ins("FDIV.D", getrd(), getrs1(), getrs2());
                COUNTS["FDIV_D"]++;
                break;
            case 0x50:
                // FEQ.S, FLT.S, FLE.S
                if(getfunc3()==2){//FEQ
                    write_reg=true;
                    write_reg_type=1;
                    write_reg_id=getrd();
                    if(((sim_register_pile.readFloatRegS(getrs1())-sim_register_pile.readFloatRegS(getrs2()))<FDIFF) && ((sim_register_pile.readFloatRegS(getrs2())-sim_register_pile.readFloatRegS(getrs1())) < FDIFF))
                        reg_value.int_value=1;
                    else
                        reg_value.int_value=0;
                    if(verbose) print_ins("FEQ.S", getrd(), getrs1(), getrs2());
                    COUNTS["FEQ_S"]++;
                }
                else if(getfunc3()==1){//FLT
                    write_reg=true;
                    write_reg_type=1;
                    write_reg_id=getrd();
                    if(sim_register_pile.readFloatRegS(getrs1())<sim_register_pile.readFloatRegS(getrs2()))
                        reg_value.int_value=1;
                    else
                        reg_value.int_value=0;
                    if(verbose) print_ins("FLT.S", getrd(), getrs1(), getrs2());
                    COUNTS["FLT_S"]++;
                }
                else if(getfunc3()==0){//FLE
                    write_reg=true;
                    write_reg_type=1;
                    write_reg_id=getrd();
                    if((sim_register_pile.readFloatRegS(getrs1())-sim_register_pile.readFloatRegS(getrs2()))<=FDIFF)
                        reg_value.int_value=1;
                    else
                        reg_value.int_value=0;
                    if(verbose) print_ins("FLE.S", getrd(), getrs1(), getrs2());
                    COUNTS["FLE_S"]++;
                }
                else{
                    printf("undefined instruction with opcode=0x53, func7=0x50\n");
                }
                break;
            case 0x51:
                // FEQ.D, FLT.D, FLE.D
                if(getfunc3()==2){//FEQ
                    write_reg=true;
                    write_reg_type=1;
                    write_reg_id=getrd();
                    if(((sim_register_pile.readFloatRegD(getrs1())-sim_register_pile.readFloatRegD(getrs2()))<FDIFF)&&((sim_register_pile.readFloatRegD(getrs2())-sim_register_pile.readFloatRegD(getrs1()))<FDIFF))
                        reg_value.int_value=1;
                    else
                        reg_value.int_value=0;
                    if(verbose) print_ins("FEQ.D", getrd(), getrs1(), getrs2());
                    COUNTS["FEQ_D"]++;
                }
                else if(getfunc3()==1){//FLT
                    write_reg=true;
                    write_reg_type=1;
                    write_reg_id=getrd();
                    if(sim_register_pile.readFloatRegD(getrs1())<sim_register_pile.readFloatRegD(getrs2()))
                        reg_value.int_value=1;
                    else
                        reg_value.int_value=0;
                    if(verbose) print_ins("FLT.D", getrd(), getrs1(), getrs2());
                    COUNTS["FLT_D"]++;
                }
                else if(getfunc3()==0){//FLE
                    write_reg=true;
                    write_reg_type=1;
                    write_reg_id=getrd();
                    if((sim_register_pile.readFloatRegD(getrs1())<sim_register_pile.readFloatRegD(getrs2()))||(sim_register_pile.readFloatRegD(getrs1())-sim_register_pile.readFloatRegD(getrs2()))<FDIFF)
                        reg_value.int_value=1;
                    else
                        reg_value.int_value=0;
                    if(verbose) print_ins("FLE.D", getrd(), getrs1(), getrs2());
                    COUNTS["FLE_D"]++;
                }
                else{
                    printf("undefined instruction with opcode=0x53, func7=0x51\n");
                }
                break;
            /*--------------------------- F&D extension convert instruction begin ---------------------------*/
            case 0x11:
                //FSGNJ.D FSGNJN.D rd, rs1
                if(getfunc3() == 0){// FSGNJ.D
                    write_reg=true;
                    write_reg_type=8;
                    write_reg_id=getrd();
                    reg_value.double_value=sim_register_pile.readFloatRegD(getrs1());
                    if(verbose) print_ins("FSGNJ.D", getrd(), getrs1());
                    COUNTS["FSGNJ_D"]++;
                }
                else if(getfunc3() == 1){
                    write_reg=true;
                    write_reg_type=8;
                    write_reg_id=getrd();
                    reg_value.double_value=0.0-sim_register_pile.readFloatRegD(getrs1());
                    if(verbose) print_ins("FSGNJN.D", getrd(), getrs1());
                    COUNTS["FSGNJN_D"]++;
                }else{
                    printf("undefined instruction in FExt, func7 = 0x11\n");
                }                break;
            case 0x20:
                //FCVT.S.D rd,rs1 double ==> float
                if(getrs2() == 1){
                    write_reg=true;
                    write_reg_type=4;
                    write_reg_id=getrd();
                    reg_value.float_value=(f32)sim_register_pile.readFloatRegD(getrs1());
                    if(verbose) print_ins("FCVT.S.D", getrd(), getrs1());
                    COUNTS["FCVT_S_D"]++;
                }
                else{
                    printf("undefined instruction in FExt, func7 = 0x20\n");
                }
                break;
            case 0x21:
                //FCVT.D.S rd,rs1 float ==> double
                if(getrs2() == 0){
                    write_reg=true;
                    write_reg_type=8;
                    write_reg_id=getrd();
                    reg_value.double_value=(f64)sim_register_pile.readFloatRegS(getrs1());
                    if(verbose) print_ins("FCVT.D.S", getrd(), getrs1());
                    COUNTS["FCVT_D_S"]++;
                }
                else{
                    printf("undefined instruction in FExt, func7 = 0x21\n");
                }
                break;
            case 0x60:
                // 0        FCVT.W.S rd, rs1    -- float rs1 ==> signed32 rd
                // 1        FCVT.WU.S rd, rs1   -- float rs1 ==> reg32 rd     NO_USE
                // (10)2    FCVT.L.S rd, rs1    -- float rs1 ==> signed64 rd
                // (11)2    FCVT.LU.S rd, rs1   -- float rs1 ==> reg64 rd     NO_USE
                if(getrs2() == 0){//w.s
                    write_reg=true;
                    write_reg_type=1;
                    write_reg_id=getrd();
                    f32 value = sim_register_pile.readFloatRegS(getrs1());
                    reg_value.int_value=(signed32)value;
                    if(verbose) print_ins("FCVT.W.S", getrd(), getrs1());
                    COUNTS["FCVT_W_S"]++;
                }else if(getrs2() == 2){ //l.s
                    write_reg=true;
                    write_reg_type=1;
                    write_reg_id=getrd();
                    f32 value = sim_register_pile.readFloatRegS(getrs1());
                    reg_value.int_value=(signed64)value;
                    if(verbose)  print_ins("FCVT.L.S", getrd(), getrs1());
                    COUNTS["FCVT_L_S"]++;
                }else{
                    printf("undefined instruction in FExt, func7 = 0x60\n");
                }
                break;
            case 0x61:
                // 0        FCVT.W.D rd, rs1    -- double rs1 ==> signed32 rd
                // 1        FCVT.WU.D rd, rs1   -- double rs1 ==> reg32 rd     NO_USE
                // (10)2    FCVT.L.D rd, rs1    -- double rs1 ==> signed64 rd
                // (11)2    FCVT.LU.D rd, rs1   -- double rs1 ==> reg64 rd     NO_USE
                if(getrs2() == 0){//FCVT.W.D rd, rs1  double ==> signed32
                    write_reg=true;
                    write_reg_type=1;
                    write_reg_id=getrd();
                    f64 value = sim_register_pile.readFloatRegD(getrs1());
                    reg_value.int_value=(signed32)value;
                    if(verbose) print_ins("FCVT.W.D", getrd(), getrs1());
                    COUNTS["FCVT_W_D"]++;
                }else if(getrs2() == 2){//FCVT.L.D rd, rs1  double ==> signed64
                    write_reg=true;
                    write_reg_type=1;
                    write_reg_id=getrd();
                    f64 value = sim_register_pile.readFloatRegD(getrs1());
                    reg_value.int_value=(signed64)value;
                    if(verbose) print_ins("FCVT.L.D", getrd(), getrs1());
                    COUNTS["FCVT_L_D"]++;
                }else{
                    printf("undefined instruction in FExt, func7 = 0x61\n");
                }
                break;
            case 0x68:
                // 0        FCVT.S.W rd, rs1    -- signed32 rs1 ==> float rd
                // 1        FCVT.S.WU rd, rs1   -- reg32 rs1 ==> float rd       NO_USE
                // (10)2    FCVT.S.L rd, rs1    -- signed64 rs1 ==> float rd
                // (11)2    FCVT.S.LU rd, rs1   -- reg64 rs1 ==> float rd       NO_USE
                if(getrs2() == 0) //s.w
                {
                    write_reg=true;
                    write_reg_type=4;
                    write_reg_id=getrd();
                    reg_value.float_value=(f32)(signed32)sim_register_pile.readReg(getrs1());
                    if(verbose) print_ins("FCVT.S.W", getrd(), getrs1());
                    COUNTS["FCVT_S_W"]++;
                }
                else if(getrs2() == 2){ //s.l
                    write_reg=true;
                    write_reg_type=4;
                    write_reg_id=getrd();
                    reg_value.float_value=(f32)(signed64)sim_register_pile.readReg(getrs1());
                    if(verbose) print_ins("FCVT.S.L", getrd(), getrs1());
                    COUNTS["FCVT_S_L"]++;
                }
                else{
                    printf("undefined instruction in FExt, func7 = 0x68\n");
                }
                break;
            case 0x69:
                // 0        FCVT.D.W rd, rs1    -- signed32  ==> double
                // 1        FCVT.D.WU rd, rs1   -- reg32  ==> double
                // (10)2    FCVT.D.L rd, rs1    -- signed64 rs1 ==> double rd     NO_USE
                // (11)2    FCVT.D.LU rd, rs1   -- reg64 rs1 ==> double rd        NO_USE
                if(getrs2() == 0){//FCVT.D.W
                    write_reg=true;
                    write_reg_type=8;
                    write_reg_id=getrd();
                    reg_value.double_value=(f64)(signed64)sim_register_pile.readReg(getrs1());
                    if(verbose) print_ins("FCVT.D.W", getrd(), getrs1());
                    COUNTS["FCVT_D_W"]++;
                }
                else if(getrs2() == 1){//FCVT.D.WU
                    write_reg=true;
                    write_reg_type=8;
                    write_reg_id=getrd();
                    reg_value.double_value=(f64)(reg64)sim_register_pile.readReg(getrs1());
                    if(verbose) print_ins("FCVT.D.WU", getrd(), getrs1());
                    COUNTS["FCVT_D_WU"]++;
                    printf("unused FCVT.D.WU");
                }
                else{
                    printf("undefined instruction in FExt, func7 = 0x69\n");
                }
                break;                
            case 0x71: //round = 000
                //FMV.X.D  floating-point register rs1 -> IEEE 754-2008 integer register rd; rs1 single
                if(getrs2() == 0){
                    write_reg=true;
                    write_reg_type=1;
                    write_reg_id=getrd();
                    f64 f_value =  sim_register_pile.readFloatRegD(getrs1());      //read float point from float reg
                    f64* fptr = (f64*)malloc(sizeof(f64)*1);
                    *fptr = f_value;
                    reg64 value = *((reg64*)fptr);
                    reg_value.int_value=value;
                    COUNTS["FMV_X_D"]++;
                }
                else{
                    printf("undefined instruction in FExt\n");
                }
                break;
            case 0x79: //round = 000
                //FMV.D.X IEEE 754-2008 integer register rs1 -> the floating-point register rd.
                if(getrs2() == 0){
                    write_reg=true;
                    write_reg_type=8;
                    write_reg_id=getrd();
                    reg64 int_value = sim_register_pile.readReg(getrs1());
                    reg64* intp = (reg64 *)malloc(sizeof(reg64)*1);
                    *intp = int_value;
                    f64 value = *((f64*)intp);
                    reg_value.double_value=value;
                    COUNTS["FMV_D_X"]++;
                }
                else{
                    printf("undefined instruction in FExt\n");
                }
                break;
        }
    }
    else{
        printf("undefined instruction in FExt\n");
    }
}
