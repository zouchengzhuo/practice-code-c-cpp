#include <iostream>
#include <regex>
#include <string>

int main(){
    std::string origin = "xxxxx  , xxx'xxx  ', xxxxxx     ?   xxxx";
    std::regex r("\\s+(?!=,|'|.|\\?)");
    std::string new_str = std::regex_replace(origin, r, "");
    std::cout << new_str << std::endl;
}