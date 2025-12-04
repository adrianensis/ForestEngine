#pragma once

// --------------------------------------------------------
// COMMON
// --------------------------------------------------------

#define INVALID_INDEX -1
#define EMPTY_MACRO(...)
#define ADD_TRAIL_COMMA(token) token ,
#define TO_STRING(token) #token
#define TO_STRING_AND_ADD_TRAIL_COMMA(token) TO_STRING(token) ,
#define TO_U32(x) static_cast<Core::u32>(x)
#define NS_BEGIN(...) namespace __VA_ARGS__ {
#define NS_END }

// --------------------------------------------------------
// TYPE TRAITS
// --------------------------------------------------------

#define CAST(Class, Ptr) dynamic_cast<Class*>(Ptr)

#define IS_BASE_OF(BaseClass, DerivedClass) std::is_base_of<BaseClass, DerivedClass>::value

#define COND_TYPE(Bool, T1, T2) typename std::conditional<Bool, T1, T2>::type

#define ADD_POINTER(Class) typename std::add_pointer<Class>::type
#define REMOVE_POINTER(Class) typename std::remove_pointer<Class>::type
#define ADD_REFERENCE(Class) typename std::add_lvalue_reference<Class>::type
#define REMOVE_REFERENCE(Class) typename std::remove_reference<Class>::type
#define IS_RAW_POINTER(Class) std::is_pointer<REMOVE_REFERENCE(Class)>::value
#define IS_ARITHMETIC(Class) std::is_arithmetic<REMOVE_REFERENCE(Class)>::value
#define IS_ENUM(Class) std::is_enum<Class>::value
#define ADD_CONST(Class) typename std::add_const<Class>::type
#define REMOVE_CONST(Class) typename std::remove_const<Class>::type

#define T_EXTENDS(Class, BaseClass) requires std::derived_from<Class, BaseClass>

// --------------------------------------------------------
// FOR LOOPS
// --------------------------------------------------------

#define FOR_MAP(it, map) \
	for (auto it = (map).begin(); it != (map).end(); ++it)

#define FOR_RANGE(it, start, size) for (Core::i32 it = start; it < (Core::i32)(size); ++it)
#define FOR_RANGE_COND(it, start, size, condition) for (Core::i32 it = start; (it < (Core::i32)(size)) && (condition); ++it)

#define FOR_ARRAY(it, array) FOR_RANGE(it, 0, (array).size())
#define FOR_ARRAY_COND(it, array, condition) FOR_RANGE_COND(it, 0, (array).size(), (condition))

#define FOR_LIST(it, list) for (auto it = (list).begin(); it != (list).end(); ++it)
#define FOR_LIST_COND(it, list, condition) for (auto it = (list).begin(); (it != (list).end()) && (condition); ++it)

// --------------------------------------------------------
// FOR EACH
// --------------------------------------------------------

// C++20 required
// from: https://www.scs.stanford.edu/~dm/blog/va-opt.html

#define PARENS ()

#define EXPAND(...) EXPAND4(EXPAND4(EXPAND4(EXPAND4(__VA_ARGS__))))
#define EXPAND4(...) EXPAND3(EXPAND3(EXPAND3(EXPAND3(__VA_ARGS__))))
#define EXPAND3(...) EXPAND2(EXPAND2(EXPAND2(EXPAND2(__VA_ARGS__))))
#define EXPAND2(...) EXPAND1(EXPAND1(EXPAND1(EXPAND1(__VA_ARGS__))))
#define EXPAND1(...) __VA_ARGS__

#define N_VA_ARGS_(_100,_99,_98,_97,_96,_95,_94,_93,_92,_91,_90,_89,_88,_87,_86,_85,_84,_83,_82,_81,_80,_79,_78,_77,_76,_75,_74,_73,_72,_71,_70,_69,_68,_67,_66,_65,_64,_63,_62,_61,_60,_59,_58,_57,_56,_55,_54,_53,_52,_51,_50,_49,_48,_47,_46,_45,_44,_43,_42,_41,_40,_39,_38,_37,_36,_35,_34,_33,_32,_31,_30,_29,_28,_27,_26,_25,_24,_23,_22,_21,_20,_19,_18,_17,_16,_15,_14,_13,_12,_11,_10,_9,_8,_7,_6,_5,_4,_3,_2,_1, N, ...) N
#define N_VA_ARGS(...) N_VA_ARGS_(__VA_ARGS__ __VA_OPT__(,) 100,99,98,97,96,95,94,93,92,91,90,89,88,87,86,85,84,83,82,81,80,79,78,77,76,75,74,73,72,71,70,69,68,67,66,65,64,63,62,61,60,59,58,57,56,55,54,53,52,51,50,49,48,47,46,45,44,43,42,41,40,39,38,37,36,35,34,33,32,31,30,29,28,27,26,25,24,23,22,21,20,19,18,17,16,15,14,13,12,11,10,9,8,7,6,5,4,3,2,1,0)

#define FOR_EACH_0(FN, ...) 
#define FOR_EACH_1(FN, E, ...) FN(E) 
#define FOR_EACH_2(FN, E, ...) FN(E) FOR_EACH_1(FN, __VA_ARGS__)
#define FOR_EACH_3(FN, E, ...) FN(E) FOR_EACH_2(FN, __VA_ARGS__)
#define FOR_EACH_4(FN, E, ...) FN(E) FOR_EACH_3(FN, __VA_ARGS__)
#define FOR_EACH_5(FN, E, ...) FN(E) FOR_EACH_4(FN, __VA_ARGS__)
#define FOR_EACH_6(FN, E, ...) FN(E) FOR_EACH_5(FN, __VA_ARGS__)
#define FOR_EACH_7(FN, E, ...) FN(E) FOR_EACH_6(FN, __VA_ARGS__)
#define FOR_EACH_8(FN, E, ...) FN(E) FOR_EACH_7(FN, __VA_ARGS__)
#define FOR_EACH_9(FN, E, ...) FN(E) FOR_EACH_8(FN, __VA_ARGS__)
#define FOR_EACH_10(FN, E, ...) FN(E) FOR_EACH_9(FN, __VA_ARGS__)
#define FOR_EACH_11(FN, E, ...) FN(E) FOR_EACH_10(FN, __VA_ARGS__)
#define FOR_EACH_12(FN, E, ...) FN(E) FOR_EACH_11(FN, __VA_ARGS__)
#define FOR_EACH_13(FN, E, ...) FN(E) FOR_EACH_12(FN, __VA_ARGS__)
#define FOR_EACH_14(FN, E, ...) FN(E) FOR_EACH_13(FN, __VA_ARGS__)
#define FOR_EACH_15(FN, E, ...) FN(E) FOR_EACH_14(FN, __VA_ARGS__)
#define FOR_EACH_16(FN, E, ...) FN(E) FOR_EACH_15(FN, __VA_ARGS__)
#define FOR_EACH_17(FN, E, ...) FN(E) FOR_EACH_16(FN, __VA_ARGS__)
#define FOR_EACH_18(FN, E, ...) FN(E) FOR_EACH_17(FN, __VA_ARGS__)
#define FOR_EACH_19(FN, E, ...) FN(E) FOR_EACH_18(FN, __VA_ARGS__)
#define FOR_EACH_20(FN, E, ...) FN(E) FOR_EACH_19(FN, __VA_ARGS__)
#define FOR_EACH_21(FN, E, ...) FN(E) FOR_EACH_20(FN, __VA_ARGS__)
#define FOR_EACH_22(FN, E, ...) FN(E) FOR_EACH_21(FN, __VA_ARGS__)
#define FOR_EACH_23(FN, E, ...) FN(E) FOR_EACH_22(FN, __VA_ARGS__)
#define FOR_EACH_24(FN, E, ...) FN(E) FOR_EACH_23(FN, __VA_ARGS__)
#define FOR_EACH_25(FN, E, ...) FN(E) FOR_EACH_24(FN, __VA_ARGS__)
#define FOR_EACH_26(FN, E, ...) FN(E) FOR_EACH_25(FN, __VA_ARGS__)
#define FOR_EACH_27(FN, E, ...) FN(E) FOR_EACH_26(FN, __VA_ARGS__)
#define FOR_EACH_28(FN, E, ...) FN(E) FOR_EACH_27(FN, __VA_ARGS__)
#define FOR_EACH_29(FN, E, ...) FN(E) FOR_EACH_28(FN, __VA_ARGS__)
#define FOR_EACH_30(FN, E, ...) FN(E) FOR_EACH_29(FN, __VA_ARGS__)
#define FOR_EACH_31(FN, E, ...) FN(E) FOR_EACH_30(FN, __VA_ARGS__)
#define FOR_EACH_32(FN, E, ...) FN(E) FOR_EACH_31(FN, __VA_ARGS__)
#define FOR_EACH_33(FN, E, ...) FN(E) FOR_EACH_32(FN, __VA_ARGS__)
#define FOR_EACH_34(FN, E, ...) FN(E) FOR_EACH_33(FN, __VA_ARGS__)
#define FOR_EACH_35(FN, E, ...) FN(E) FOR_EACH_34(FN, __VA_ARGS__)
#define FOR_EACH_36(FN, E, ...) FN(E) FOR_EACH_35(FN, __VA_ARGS__)
#define FOR_EACH_37(FN, E, ...) FN(E) FOR_EACH_36(FN, __VA_ARGS__)
#define FOR_EACH_38(FN, E, ...) FN(E) FOR_EACH_37(FN, __VA_ARGS__)
#define FOR_EACH_39(FN, E, ...) FN(E) FOR_EACH_38(FN, __VA_ARGS__)
#define FOR_EACH_40(FN, E, ...) FN(E) FOR_EACH_39(FN, __VA_ARGS__)
#define FOR_EACH_41(FN, E, ...) FN(E) FOR_EACH_40(FN, __VA_ARGS__)
#define FOR_EACH_42(FN, E, ...) FN(E) FOR_EACH_41(FN, __VA_ARGS__)
#define FOR_EACH_43(FN, E, ...) FN(E) FOR_EACH_42(FN, __VA_ARGS__)
#define FOR_EACH_44(FN, E, ...) FN(E) FOR_EACH_43(FN, __VA_ARGS__)
#define FOR_EACH_45(FN, E, ...) FN(E) FOR_EACH_44(FN, __VA_ARGS__)
#define FOR_EACH_46(FN, E, ...) FN(E) FOR_EACH_45(FN, __VA_ARGS__)
#define FOR_EACH_47(FN, E, ...) FN(E) FOR_EACH_46(FN, __VA_ARGS__)
#define FOR_EACH_48(FN, E, ...) FN(E) FOR_EACH_47(FN, __VA_ARGS__)
#define FOR_EACH_49(FN, E, ...) FN(E) FOR_EACH_48(FN, __VA_ARGS__)
#define FOR_EACH_50(FN, E, ...) FN(E) FOR_EACH_49(FN, __VA_ARGS__)
#define FOR_EACH_51(FN, E, ...) FN(E) FOR_EACH_50(FN, __VA_ARGS__)
#define FOR_EACH_52(FN, E, ...) FN(E) FOR_EACH_51(FN, __VA_ARGS__)
#define FOR_EACH_53(FN, E, ...) FN(E) FOR_EACH_52(FN, __VA_ARGS__)
#define FOR_EACH_54(FN, E, ...) FN(E) FOR_EACH_53(FN, __VA_ARGS__)
#define FOR_EACH_55(FN, E, ...) FN(E) FOR_EACH_54(FN, __VA_ARGS__)
#define FOR_EACH_56(FN, E, ...) FN(E) FOR_EACH_55(FN, __VA_ARGS__)
#define FOR_EACH_57(FN, E, ...) FN(E) FOR_EACH_56(FN, __VA_ARGS__)
#define FOR_EACH_58(FN, E, ...) FN(E) FOR_EACH_57(FN, __VA_ARGS__)
#define FOR_EACH_59(FN, E, ...) FN(E) FOR_EACH_58(FN, __VA_ARGS__)
#define FOR_EACH_60(FN, E, ...) FN(E) FOR_EACH_59(FN, __VA_ARGS__)
#define FOR_EACH_61(FN, E, ...) FN(E) FOR_EACH_60(FN, __VA_ARGS__)
#define FOR_EACH_62(FN, E, ...) FN(E) FOR_EACH_61(FN, __VA_ARGS__)
#define FOR_EACH_63(FN, E, ...) FN(E) FOR_EACH_62(FN, __VA_ARGS__)
#define FOR_EACH_64(FN, E, ...) FN(E) FOR_EACH_63(FN, __VA_ARGS__)
#define FOR_EACH_65(FN, E, ...) FN(E) FOR_EACH_64(FN, __VA_ARGS__)
#define FOR_EACH_66(FN, E, ...) FN(E) FOR_EACH_65(FN, __VA_ARGS__)
#define FOR_EACH_67(FN, E, ...) FN(E) FOR_EACH_66(FN, __VA_ARGS__)
#define FOR_EACH_68(FN, E, ...) FN(E) FOR_EACH_67(FN, __VA_ARGS__)
#define FOR_EACH_69(FN, E, ...) FN(E) FOR_EACH_68(FN, __VA_ARGS__)
#define FOR_EACH_70(FN, E, ...) FN(E) FOR_EACH_69(FN, __VA_ARGS__)
#define FOR_EACH_71(FN, E, ...) FN(E) FOR_EACH_70(FN, __VA_ARGS__)
#define FOR_EACH_72(FN, E, ...) FN(E) FOR_EACH_71(FN, __VA_ARGS__)
#define FOR_EACH_73(FN, E, ...) FN(E) FOR_EACH_72(FN, __VA_ARGS__)
#define FOR_EACH_74(FN, E, ...) FN(E) FOR_EACH_73(FN, __VA_ARGS__)
#define FOR_EACH_75(FN, E, ...) FN(E) FOR_EACH_74(FN, __VA_ARGS__)
#define FOR_EACH_76(FN, E, ...) FN(E) FOR_EACH_75(FN, __VA_ARGS__)
#define FOR_EACH_77(FN, E, ...) FN(E) FOR_EACH_76(FN, __VA_ARGS__)
#define FOR_EACH_78(FN, E, ...) FN(E) FOR_EACH_77(FN, __VA_ARGS__)
#define FOR_EACH_79(FN, E, ...) FN(E) FOR_EACH_78(FN, __VA_ARGS__)
#define FOR_EACH_80(FN, E, ...) FN(E) FOR_EACH_79(FN, __VA_ARGS__)
#define FOR_EACH_81(FN, E, ...) FN(E) FOR_EACH_80(FN, __VA_ARGS__)
#define FOR_EACH_82(FN, E, ...) FN(E) FOR_EACH_81(FN, __VA_ARGS__)
#define FOR_EACH_83(FN, E, ...) FN(E) FOR_EACH_82(FN, __VA_ARGS__)
#define FOR_EACH_84(FN, E, ...) FN(E) FOR_EACH_83(FN, __VA_ARGS__)
#define FOR_EACH_85(FN, E, ...) FN(E) FOR_EACH_84(FN, __VA_ARGS__)
#define FOR_EACH_86(FN, E, ...) FN(E) FOR_EACH_85(FN, __VA_ARGS__)
#define FOR_EACH_87(FN, E, ...) FN(E) FOR_EACH_86(FN, __VA_ARGS__)
#define FOR_EACH_88(FN, E, ...) FN(E) FOR_EACH_87(FN, __VA_ARGS__)
#define FOR_EACH_89(FN, E, ...) FN(E) FOR_EACH_88(FN, __VA_ARGS__)
#define FOR_EACH_90(FN, E, ...) FN(E) FOR_EACH_89(FN, __VA_ARGS__)
#define FOR_EACH_91(FN, E, ...) FN(E) FOR_EACH_90(FN, __VA_ARGS__)
#define FOR_EACH_92(FN, E, ...) FN(E) FOR_EACH_91(FN, __VA_ARGS__)
#define FOR_EACH_93(FN, E, ...) FN(E) FOR_EACH_92(FN, __VA_ARGS__)
#define FOR_EACH_94(FN, E, ...) FN(E) FOR_EACH_93(FN, __VA_ARGS__)
#define FOR_EACH_95(FN, E, ...) FN(E) FOR_EACH_94(FN, __VA_ARGS__)
#define FOR_EACH_96(FN, E, ...) FN(E) FOR_EACH_95(FN, __VA_ARGS__)
#define FOR_EACH_97(FN, E, ...) FN(E) FOR_EACH_96(FN, __VA_ARGS__)
#define FOR_EACH_98(FN, E, ...) FN(E) FOR_EACH_97(FN, __VA_ARGS__)
#define FOR_EACH_99(FN, E, ...) FN(E) FOR_EACH_98(FN, __VA_ARGS__)
#define FOR_EACH_100(FN, E, ...) FN(E) FOR_EACH_99(FN, __VA_ARGS__)

#define FOR_EACH__(FN, NARGS, ...) FOR_EACH_##NARGS(FN, __VA_ARGS__) 
#define FOR_EACH_(FN, NARGS, ...) FOR_EACH__(FN, NARGS, __VA_ARGS__)
#define FOR_EACH(FN, ...) FOR_EACH_(FN, N_VA_ARGS(__VA_ARGS__), __VA_ARGS__)
// ODD
#define FOR_EACH_ODD(macro, ...)                                    \
  __VA_OPT__(EXPAND(FOR_EACH_HELPER_ODD(macro, __VA_ARGS__)))

#define FOR_EACH_HELPER_ODD(macro, a1, a2, ...)                         \
  macro(a1)                                                     \
  __VA_OPT__(FOR_EACH_AGAIN_ODD PARENS (macro, __VA_ARGS__))

#define FOR_EACH_AGAIN_ODD() FOR_EACH_HELPER_ODD

// EVEN
#define FOR_EACH_EVEN(macro, ...)                                    \
  __VA_OPT__(EXPAND(FOR_EACH_HELPER_EVEN(macro, __VA_ARGS__)))

#define FOR_EACH_HELPER_EVEN(macro, a1, a2, ...)                         \
  macro(a2)                                                     \
  __VA_OPT__(FOR_EACH_AGAIN_EVEN PARENS (macro, __VA_ARGS__))

#define FOR_EACH_AGAIN_EVEN() FOR_EACH_HELPER_EVEN

// #define CHECK_EQUAL(...) true FOR_EACH(CHECK_EQUAL_HELPER, __VA_ARGS__)
// #define CHECK_EQUAL_HELPER(name) && BaseName == other.name
