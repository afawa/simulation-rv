#ifndef instruction_hpp
#define instruction_hpp

#include "system_setting.hpp"
#include "register_pile_setting.hpp"
#include "memory_setting.hpp"
#include "utils.hpp"
#include <cstring>
#include <string>
#include <map>

//instruction type
#define R_TYPE   0
#define I_TYPE   1
#define S_TYPE   2
#define SB_TYPE  3
#define U_TYPE   4
#define UJ_TYPE  5
#define R4_TYPE  6
#define SCALL    7

class instruction {
public:
    ins inst;
    xcode opcode;           // inst[0-6]
    insType optype;
    byte tag;               // bit0 set if immediate is valid,
                            // bit1 set if func3 is valid,
                            // bit2 set if func7 is valid，
                            // bit3 set if rd is valid
                            // bit4 set if rs1 is valid
                            // bit5 set if rs2 is valid
                            // bit6 set if func2 is valid
                            // bit7 set if rs3 is valid
    imm immediate;
    xcode func2;            // inst[25-26]
    xcode func3;            // inst[12-14]
    xcode func7;            // inst[25-31]
    regID rd;               // inst[7-11]
    regID rs1;              // inst[15-19]
    regID rs2;              // inst[20-24]
    regID rs3;              // inst[27-31]
    
public:
    instruction();
    instruction(ins inst);
    bool getType(ins inst);         // if success return true, else return false
    bool setIMM(ins inst);          // (set immediate) Notice need switch, AND BE CAREFUL OF IMM BIT ORDER
    bool decode(ins inst);          // set rx, (func3), (func7), (and call getIMM)
    
    // These gets are designed to avoid using invalid parts of instruction
    imm getImm();
    xcode getfunc3();
    xcode getfunc7();
    xcode getfunc2();
    regID getrd();
    regID getrs1();
    regID getrs2();
    regID getrs3();
    
    void execute();
    void execute_R64();
    void execute_R();
    void execute_I();
    void execute_SX();
    void execute_UX();
    void execute_O();
    void execute_R4();
    void execute_FExt();
    void execute_SCALL();
    

    void print_ins(const char* inst_name, regID rd, regID rs1, regID rs2, regID rs3);
    void print_ins(const char* inst_name, regID rd, regID rs1, regID rs2);
    void print_ins(const char* inst_name, regID r1, regID r2, imm imm0);
    void print_ins(const char* inst_name, regID r1, regID r2);
    void print_ins(const char* inst_name, regID rx, imm imm0);
    void print_ins(const char* inst_name, regID rx);
    void print_ins(const char* inst_name);
};

extern bool verbose;
extern bool GDB_MODE;
extern bool IS_TO_EXIT;
extern std::map<std::string,int> COUNTS;
extern long int COUNT_INS;

extern bool instruction_execute;
extern instruction instruction_to_execute;
extern bool instruction_decode;
extern ins instruction_to_decode;
extern bool write_reg;
extern int write_reg_type; //0 PC 1 int 2 float 3 double
extern regID write_reg_id;
struct write_reg_value{
    memAddress address;
    reg64 int_value;
    f64 double_value;
    f32 float_value;
};
extern struct write_reg_value reg_value;
extern bool use_memory;
extern int load_save_type; //0 int_load 1 int_save 2 float_load 3 float_save
extern memAddress address;
extern int get_length; //使用内存长度
extern bool memory_signed;
extern regID memory_reg;
extern bool try_fetch;

#endif