#include "classextend.h"
#include "1234.basic_extend.h"
#include "5.convert_base_child.h"
#include "6.constructor_in_extend.h"
#include "7.copy_assignment_in_extend.h"
#include "8.destructor_in_extend.h"
#include "9.name_find_in_extend.h"
#include "10.abstract_class.h"
#include "11.container_with_extend.h"
#include "12.handler_in_extend.h"

int main(){
    RedBook book("redbook", 99,"child");
    //printBookInfo(book);
    //printBookInfoInBase(book);
    //testChangeOne();
    //testConvert();
    //testExtendConstructor();
    //testCopyAndAssignment();
    //testDestructor();
    //testNameFind();
    //testAbstractClass();
    //testContainerWithExtend();
    testHandle();
};