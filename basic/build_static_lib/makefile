# OUTDIR = ./lib
# CC     = g++
# LINK   = g++ -o
# CFLAG  = -c -O2 -o

run:
	g++ ./main.cpp -Ilib -Llib -ltest -o main && \
	./main && \
	rm ./main
	# -Idir 指定头文件路径
	# -l指定编译需要的库，编译阶段根据此项指定名查找、加载某个动态库，g++会给补齐 前缀 'lib' 和后缀 ‘.a’
	# -Ldir 指定链接库目录，如果不指定，在链接阶段只会从系统默认的目录查找 
	# 与动态库不同的时，静态库会被编译进可执行文件里边，不再依赖库加载路径LD_LIBRARY_PATH

staticlib:
	g++ -c ./lib/libtest.cpp -o ./lib/libtest.o
	g++ -c ./lib/libtest2.cpp -o ./lib/libtest2.o
	ar -cr ./lib/libtest.a ./lib/*.o