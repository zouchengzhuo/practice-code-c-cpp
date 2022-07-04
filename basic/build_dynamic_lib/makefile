
# OUTDIR = ./lib
# CC     = g++
# LINK   = g++ -o
# CFLAG  = -c -O2 -o

run:
	g++ ./main.cpp -Ilib -Llib -ltest -o main && \
	sh ./run.sh && \
	rm ./main
	# -Idir 指定头文件路径
	# -l指定编译需要的库，编译阶段根据此项指定名查找、加载某个动态库，g++会给补齐 前缀 'lib' 和后缀 ‘.so’
	# -Ldir 指定链接库目录，如果不指定，在链接阶段只会从系统默认的目录查找 
	#  因为依赖动态库，代码是通过链接而非编译在可执行文件内，需要 export LD_LIBRARY_PATH=xxx
	#  运行时，还得修改环境变量中的库搜索路径后才可以运行编译好的可执行程序，不然运行时也找不到库
dylib:
	g++ lib/libtest.cpp -fPIC -shared -o lib/libtest.so