#include <iostream>
#include <string>
#include <codecvt>
#include <locale>
#include <regex>

#define IsAlpha         "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz"
#define IsAlnum         "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz"
#define IsN "0123456789²³¹¼½¾٠١٢٣٤٥٦٧٨٩۰۱۲۳۴۵۶۷۸۹߀߁߂߃߄߅߆߇߈߉०१२३४५६७८९০১২৩৪৫৬৭৮৯৴৵৶৷৸৹੦੧੨੩੪੫੬੭੮੯૦૧૨૩૪૫૬૭૮૯୦୧୨୩୪୫୬୭୮୯୲୳୴୵୶୷௦௧௨௩௪௫௬௭௮௯௰௱௲౦౧౨౩౪౫౬౭౮౯౸౹౺౻౼౽౾೦೧೨೩೪೫೬೭೮೯൦൧൨൩൪൫൬൭൮൯൰൱൲൳൴൵๐๑๒๓๔๕๖๗๘๙໐໑໒໓໔໕໖໗໘໙༠༡༢༣༤༥༦༧༨༩༪༫༬༭༮༯༰༱༲༳၀၁၂၃၄၅၆၇၈၉႐႑႒႓႔႕႖႗႘႙፩፪፫፬፭፮፯፰፱፲፳፴፵፶፷፸፹፺፻፼ᛮᛯᛰ០១២៣៤៥៦៧៨៩៰៱៲៳៴៵៶៷៸៹᠐᠑᠒᠓᠔᠕᠖᠗᠘᠙᥆᥇᥈᥉᥊᥋᥌᥍᥎᥏᧐᧑᧒᧓᧔᧕᧖᧗᧘᧙᧚᪀᪁᪂᪃᪄᪅᪆᪇᪈᪉᪐᪑᪒᪓᪔᪕᪖᪗᪘᪙᭐᭑᭒᭓᭔᭕᭖᭗᭘᭙᮰᮱᮲᮳᮴᮵᮶᮷᮸᮹᱀᱁᱂᱃᱄᱅᱆᱇᱈᱉᱐᱑᱒᱓᱔᱕᱖᱗᱘᱙⁰⁴⁵⁶⁷⁸⁹₀₁₂₃₄₅₆₇₈₉⅐⅑⅒⅓⅔⅕⅖⅗⅘⅙⅚⅛⅜⅝⅞⅟ⅠⅡⅢⅣⅤⅥⅦⅧⅨⅩⅪⅫⅬⅭⅮⅯⅰⅱⅲⅳⅴⅵⅶⅷⅸⅹⅺⅻⅼⅽⅾⅿↀↁↂↅↆↇↈ↉①②③④⑤⑥⑦⑧⑨⑩⑪⑫⑬⑭⑮⑯⑰⑱⑲⑳⑴⑵⑶⑷⑸⑹⑺⑻⑼⑽⑾⑿⒀⒁⒂⒃⒄⒅⒆⒇⒈⒉⒊⒋⒌⒍⒎⒏⒐⒑⒒⒓⒔⒕⒖⒗⒘⒙⒚⒛⓪⓫⓬⓭⓮⓯⓰⓱⓲⓳⓴⓵⓶⓷⓸⓹⓺⓻⓼⓽⓾⓿❶❷❸❹❺❻❼❽❾❿➀➁➂➃➄➅➆➇➈➉➊➋➌➍➎➏➐➑➒➓⳽㆒㆓㆔㆕㈠㈡㈢㈣㈤㈥㈦㈧㈨㈩㉈㉉㉊㉋㉌㉍㉎㉏㉑㉒㉓㉔㉕㉖㉗㉘㉙㉚㉛㉜㉝㉞㉟㊀㊁㊂㊃㊄㊅㊆㊇㊈㊉㊱㊲㊳㊴㊵㊶㊷㊸㊹㊺㊻㊼㊽㊾㊿꘠꘡꘢꘣꘤꘥꘦꘧꘨꘩ꛦꛧꛨꛩꛪꛫꛬꛭꛮꛯ꠰꠱꠲꠳꠴꠵꣐꣑꣒꣓꣔꣕꣖꣗꣘꣙꤀꤁꤂꤃꤄꤅꤆꤇꤈꤉꧐꧑꧒꧓꧔꧕꧖꧗꧘꧙꩐꩑꩒꩓꩔꩕꩖꩗꩘꩙꯰꯱꯲꯳꯴꯵꯶꯷꯸꯹０１２３４５６７８９"

void test1(){
    std::wstring result = L"a...b";
    //两个及以上的点前面加东西 如 "..." 变成 " DOTMULTI..."
    std::wregex r_rep_dotmulti_1(L"\\.([\\.]+)");
    result = std::regex_replace(result,r_rep_dotmulti_1, L" DOTMULTI$1");
    std::wstring_convert<std::codecvt_utf8<wchar_t>> cvt;
    std::string sresult = cvt.to_bytes(result);
    std::cout << "result:" << sresult << std::endl;
}

void test2(){
    std::wstring result = L"a',?',!',-'";
    std::wregex r_apostrophe6(L"([^" IsAlnum "\\s])[']");
    result = std::regex_replace(result,r_apostrophe6, L"$1 '");
    std::wstring_convert<std::codecvt_utf8<wchar_t>> cvt;
    std::string sresult = cvt.to_bytes(result);
    std::cout << "result:" << sresult << std::endl;
}

void test3(){
    std::wregex  r_apostrophe1(L"([^" IsAlpha "])[']([^" IsAlpha "])");
    std::wregex  r_apostrophe2(L"([^" IsAlpha "])[']([" IsAlpha "])");
    std::wregex  r_apostrophe3(L"([" IsAlpha "])[']([^" IsAlpha "])");
    std::wregex  r_apostrophe4(L"([" IsAlpha "])[']([" IsAlpha "])");
    std::wregex  r_apostrophe5(L"^[']([" IsAlpha "])"); // 句首的' 只能是引号

    std::wstring_convert<std::codecvt_utf8<wchar_t>> cvt;
    std::wstring result = L" 1'2  1'a  a'1  ";
    result = std::regex_replace(result, r_apostrophe1, L"$1 ' $2");
    result = std::regex_replace(result, r_apostrophe2, L"$1 ' $2");
    result = std::regex_replace(result, r_apostrophe3, L"$1 ' $2");
    std::cout << "r_apostrophe1:" <<   cvt.to_bytes(result) << std::endl;
}

void test4(){
    std::wregex  r_comma1(L"([^" IsN "])[,]");
    std::wregex  r_comma2(L"[,]([^" IsN "])");

    std::wstring_convert<std::codecvt_utf8<wchar_t>> cvt;
    std::wstring result = L"a,,a";
    result = std::regex_replace(result, r_comma1, L"$1 , ");
    result = std::regex_replace(result, r_comma2, L" , $1");
    std::cout << "r_apostrophe1:" <<   cvt.to_bytes(result) << std::endl;
}

int main(){
    //test1();
    //test2();
    //test3();
    test4();
}