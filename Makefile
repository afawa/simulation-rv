CC = g++
PROC = riscv_sim

SRC= main.cpp memory_setting.cpp register_pile_setting.cpp instruction_execute.cpp instruction_decode.cpp instruction_other.cpp utils.cpp


all: $(PROC)

riscv_sim: $(SRC)
	$(CC) -o $(PROC) $(SRC) -Wall


clean:
	-rm $(PROC)
