#ifndef _TEST_H
#define _TEST_H

#include <stdio.h>
#include <string.h>
#include <math.h>

#define TEST_USE(n, var)\
    extern int test_##n();\
    if (!var) var = test_##n();
#define TEST_INIT(n)\
    static unsigned int __test_cnt_##n = 0;\
    static unsigned int __test_fail_cnt_##n = 0;
#define TEST_ASSERT(n,x)\
    __test_cnt_##n++;\
    if (!(x)) {\
        printf("ERROR on %s @%d %s: %s\n",__FILE__,__LINE__,__PRETTY_FUNCTION__,#x);\
        __test_fail_cnt_##n++;\
    }
#define TEST_ASSERT_FLOAT(n, a, b, diff)\
    __test_cnt_##n++;\
    if (fabsl(a-b) > diff) {\
        printf("ERROR on %s @%d %s: %s != %s\n",__FILE__,__LINE__,__PRETTY_FUNCTION__,#a, #b);\
        __test_fail_cnt_##n++;\
    }

#define TEST_CMP(n,x,b)\
    __test_cnt_##n++;\
    if (x!=b) {\
        printf("ERROR on %s @%d %s: %s != %s\n",__FILE__,__LINE__,__PRETTY_FUNCTION__,#x, #b);\
        __test_fail_cnt_##n++;\
    }

#define TEST_CMP_STR(n,x,b,cnt)\
    __test_cnt_##n++;\
    if (strncmp(x,b,cnt) != 0) {\
        printf("ERROR on %s @%d %s: %s\n",__FILE__,__LINE__,__PRETTY_FUNCTION__,#x);\
        __test_fail_cnt_##n++;\
    }

#define TEST_END(n)\
    if (__test_fail_cnt_##n>0) {\
        printf("%s: %d/%d FAILED\n",#n,__test_fail_cnt_##n,__test_cnt_##n);\
        return 1;\
    } else {\
        printf("%s: %d OK\n",#n,__test_cnt_##n);\
        return 0;\
    }

#define TEST_SUB(n, method)\
    if (method() != 0) {\
        printf("ERROR on subtest: %s @%d %s: %s\n",__FILE__,__LINE__,__PRETTY_FUNCTION__,#method);\
        return 1;\
    }

#define TEST_SUB_END(n)\
    return 0;

#define TEST_THROW(n, t, x, ee)\
    __test_cnt_##n++;\
    try {\
        (x);\
        __test_fail_cnt_##n++;\
    } catch (t exp) {\
        if (ee != std::string("") && exp != ee) __test_fail_cnt_##n++;\
    }

#endif
