#ifndef class_template_h
#define class_template_h
#include <iostream>
#include <vector>


namespace cls{
    //模板类， 模板形参可以用在类里边的各种位置，还可以定义
    template<typename T, typename U>
    class Test{
        public:
            T t;
            U u;
            int compare(const T &t, const U &u){return 0;};
            //类内部还可以定义别的模板函数
            template<typename K>
            const K& tmpInMember(const K &k){
                return k;
            };
    };

    template<typename T>
    class InnerType{
        public:
            //通过这种方式，可以在模板类内部定义类型
            typename T::iterator begin;
            typename T::iterator end;
            //值传递
            void print(typename T::iterator b, typename T::iterator e){
                while (b!=e)
                {
                    std::cout << *b << ",";
                    b++;
                }
                std::cout << std::endl;
                
            };
            //const引用传递
            void printConst(const typename T::iterator &bc, const typename T::iterator &ec){
                typename T::iterator b = bc, e = ec;
                while (b!=e)
                {
                    std::cout << *b << ",";
                    b++;
                }
                std::cout << std::endl;
                
            };
            void printSelf(){
                while (begin!=end)
                {
                    std::cout << *begin << ",";
                    begin++;
                }
                std::cout << std::endl;
            };
            InnerType(){};
            InnerType(typename T::iterator b, typename T::iterator e):begin(b), end(e){};
    };

    void testClassTemplate(){
        Test<int, int> test;
        int i0 = test.compare(4, 5);
        int i1 = test.tmpInMember<int>(5);
        std::cout << "i0:" << i0 << " i1:" << i1 << std::endl;
    };
    void testInnerType(){
        InnerType<std::vector<std::string>> inner;
        std::string arr[] = {"abc", "def", "ghi"};
        std::vector<std::string> v(arr, arr + 3);
        
        inner.print(v.begin(), v.end());

        std::vector<std::string>::iterator b = v.begin();
        std::vector<std::string>::iterator e = v.end();
        inner.printConst(b, e);
        
        
        InnerType<std::vector<std::string>> inner2(v.begin(), v.end());
        inner2.printSelf();


        //InnerType<std::vector<std::string>> inner3("ddd", "vvv"); //类型不符合，报错
        //InnerType<std::vector<std::string>> inner4(3, 8); /类型不符合，报错
    };
};


#endif