#ifndef not_type_template_h
#define not_type_template_h
#include <iostream>
//模板类型形参，也可以不是类型，是常量，这个用法值得注意

namespace nottype{
    template<typename T, size_t K>
    //可以这样指定模板类型的默认值
    void init_c_arr(T (&arr)[K], const T &dft = T()){
        for(size_t i=0;i<K;i++){
            arr[i] = dft;
        }
    };

    void testNotTypeTemplate(){
        int arr1[56];
        //直接初始化，自动推断模板类型 <int, 56>
        init_c_arr(arr1);
        std::string arr2[33];
        init_c_arr<std::string, 33>(arr2, "abc");
        std::cout << "arr1[8]:" << arr1[8] << std::endl;
        std::cout << "arr2[8]:" << arr2[8] << std::endl;
    };
};

#endif