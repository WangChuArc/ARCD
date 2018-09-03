#pragma once

#include <algorithm>
#include <functional>
#include <tuple>
#include <iterator>
#include <cassert>

namespace FPInCpp
{
    // type does not exist
    class TDNE {};

    template<typename T>
    struct TypeExist : std::true_type
    {
    };

    template<>
    struct TypeExist<TDNE> : std::false_type
    {
    };

    template<typename T>
    struct RemoveFuncConst
    {
        typedef T type;
    };

    template<typename TR, typename TC, typename ...TArgs>
    struct RemoveFuncConst<TR(TC::*)(TArgs...) const>
    {
        using type = TR(TC::*)(TArgs...);
    };

    template<typename T>
    struct IsContainer
    {
        typedef typename std::decay<T>::type decayedType;

        template<typename>
        struct _SFINAE
        {
            typedef std::true_type type;
        };

        template<typename U> static typename _SFINAE<typename U::iterator>::type _impl(int);
        template<typename U> static typename std::false_type _impl(float);

        typedef decltype(_impl<decayedType>(0)) type;
        static const bool value = type::value;
    };

    template<typename TL, typename TR>
    struct CommonType
    {
        template<typename T>
        struct _SFINAE
        {
            typedef T type;
        };

        template<typename UL, typename UR> static  typename _SFINAE<decltype(false ? declval<UL>() : declval<UR>())>::type _impl(int);
        template<typename UL, typename UR> static TDNE _impl(float);

        typedef decltype(_impl<TL, TR>(0)) type;
    };

    template<typename T>
    struct _ExtractType
    {
        typedef int _FirstChoice;
        typedef float _SecondChoice;

        template<typename U> static typename U::type _extract_impl(_FirstChoice);
        template<typename U> static TDNE _extract_impl(_SecondChoice);

        typedef decltype(_extract_impl<T>(_FirstChoice())) type;
    };

    template<typename T>
    struct _HasMemberType_impl : std::true_type
    {
    };

    template<>
    struct _HasMemberType_impl<TDNE> : std::false_type
    {
    };

    template<typename T>
    struct _HasMemberType : _HasMemberType_impl<typename _ExtractType<T>::type>
    {
    };

    

    template<typename TCvtFrom, typename TCvtTo>
    struct _IsConvertible
    {
        static std::true_type _SFINAE_impl(TCvtTo);

        template<typename U> static decltype(_SFINAE_impl(declval<U>())) _IsConvertible_impl(int);
        template<typename U> static std::false_type _IsConvertible_impl(float);

        typedef decltype(_IsConvertible_impl<TCvtFrom>(0)) type;
        static const bool value = type::value;
    };

    template<typename T>
    struct _ExtractBoolean_impl : std::false_type
    {
    };

    template<>
    struct _ExtractBoolean_impl<std::true_type> : std::true_type
    {
    };

    template<>
    struct _ExtractBoolean_impl<TDNE> : std::false_type
    {
    };

    template<typename T>
    struct _ExtractBoolean : _ExtractBoolean_impl<typename _ExtractType<T>::type>
    {
        
    };

    template<size_t ...TSeq>
    struct IntegerSequence
    {
        // nothing
    };

    template<size_t, typename>
    struct _CombineSequence
    {
        // should not be used
    };

    template<size_t THead, size_t ...TTails>
    struct _CombineSequence<THead, IntegerSequence<TTails...>>
    {
        typedef IntegerSequence<TTails..., THead> type;
    };

    template<size_t TNum>
    struct IndexSequence
    {
        static_assert(TNum >= 0, "the index integer sequence should begin with 0");
        typedef typename _CombineSequence<TNum, typename IndexSequence<TNum - 1>::type>::type type;
    };

    template<>
    struct IndexSequence<0>
    {
        typedef IntegerSequence<0> type;
    };
    template<size_t TNum>
    struct TailIdxSequence
    {
        static_assert(TNum > 0, "the tails length of the tuple shoudle greater than 0");
        typedef typename _CombineSequence<TNum, typename IndexSequence<TNum - 1>::type>::type type;
    };

    template<>
    struct TailIdxSequence<1>
    {
        typedef IntegerSequence<1> type;
    };

    template<typename T>
    struct _IsTuple : std::false_type
    { 
    };

    template<typename ...TArgs>
    struct _IsTuple<std::tuple<TArgs...>> : std::true_type
    {
    };

    template<typename TLeft, typename TRight>
    struct _TupleTypeConcat {};

    template<typename ...TLeftArgs, typename ...TRightArgs>
    struct _TupleTypeConcat<std::tuple<TLeftArgs...>, std::tuple<TRightArgs...>>
    {
        typedef std::tuple<TLeftArgs..., TRightArgs...> type;
    };

    template<template<typename... TParams> class TFunc>
    struct _StaticFuncHelper
    {
        static const int argsNum = sizeof...(TParams);
    };


    template<typename T>
    struct _TplHeadAndTails
    {

    };

    template<typename THead, typename ...TTails>
    struct _TplHeadAndTails<std::tuple<THead, TTails...>>
    {
        typedef std::tuple<THead, TTails...> type;
        typedef THead headType;
        typedef std::tuple<TTails...> tailsType;
    };

    template<>
    struct _TplHeadAndTails<std::tuple<>>
    {
        typedef std::tuple<> type;
        typedef TDNE headType;
        typedef std::tuple<> tailsType;
    };

    template<typename TTpl, typename TIdxSeq>
    struct _TplInitAndLast_impl
    {

    };

    template<typename TTpl, size_t... TIdxSeq>
    struct _TplInitAndLast_impl<TTpl, IntegerSequence<TIdxSeq...>>
    {
        static const int _last = sizeof...(TIdxSeq);
        typedef std::tuple<typename std::tuple_element<TIdxSeq, TTpl>::type...> initType;
        typedef typename std::tuple_element<_last, TTpl>::type lastType;
    };

    template<typename T>
    struct _TplInitAndLast{};

    template<typename... T>
    struct _TplInitAndLast<std::tuple<T...>>
    {
        typedef std::tuple<T...> type;
        static const int ArgsCount = sizeof...(T);
        typedef typename _TplInitAndLast_impl<type, typename IndexSequence<ArgsCount - 2>::type>::initType initType;
        typedef typename _TplInitAndLast_impl<type, typename IndexSequence<ArgsCount - 2>::type>::lastType lastType;
    };

    template<typename T>
    struct _TplInitAndLast<std::tuple<T>>
    {
        typedef std::tuple<T> type;
        static const int ArgsCount = 1;
        typedef std::tuple<> initType;
        typedef T lastType;
    };

    template<>
    struct _TplInitAndLast<std::tuple<>>
    {
        typedef std::tuple<> type;
        static const int ArgsCount = 0;
        typedef std::tuple<> initType;
        typedef TDNE lastType;
    };

    template<typename T>
    struct _TupleHelper {};

    template<typename ...T>
    struct _TupleHelper<std::tuple<T...>>
    {
        typedef std::tuple<T...> type;
        static const int size = sizeof...(T);
        typedef typename _TplHeadAndTails<type>::headType headType;
        typedef typename _TplHeadAndTails<type>::tailsType tailsType;
        typedef typename _TplInitAndLast<type>::initType initType;
        typedef typename _TplInitAndLast<type>::lastType lastType;
    };


    template<typename TLeft, typename TRight>
    struct _ArgsWrapperTypeConcat {};

    template<typename TLeft, typename TRight>
    typename _ArgsWrapperTypeConcat<TLeft, TRight>::type ArgsWrapper_cat(TLeft& _leftArg, TRight& _rightArg);


    template<typename ...TArgs>
    struct ArgsWrapper
    {
        static const int elemNum = sizeof...(TArgs);
        static_assert(elemNum >= 0, "the number of the args should greater than 0");

        template<typename TLeft, typename TRight> 
        friend typename _ArgsWrapperTypeConcat<TLeft, TRight>::type ArgsWrapper_cat(TLeft& _leftArg, TRight& _rightArg);

        struct ToTuple { typedef typename std::tuple<TArgs...> type; };

        template<int TIdx>
        struct elemType
        {
            static_assert(TIdx >= 0, "index of element should greater than 0");
            static_assert(TIdx < sizeof...(TArgs), "index overflow");

            typedef typename std::tuple_element<TIdx, typename ToTuple::type>::type type;
        };

        ArgsWrapper(std::tuple<TArgs...>&& _src) : m_tuple(_src) {};

        ArgsWrapper(ArgsWrapper<TArgs...>&& _src) : m_tuple(std::move(_src.m_tuple)) {};
        ArgsWrapper(const ArgsWrapper<TArgs...>& _src) : m_tuple(_src.m_tuple) {};

        ArgsWrapper<TArgs...>& operator =(ArgsWrapper<TArgs...>&& _src) { m_tuple = move(_src.m_tuple); return *this; };
        ArgsWrapper<TArgs...>& operator =(const ArgsWrapper<TArgs...>& _src) { m_tuple = _src.m_tuple; return *this; };

        ~ArgsWrapper() {};

        template<int TIdx>
        typename std::tuple_element<TIdx, typename ToTuple::type>::type get() 
        {
            static_assert(TIdx >= 0, "index of element should greater than 0");
            static_assert(TIdx < sizeof...(TArgs), "index overflow");

            return std::get<TIdx>(m_tuple); 
        }

    private:
        std::tuple<TArgs...> m_tuple;
    };



    template<typename T>
    struct FromTuple {};

    template<typename ...TArgs>
    struct FromTuple<std::tuple<TArgs...>>
    {
        struct ToArgsWrapper { typedef typename ArgsWrapper<TArgs...> type; };
    };

    template<typename T>
    struct _ArgsWrapperHelper{};

    template<typename ...TArgs>
    struct _ArgsWrapperHelper<ArgsWrapper<TArgs...>>
    {
        typedef ArgsWrapper<TArgs...> type;
        typedef typename type::ToTuple::type tupleType;

        typedef typename _TupleHelper<tupleType>::headType headType;
        typedef typename FromTuple<typename _TupleHelper<tupleType>::tailsType>::ToArgsWrapper::type tailsType;
        typedef typename FromTuple<typename _TupleHelper<tupleType>::initType>::ToArgsWrapper::type initType;
        typedef typename _TupleHelper<tupleType>::lastType lastType;
    };

    template<typename ...TArgs>
    struct TransToArgsWrapper
    {
        typedef ArgsWrapper<TArgs...> type;
    };

    template<typename ...TArgs>
    struct TransToArgsWrapper<std::tuple<TArgs...>>
    {
        typedef ArgsWrapper<TArgs...> type;
    };

    template<typename ...TArgs>
    struct TransToArgsWrapper<ArgsWrapper<TArgs...>>
    {
        typedef ArgsWrapper<TArgs...> type;
    };

    template<typename ...TArgs>
    ArgsWrapper<TArgs...> make_ArgsWrapper(TArgs&&... _args)
    {
        return ArgsWrapper<TArgs...>(std::make_tuple(_args...));
    }

    template<typename ...TArgs>
    ArgsWrapper<TArgs...> make_ArgsWrapper(std::tuple<TArgs...>& _tuple)
    {
        return make_ArgsWrapper(std::tuple<TArgs...>(_tuple));
    }

    template<typename ...TArgs>
    ArgsWrapper<TArgs...> make_ArgsWrapper(std::tuple<TArgs...>&& _tuple)
    {
        return ArgsWrapper<TArgs...>(std::move(_tuple));
    }

    template<typename ...TLeftArgs, typename ...TRightArgs>
    struct _ArgsWrapperTypeConcat<ArgsWrapper<TLeftArgs...>, ArgsWrapper<TRightArgs...>>
    {
        typedef ArgsWrapper<TLeftArgs..., TRightArgs...> type;
    };

    template<typename TLeft, typename TRight>
    typename _ArgsWrapperTypeConcat<TLeft, TRight>::type ArgsWrapper_cat(TLeft& _leftArg, TRight& _rightArg)
    {
        return typename _ArgsWrapperTypeConcat<TLeft, TRight>::type(tuple_cat(_leftArg.m_tuple, _rightArg.m_tuple));
    }

    template<typename TLeft, typename TRight>
    struct StaticAnd : std::false_type
    {
    };

    template<>
    struct StaticAnd<std::true_type, std::true_type> : std::true_type
    {
        
    };

    template<typename TLeft, typename TRight>
    struct StaticOr : std::true_type
    {
        
    };

    template<>
    struct  StaticOr<std::false_type, std::false_type> : std::false_type
    {
        
    };

    template<>
    struct  StaticOr<TDNE, TDNE> : std::false_type
    {
        
    };

    template<typename T>
    struct StaticNot : std::false_type
    {
        
    };

    template<>
    struct StaticNot<std::false_type> : std::true_type
    {
        
    };

    template<>
    struct StaticNot<TDNE> : std::true_type
    {

    };

    template<typename TLeft, typename TRight>
    struct StaticXor : std::true_type
    {

    };
    
    template<typename T>
    struct StaticXor<T, T> : std::false_type
    {
        
    };


    template<template<typename> class TFunc, typename TResult, typename TArgsWarpper>
    struct _StaticMap_impl
    {
        typedef _ArgsWrapperHelper<TArgsWarpper> ArgsExtracter;
        typedef typename _StaticMap_impl<
            TFunc, 
            typename _ArgsWrapperTypeConcat<
                TResult, 
                ArgsWrapper<
                    typename _ExtractType<
                        TFunc<
                            typename ArgsExtracter::headType
                        > /* TFunc */
                    >::type /* _ExtractType */
                > /* ArgsWrapper */
            >::type /* _ArgsWrapperTypeConcat */,
            typename ArgsExtracter::tailsType
        >::type /* _StaticMap_impl */ type;
    };

    template<template<typename> class TFunc, typename TResult>
    struct _StaticMap_impl<TFunc, TResult, ArgsWrapper<>>
    {
        typedef TResult type;
    };

    template<typename ...TArgs>
    struct StaticMap
    {
        template<template<typename> class TFunc>
        struct  Using
        {
            typedef typename _StaticMap_impl<TFunc, ArgsWrapper<>, ArgsWrapper<TArgs...>>::type type;
        };
    };

    template<template<typename, typename> class TFunc, typename TAcc, typename TArgs>
    struct _StaticReduce_impl
    {
    };

    template<template<typename, typename> class TFunc, typename TAcc, typename THead, typename ...TTails>
    struct _StaticReduce_impl<TFunc, TAcc, ArgsWrapper<THead, TTails...>>
    {
        typedef typename _StaticReduce_impl<TFunc, typename _ExtractType<TFunc<THead, TAcc>>::type, ArgsWrapper<TTails...>>::type type;
    };

    template<template<typename, typename> class TFunc, typename TAcc>
    struct _StaticReduce_impl<TFunc, TAcc, ArgsWrapper<>>
    {
        typedef typename _ExtractType<TAcc>::type type;
    };

    template<typename ...TArgs>
    struct StaticReduce
    {
        template<typename TDefault>
        struct With
        {
            template<template<typename, typename> class TFunc>
            struct Using
            {
                typedef typename _StaticReduce_impl<TFunc, TDefault, ArgsWrapper<TArgs...>>::type type;
            };
        };
    };

    template<typename TBoolean, typename TNode, typename TNodeList>
    struct _TryCatchNode
    {
        typedef TNodeList type;
    };

    template<typename TNode, typename TNodeList>
    struct _TryCatchNode<std::true_type, TNode, TNodeList>
    {
        typedef typename _ArgsWrapperTypeConcat<TNodeList, ArgsWrapper<TNode>>::type type;
    };

    template<template<typename> class TFunc, typename TArgs, typename TResult>
    struct _StaticFilter_impl
    {
        typedef typename _ArgsWrapperHelper<TArgs> ArgsExtracter;
        typedef typename _StaticFilter_impl<
                            TFunc, 
                            typename ArgsExtracter::tailsType, 
                            typename _TryCatchNode<
                                typename _ExtractBoolean<
                                    TFunc<
                                        typename ArgsExtracter::headType
                                    >
                                >::type, 
                                typename ArgsExtracter::headType, 
                                TResult
                            >::type
                        >::type type;
    };

    template<template<typename> class TFunc, typename TResult>
    struct _StaticFilter_impl<TFunc, ArgsWrapper<>, TResult>
    {
        typedef TResult type;
    };

    template<typename ...TArgs>
    struct StaticFilter
    {
        template<template<typename> class TFunc>
        struct Using
        {
            typedef typename _StaticFilter_impl<TFunc, ArgsWrapper<TArgs...>, ArgsWrapper<>>::type type;
        };
    };

    
    template<typename TLeftArgs, typename TRightArgs, template<typename, typename> class TFunc, typename TResults>
    struct _StaticZip_impl
    {
        typedef typename _StaticZip_impl<
            typename _ArgsWrapperHelper<TLeftArgs>::tailsType,
            typename _ArgsWrapperHelper<TRightArgs>::tailsType,
            TFunc, 
            typename _ArgsWrapperTypeConcat<
                TResults, 
                ArgsWrapper<
                    typename _ExtractType<
                        TFunc<
                            typename _ArgsWrapperHelper<TLeftArgs>::headType,
                            typename _ArgsWrapperHelper<TRightArgs>::headType
                        > /* TFunc */
                    >::type /* _ExtractType */
                > /* ArgsWrapper */
            >::type /* TypeConcat */
        >::type /* _StaticZip_impl */ type;
    };

    template<template<typename, typename> class TFunc, typename TResult>
    struct _StaticZip_impl<ArgsWrapper<>, ArgsWrapper<>, TFunc, TResult>
    {
        typedef TResult type;
    };

    template<typename ...TLeftArgs>
    struct StaticZip
    {
        template<typename ...TRightArgs>
        struct With
        {
            template<template<typename, typename>class TFunc>
            struct Using
            {
                static_assert(sizeof...(TLeftArgs) == sizeof...(TRightArgs), "two arg list should have same size");

                typedef typename _StaticZip_impl<ArgsWrapper<TLeftArgs...>, ArgsWrapper<TRightArgs...>, TFunc, ArgsWrapper<>>::type type;
            };
        };
    };



    template<typename TLeft, typename TRight>
    struct _IsSame
    {
        typedef typename std::is_same<typename std::decay<TLeft>::type, typename std::decay<TRight>::type>::type type;
    };

    template<typename TArg>
    struct _Is
    {
        template<typename ...TList>
        struct In
        {
            typedef typename _StaticReduce_impl<StaticOr, std::false_type, ArgsWrapper<typename _IsSame<TArg, TList>::type...>>::type type;
        };
    };

    template<typename T>
    struct IsFunctor
    {
        template<typename>
        struct _SFINAE
        {
            typedef std::true_type type;
        };

        typedef typename std::decay<T>::type functorType;

        template<typename U> static typename _SFINAE<decltype(&functorType::operator())>::type _IsFunctor_impl(int);
        template<typename U> static std::false_type _IsFunctor_impl(float);

        typedef decltype(_IsFunctor_impl<T>(0)) type;
        static const bool value = type::value;
    };

    template<typename TIsFunctor, typename TFunc>
    struct _CallableType_impl
    {
        typedef TFunc type;
    };

    template<typename TFunc>
    struct _CallableType_impl<std::true_type, TFunc>
    {
        typedef decltype(&TFunc::operator()) type;
    };

    template<typename T>
    struct _CallableType
    {
        typedef typename _CallableType_impl<typename IsFunctor<T>::type, T>::type type;
    };

    template<typename T>
    struct _FunctionHelper_impl
    {

    };

    template<typename TR, typename ...TArgs>
    struct _FunctionHelper_impl<TR(*)(TArgs...)>
    {
        typedef TR returnType;
        typedef ArgsWrapper<TArgs...> argsType;
        typedef TDNE classType;
    };

    template<typename TR, typename TC, typename ...TArgs>
    struct _FunctionHelper_impl < TR(TC::*)(TArgs...)>
    {
        typedef TR returnType;
        typedef ArgsWrapper<TArgs...> argsType;
        typedef TC classType;
    };

    template<typename T>
    struct _FunctionHelper
    {
        typedef typename RemoveFuncConst<typename _CallableType<typename std::decay<T>::type>::type>::type callableType;

        typedef typename _FunctionHelper_impl<callableType>::returnType returnType;
        typedef typename _FunctionHelper_impl<callableType>::argsType argsType;
        typedef typename _FunctionHelper_impl<callableType>::classType classType;
    };

    template<typename T>
    struct IsCallable : std::integral_constant<bool, std::is_function<T>::value || std::is_member_function_pointer<T>::value || IsFunctor<T>::value>
    {
        
    };

    template<typename TFunc, typename TArgs>
    struct _CheckInvocable_impl
    {
        static_assert(IsCallable<TFunc>::value, "function passed in is not callable");
        static_assert(!std::is_member_function_pointer<typename RemoveFuncConst<typename std::decay<TFunc>::type>::type>::value, "couldn't invoke member function");
        typedef typename _FunctionHelper<TFunc>::argsType paramsType;
        typedef typename TransToArgsWrapper<TArgs>::type argsType;
        static_assert(paramsType::elemNum == argsType::elemNum, "the number of function parameters is not the same as the argument passed in");
        typedef typename _StaticZip_impl<argsType, paramsType, _IsConvertible, ArgsWrapper<>>::type zipType;
        static_assert(_StaticReduce_impl<StaticAnd, std::true_type, zipType>::type::value, "args passed in are not compatible with func parameters");
    };

    template<typename TFunc, typename TArgs>
    size_t CheckInvocable()
    {
        return sizeof(_CheckInvocable_impl<TFunc, TArgs>);
    };

    template<typename TFunc, typename ...TArgs>
    typename _FunctionHelper<TFunc>::returnType invoke_impl(TFunc&& _func, TArgs&&... _args)
    {
        return _func(std::forward<TArgs>(_args)...);
    }

    template<typename TFunc, typename ...TArgs>
    typename _FunctionHelper<TFunc>::returnType invoke(TFunc&& _func, TArgs&&... _args)
    {
        CheckInvocable<TFunc, std::tuple<TArgs...>>();
        return invoke_impl(std::forward<TFunc>(_func), std::forward<TArgs>(_args)...);
    }

    template<typename ...TArgs, typename TFunc, size_t ...TIdx>
    typename _FunctionHelper<TFunc>::returnType apply_impl (ArgsWrapper<TArgs...>&& _args, TFunc&& _func, IntegerSequence<TIdx...>&)
    {
        return FPInCpp::invoke(std::forward<TFunc>(_func), _args.get<TIdx>()...);
    }

    template<typename TFunc, size_t ...TIdx>
    typename _FunctionHelper<TFunc>::returnType apply_impl(ArgsWrapper<>&& _args, TFunc&& _func, IntegerSequence<TIdx...>&)
    {
        return FPInCpp::invoke(std::forward<TFunc>(_func));
    }

    template<typename TArgs, typename TFunc>
    typename _FunctionHelper<TFunc>::returnType apply(TArgs&& _args, TFunc&& _func)
    {
        typedef typename std::decay<TArgs>::type decayedType;
        CheckInvocable<TFunc, decayedType>();
        
        static const size_t _maxIdx = (_TupleHelper<decayedType>::size - 1) < 0 ? 0 : (_TupleHelper<decayedType>::size - 1);
        return apply_impl(make_ArgsWrapper(std::forward<TArgs>(_args)), std::forward<TFunc>(_func), typename IndexSequence<_maxIdx>::type{});
    }

    template<typename THead, typename ...TTails>
    struct _HeadOf
    {
        typedef THead type;
    };

    template<typename T>
    struct _RemoveContext
    {
        typedef T type;
    };

    template<template<typename...> class TContainer, typename... TArgs>
    struct _RemoveContext<TContainer<TArgs...>>
    {
        typedef typename ArgsWrapper<TArgs...>::type type;
    };

    template<typename TContainer, typename TFunc>
    std::vector<typename _FunctionHelper<TFunc>::returnType> map(TContainer&& _args, TFunc&& _func)
    {
        typedef typename std::decay<TContainer>::type decayedType;
        CheckInvocable<TFunc, std::tuple<typename decayedType::value_type>>();

        std::vector<typename _FunctionHelper<TFunc>::returnType> r;
        for (auto& it : _args)
        {
            r.push_back(FPInCpp::invoke(_func, it));
        }
        return r;
    }

    template<typename TContainer, typename TDefault, typename TFunc>
    typename std::decay<TDefault>::type reduce(TContainer&& _container, TDefault&& _acc, TFunc&& _func)
    {
        typedef typename std::decay<TContainer>::type decayedType;
        CheckInvocable<TFunc, std::tuple<typename std::decay<TDefault>::type, typename decayedType::value_type>>();

        TDefault acc(_acc);

        for (TContainer::iterator it = _container.begin(); it != _container.end(); it++)
        {
            acc = FPInCpp::invoke(_func, acc, *it);
        }

        return move(acc);
    }

    template<typename TContainer, typename TFunc>
    std::vector<typename std::decay<TContainer>::type::value_type> filter(TContainer&& _args, TFunc&& _func)
    {
        typedef typename std::decay<TContainer>::type decayedType;
        CheckInvocable<TFunc, std::tuple<typename decayedType::value_type>>();

        std::vector<typename decayedType::value_type> rst;

        for (auto& it : _args)
        {
            if (FPInCpp::invoke(_func, it))
            {
                rst.push_back(it);
            }
        }

        return move(rst);
    }

    template<typename TContainer, typename TFunc>
    std::vector<typename _FunctionHelper<TFunc>::returnType::value_type> flatMap(TContainer&& _container, TFunc&& _func)
    {
        typedef typename std::decay<TContainer>::type decayedType;
        CheckInvocable<TFunc, std::tuple<typename decayedType::value_type>>();

        std::vector<typename _FunctionHelper<TFunc>::returnType::value_type> rst;

        typename _FunctionHelper<TFunc>::returnType innerCont;

        for (auto& it : _container)
        {
            innerCont = FPInCpp::invoke(_func, it);

            std::move(innerCont.begin(), innerCont.end(), std::back_inserter(rst));
        }

        return rst;
    }

    template<typename TFstFunc, typename TScdFunc, size_t ...TIdx>
    auto compose_impl_impl(TFstFunc&& _fstFunc, TScdFunc&& _scdFunc, IntegerSequence<TIdx...>)
    {
        typedef typename _FunctionHelper<TFstFunc>::argsType argsType;
        typedef typename _FunctionHelper<TScdFunc>::returnType returnType;

        function<returnType(typename argsType::elemType<TIdx>::type...)> r = [_fstFunc, _scdFunc](typename argsType::elemType<TIdx>::type&&... _args) mutable
        {
            return FPInCpp::invoke(_scdFunc, FPInCpp::invoke(_fstFunc, std::forward<typename argsType::elemType<TIdx>::type>(_args)...));
        };

        return r;
    }

    template<typename TFstFunc, typename TScdFunc>
    auto compose_impl_impl(TFstFunc&& _fstFunc, TScdFunc&& _scdFunc)
    {
        typedef typename _FunctionHelper<TScdFunc>::returnType returnType;

        function<returnType()> r = [_fstFunc, _scdFunc]() mutable
        {
            return FPInCpp::invoke(_scdFunc, FPInCpp::invoke(_fstFunc));
        };

        return r;
    }

    template<typename TFstFunc, typename TScdFunc, typename ...TArgs>
    auto compose_impl(TFstFunc&& _fstFunc, TScdFunc&& _scdFunc, ArgsWrapper<TArgs...>*)
    {
        static const size_t _maxIdx = _FunctionHelper<TFstFunc>::argsType::elemNum - 1;

        return compose_impl_impl(std::forward<TFstFunc>(_fstFunc), std::forward<TScdFunc>(_scdFunc), typename IndexSequence<_maxIdx>::type{});
    }

    template<typename TFstFunc, typename TScdFunc>
    auto compose_impl(TFstFunc&& _fstFunc, TScdFunc&& _scdFunc, ArgsWrapper<>*)
    {
        return compose_impl_impl(std::forward<TFstFunc>(_fstFunc), std::forward<TScdFunc>(_scdFunc));
    }

    template<typename TFstFunc, typename TScdFunc>
    auto compose(TFstFunc&& _fstFunc, TScdFunc&& _scdFunc)
    {
        CheckInvocable<TScdFunc, std::tuple<typename _FunctionHelper<TFstFunc>::returnType>>();

        return compose_impl(std::forward<TFstFunc>(_fstFunc), std::forward<TScdFunc>(_scdFunc), (_FunctionHelper<TFstFunc>::argsType*)nullptr);
    }

    template<typename TLCont, typename TRCont, typename TFunc>
    std::vector<typename _FunctionHelper<TFunc>::returnType> zip(TLCont&& _leftCont, TRCont&& _rightCont, TFunc&& _func)
    {
        CheckInvocable<TFunc, std::tuple<typename TLCont::value_type, typename TRCont::value_type>>();

        assert(_leftCont.size() == _rightCont.size());

        std::vector<typename _FunctionHelper<TFunc>::returnType> rst;

        auto itL = _leftCont.begin();
        auto itR = _rightCont.begin();

        while(itL != _leftCont.end())
        {
            rst.push_back(FPInCpp::invoke(_func, *itL, *itR));
            itL++;
            itR++;
        }

        return rst;
    }

    template<typename TRst, typename TElem>
    TRst operator >> (TElem _elem, std::function<TRst(TElem)> _func)
    {
        return move(_func(_elem));
    }
}