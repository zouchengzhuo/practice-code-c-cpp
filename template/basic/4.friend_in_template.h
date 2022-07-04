#ifndef friend_in_template_h
#define friend_in_template_h

#include <iostream>
#include <vector>

namespace friendTmp{
    template<typename T> class FriendPrintSize;

    class FirendTest1{
        public:
            //任何实例化类型都设置为友元
            template<typename T> friend class FriendPrintSize;
            FirendTest1(){};
            FirendTest1(const int &size):size(size){};
        private:
            int size;
    };

    class FirendTest2{
        public:
            //任何实例化类型都设置为友元
            friend class FriendPrintSize<FirendTest2>;
            FirendTest2(){};
            FirendTest2(const int &size):size(size){};
        private:
            int size;
    };

    template<typename T>
    class FriendPrintSize{
        public:
            void print(const FirendTest1 &obj){
                std::cout << " size:" << obj.size << std::endl;
            };
            void print(const FirendTest2 &obj){
                std::cout << " size:" << obj.size << std::endl;
            };
    };



    void testFriendInTemplate(){
        FirendTest1 f1(1);
        FirendTest2 f2(2);
        FriendPrintSize<int> p1;
        p1.print(f1);
        //p1.print(f2);//报错，无法访问，因为friend只设置了特定的模板类实例化类型
        FriendPrintSize<FirendTest2> p2;
        p2.print(f2); //ok
    };
};

#endif