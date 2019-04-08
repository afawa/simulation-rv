echo "################################"
echo "test add"
./riscv_sim ./test/add

sleep 3

echo "################################"
echo "test mul"
./riscv_sim ./test/mul-div

sleep 3

echo "################################"
echo "test float"
./riscv_sim ./test/double-floatги┐╔╤бгм╙╨╢ю═т╝╙╖╓гй

sleep 3

echo "################################"
echo "test n!"
./riscv_sim ./test/n!

sleep 3

echo "################################"
echo "test qsort"
./riscv_sim ./test/qsort

sleep 3

echo "################################"
echo "test function"
./riscv_sim ./test/simple-fuction

echo "done"
