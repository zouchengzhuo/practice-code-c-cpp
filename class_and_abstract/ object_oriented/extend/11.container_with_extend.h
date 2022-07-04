#ifndef container_with_extend_h
#define container_with_extend_h
#include <iostream>
#include <vector>

class ContainerItemBase{
    public:
        ContainerItemBase(const int &a):a(a){};
        int a;
};
class ContainerItem: public ContainerItemBase{
    public:
        ContainerItem(const int &a, const int &b): ContainerItemBase(a), b(b){};
        int b;
};

//容器中，只能存储泛型类型的对象，向基类容器存储派生类对象时， 会丢掉派生旅特有部分，只留下基类部分
//实际上容器中的已经是基类对象，而不是派生类对象了
void testContainerWithExtend(){
    std::vector<ContainerItemBase> v;
    ContainerItem item(1, 2);
    v.push_back(item);
    std::cout << v[0].a << std::endl;
    //std::cout << v[0].b << std::endl;//会报错
};

#endif