#include "system_setting.hpp"
#include "register_pile_setting.hpp"
#include "memory_setting.hpp"
#include "instruction.hpp"
#include "utils.hpp"
#include <time.h>

bool GDB_MODE =false;
bool verbose = false;
bool IS_TO_EXIT = false;
long int COUNT_INS = 0;

//execute 阶段
bool instruction_execute=false;
instruction instruction_to_execute;

//decode 阶段
bool instruction_decode=false;
ins instruction_to_decode;

//write back 阶段
bool write_reg=false;
int write_reg_type; //0 PC 1 int 2 float 3 double
regID write_reg_id;
struct write_reg_value reg_value;

//memory 阶段
bool use_memory=false;
int load_save_type; //0 int_load 1 int_save 2 float_load 3 float_save
memAddress address;
int get_length; //使用内存长度
bool memory_signed;
regID memory_reg;

//fetch 阶段
//frtch 阶段总是可以执行，除非遇到跳转指令
bool try_fetch=true;


static int GDB_TYPE;
static bool IS_FIRST_GDB = false;
std::map<std::string,int> COUNTS;
static memAddress currentPC;

void help(){
    printf("This is a simulator to execute riscv ELF!\n");
    printf("    Usage: ./sim <filename> [--verbose|--debug]\n");
    printf("           ./sim --help\n");
}

ins fetch(){
    ins inst;
    currentPC = sim_register_pile.getPC();
    inst = sim_mem.get_memory_32(currentPC);
    sim_register_pile.nextPC();
    return inst;
}

int main(int argc, char * argv[]){
    const char * file_name;
    clock_t start,end;
    start=clock();
    if(argc < 2 || argc > 3 || strcmp(argv[1],"--help") == 0){
        help();
        return 0;
    }
    file_name = argv[1];
    if(argc == 3) {
        if (strcmp(argv[2], "--debug") == 0){
            printf("> (gdb mode)\n>\n");
            IS_FIRST_GDB = true;
            GDB_MODE=true;
        }
        else if (strcmp(argv[2], "--verbose") == 0)
            verbose = true;
        else{
            printf("\nINVALID COMMAND!\n");
            help();
            return 0;
        }
    }
    if(load_elf(file_name)==true){
        if(GDB_MODE)    printf("> ");
        printf("sp = %lx  PC = %lx\n", sim_register_pile.readReg(sp),sim_register_pile.getPC());
        if(GDB_MODE)    printf("> ");
        printf("EXECUTE %s\n",file_name);
        if(GDB_MODE)    printf("> ");
        printf("\n");
    }
    else{
        printf("LOAD ERROR!\n");
        return -1;
    }
    long cycle=0;
    print_ins_cnt_init();
    while(1){
        //gdb控制
        if(GDB_MODE){
            char cmd[20];
            if(IS_FIRST_GDB){
                printf(">\n");
                printf("> select a mode you want to run with:\n");
                printf("> continue: continue running\n");
                printf("> step: step mode\n");
                printf("> memory: print memory content\n");
                printf("> register: print register file info\n");
                printf("> quit: quit gdb mode\n>\n>\n");
            }
            if(IS_FIRST_GDB||GDB_TYPE==1){
                entry: IS_FIRST_GDB=false;
                printf(">");
                scanf("%s",cmd);        //read command
                fflush(stdin);          //clean stdin buffer
                if(strcmp(cmd,"continue")==0){
                    GDB_MODE=false;
                    verbose=false;
                }else if(strcmp(cmd,"step")==0){
                    GDB_TYPE=1;
                    verbose=true;
                }else if(strcmp(cmd,"memory")==0){
                    memAddress debug_mem=0;
                    reg32 mem_content=0;
                    printf("> set memory address\n");
                    printf("> ");
                    scanf("%lx",&debug_mem);
                    for(int row=0;row<4;++row){
                        printf("> ");
                        for(int col=0;col<4;++col){
                            mem_content=sim_mem.get_memory_32(debug_mem);
                            printf("0x%08x",mem_content);
                            printf("    ");
                            debug_mem+=4;
                        }
                        printf("\n");
                    }
                    goto entry;
                }else if(strcmp(cmd,"register")==0){
                    sim_register_pile.readReg();
                    sim_register_pile.readFloatReg();
                    goto entry;
                }else if(strcmp(cmd,"quit")==0){
                    printf("gdb quit\n");
                    return 0;
                }
            }
            
        }

        //主程序
        cycle=cycle+1;
        if(verbose)
            printf("############################################\n");
        //在execute阶段设置wirte back 和 memory阶段参数
        //write back 阶段
        if(write_reg){
            if(verbose)
                printf("writeback valid!\n");
            if(write_reg_type&2){
                //write PC
                if(verbose)
                    printf("set PC value:%ld\n",reg_value.address);
                sim_register_pile.setPC(reg_value.address);
                try_fetch=true;
            }
            if(write_reg_type&1){
                //write int
                if(verbose)
                    printf("write int reg:rrx%d value:%ld\n",write_reg_id,reg_value.int_value);
                sim_register_pile.writeReg(write_reg_id,reg_value.int_value);
            }
            if(write_reg_type&4){
                //write float
                if(verbose)
                    printf("write float reg:frx%d value:%f\n",write_reg_id,reg_value.float_value);
                sim_register_pile.writeFloatRegS(write_reg_id,reg_value.float_value);
            }
            if(write_reg_type&8){
                if(verbose)
                    printf("write double reg:frx%d value:%lf\n",write_reg_id,reg_value.double_value);
                sim_register_pile.writeFloatRegD(write_reg_id,reg_value.double_value);
            }
            write_reg=false;
        }else{
            if(verbose)
                printf("writeback invalid!\n");
        }
        if(verbose){
            printf("after writeback ....\n");
            sim_register_pile.readReg();
            sim_register_pile.readFloatReg();
        }
        if(verbose)
            printf("\n");

        //memory 阶段
        if(use_memory){
            if(verbose)
                printf("memory vaild!\n");
            if(load_save_type==0){
                //int_load
                if(verbose)
                    printf("load int ");
                if(get_length==8){
                    if(verbose)
                        printf("length:8 ");
                    if(memory_signed==true){
                        if(verbose)
                            printf("signed:true reg:rrx%d address:0x%lx\n",memory_reg,address);
                        sim_register_pile.writeReg(memory_reg,(signed8)sim_mem.get_memory_8(address));
                    }else{
                        if(verbose)
                            printf("signed:false reg:rrx%d address:0x%lx\n",memory_reg,address);
                        sim_register_pile.writeReg(memory_reg,sim_mem.get_memory_8(address));
                    }
                }else if(get_length==16){
                    if(verbose)
                        printf("length:16 ");
                    if(memory_signed){
                        if(verbose)
                            printf("signed:true reg:rrx%d address:0x%lx\n",memory_reg,address);
                        sim_register_pile.writeReg(memory_reg,(signed16)sim_mem.get_memory_16(address));
                    }else{ 
                        if(verbose)
                            printf("signed:false reg:rrx%d address:0x%lx\n",memory_reg,address);
                        sim_register_pile.writeReg(memory_reg,sim_mem.get_memory_16(address));
                    }
                }else if(get_length==32){
                    if(verbose)
                        printf("length:32 ");
                    if(memory_signed){
                        if(verbose)
                            printf("signed:true reg:rrx%d address:0x%lx\n",memory_reg,address);
                        sim_register_pile.writeReg(memory_reg,(signed32)sim_mem.get_memory_32(address));
                    }else{
                        if(verbose)
                            printf("signed:false reg:rrx%d address:0x%lx\n",memory_reg,address);
                        sim_register_pile.writeReg(memory_reg,sim_mem.get_memory_32(address));
                    }
                }else if(get_length==64){
                    if(verbose)
                        printf("length:64 signed:none reg:rrx%d address:0x%lx\n",memory_reg,address);
                    sim_register_pile.writeReg(memory_reg,(sim_mem.get_memory_64(address)));
                }
            }else if(load_save_type==1){
                //int_save
                if(verbose)
                    printf("save int ");
                if(get_length==8){
                    if(verbose)
                        printf("length:8 address:%lx reg:rrx%d\n",address,memory_reg);
                    sim_mem.set_memory_8(address,(reg8)sim_register_pile.readReg(memory_reg));
                }else if(get_length==16){
                    if(verbose)
                        printf("length:16 address:%lx reg:rrx%d\n",address,memory_reg);
                    sim_mem.set_memory_16(address,(reg16)sim_register_pile.readReg(memory_reg));
                }else if(get_length==32){
                    if(verbose)
                        printf("length:32 address:%lx reg:rrx%d\n",address,memory_reg);
                    sim_mem.set_memory_32(address,(reg32)sim_register_pile.readReg(memory_reg));
                }else if(get_length==64){
                    if(verbose)
                        printf("length:64 address:%lx reg:rrx%d\n",address,memory_reg);
                    sim_mem.set_memory_64(address,(reg64)sim_register_pile.readReg(memory_reg));
                }
            }else if(load_save_type==2){
                //float_load
                if(verbose)
                    printf("load float ");
                if(get_length==32){
                    if(verbose)
                        printf("length:32 reg:frx%d address:0x%lx\n",memory_reg,address);
                    sim_register_pile.writeFloatRegS(memory_reg,sim_mem.get_memory_f32(address));
                }else if(get_length==64){
                    if(verbose)
                        printf("length:64 reg:frx%d address:0x%lx\n",memory_reg,address);
                    sim_register_pile.writeFloatRegD(memory_reg,sim_mem.get_memory_f64(address));
                }else{
                    printf("invailid length to get float or double\n");
                }
            }else if(load_save_type==3){
                //float_save
                if(verbose)
                    printf("save float ");
                if(get_length==32){
                    if(verbose)
                        printf("length:32 address:%lx reg:frx%d\n",address,memory_reg);
                    sim_mem.set_memory_f32(address,sim_register_pile.readFloatRegS(memory_reg));
                }else if(get_length==64){
                    if(verbose)
                        printf("length:64 address:%lx reg:frx%d\n",address,memory_reg);
                    sim_mem.set_memory_f64(address,sim_register_pile.readFloatRegD(memory_reg));
                }else{
                    printf("invailid length to save float or double\n");
                }     
            }
            use_memory=false;
        }else{
            if(verbose)
                printf("memery invalid!\n");
        }
        if(verbose)
            printf("\n");
        //execute 阶段
        //先确定instruction 设置后两个阶段的参数
        
        if(instruction_execute){
            if(verbose)
                printf("execute:...\n");
            instruction_to_execute.execute();
            COUNT_INS++;
            if(IS_TO_EXIT)
                break;
            instruction_execute=false;
        }else{
            if(verbose)
                printf("no instruction execute!\n");
        }
        if(verbose)
            printf("\n");
        //decode 阶段，注意数据冒险，把decode完后的instruction类直接传递下去
        //如果发现调整指令把之前阶段全部停止
        if(instruction_decode){
            if(verbose)
                printf("decode starting\n");
            instruction instruction_decoded;
            if(instruction_decoded.decode(instruction_to_decode)){
                instruction_to_execute=instruction_decoded;
                instruction_execute=true;
                instruction_decode=false;
                if(verbose)
                    printf("decode done\n");
            }else{
                printf("DECODE ERROR!\n");
                printf("> currentPC = %lx\n", currentPC);
                printf("> instruction = %x\n", instruction_to_decode);
                printf("current register(current instruction not run yet)...\n");
                sim_register_pile.readReg();
                sim_register_pile.readFloatReg();
                printf("error!!!\n");
                return 0;
            }
        }else{
            if(verbose)
                printf("no instruction decode\n");
        }
        if(verbose)
            printf("\n");
        //fetch阶段
        if(try_fetch){
            if(verbose)
                printf("fetching\n");
            instruction_to_decode=fetch();
            instruction_decode=true;
            if(verbose)
                printf("instruction:%08x\n",instruction_to_decode);
        }else{
            if(verbose)
                printf("this time no fetching!\n");
        }
        if(verbose)
            printf("############################################\n\n\n");
    }
    end=clock();
    double duration =(double) (end-start)/((clock_t)1000);
    printf("duration: %lfs\n",duration);
    printf("total cycles: %ld\n",cycle);
    //print COUNTS
    print_ins_cnt();
   
    printf("congratulations!!!!\n");
    return 0;
}
