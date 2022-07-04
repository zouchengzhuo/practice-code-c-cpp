#include <fstream>
#include <iostream>
#include <map>


void testMap(){
    std::map<std::string, std::string> m;
    m.insert(std::make_pair<std::string, std::string>("key1", "value1"));
    m.insert(std::make_pair<std::string, std::string>("key2", "value2"));
    std::map<std::string, std::string>::const_iterator iter = m.begin();
    while (iter != m.end())
    {
        std::cout << "key:" << iter->first << " value:" << iter->second << std::endl;
        iter++;
    };
};

int main(){
    //testMap();
    return 0;
}