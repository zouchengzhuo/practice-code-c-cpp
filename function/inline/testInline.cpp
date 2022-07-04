#include "testInline.h"
#include "chrono"

std::time_t getTimeStamp()
{
    std::chrono::time_point<std::chrono::system_clock,std::chrono::milliseconds> tp = std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::system_clock::now());
    auto tmp=std::chrono::duration_cast<std::chrono::milliseconds>(tp.time_since_epoch());
    std::time_t timestamp = tmp.count();
    //std::time_t timestamp = std::chrono::system_clock::to_time_t(tp);
    return timestamp;
}

int main(){
    
    const long round = 1000000000;
    int res;
    auto now = getTimeStamp();
    for(int i=0; i<round; i++){
        res = testTime(4, 7);
    }
    auto diff1 = getTimeStamp() - now;
    std::cout << "normal fn diff:" << diff1 << std::endl;
    now = getTimeStamp();
    for(int i=0; i<round; i++){
        res = testInlineTIme(4, 7);
    }
    auto diff2 = getTimeStamp() - now;
    std::cout << "inline fn diff:" << diff2 << std::endl;
    std::cout << "diff2 - diff1:" << diff2 - diff1 << std::endl;

    //结论： 这种模拟，无论涉及还是不涉及形参拷贝，速度都差不多
}