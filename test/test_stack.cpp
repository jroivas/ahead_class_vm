#include <test.h>
#include <vm_stack.h>

int test_stack() {
    TEST_INIT(stack);

    vmStack st(4);
    vmObject o;

    TEST_ASSERT(stack, st.size == 0);
    TEST_ASSERT(stack, st.peekType() == ST_EMPTY);

    st.pushInteger(42);
    TEST_ASSERT(stack, st.size == 1);
    TEST_ASSERT(stack, st.peekType() == ST_INT);

    st.pushLong(123456789012345L);
    TEST_ASSERT(stack, st.size == 2);
    TEST_ASSERT(stack, st.peekType() == ST_LONG);

    st.pushFloat(0.1234567890123f);
    TEST_ASSERT(stack, st.size == 3);
    TEST_ASSERT(stack, st.peekType() == ST_FLOAT);

    st.pushDouble(0.1234567890123);
    TEST_ASSERT(stack, st.size == 4);
    TEST_ASSERT(stack, st.peekType() == ST_DOUBLE);

    TEST_THROW(stack, const char*, st.pushObject(&o), std::string(""));
    TEST_ASSERT(stack, st.size == 4);

    double a = st.popDouble();
    std::cout.precision(20);
    TEST_ASSERT_FLOAT(stack, a, 0.1234567890123, 0.00000000000001);

    float b = st.popFloat();
    TEST_ASSERT_FLOAT(stack, b, 0.1234567890123, 0.00000001);
    TEST_ASSERT(stack, st.popLong() == 123456789012345L);
    TEST_ASSERT(stack, st.popInteger() == 42);

    TEST_ASSERT(stack, st.size == 0);
    TEST_ASSERT(stack, st.peekType() == ST_EMPTY);

    TEST_THROW(stack, const char*, st.popLong(), std::string(""));

    st.pushObject(&o);
    TEST_ASSERT(stack, st.peekType() == ST_OBJ);
    TEST_ASSERT(stack, st.size == 1);
    TEST_ASSERT(stack, &o == st.popObject());

    TEST_END(stack);
}
