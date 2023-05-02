#pragma once

#include <functional>
#include <utility>

namespace nonstd {

namespace detail {

template<typename... Types>
struct TypeList
{};

template<typename _TpyeList>
struct popFront
{};

template<
    template<typename...>
    /* maybe class instead of typename ?*/
    typename _TypeList,
    typename Type,
    typename... Types>
struct popFront<_TypeList<Type, Types...>>
{
    using type = _TypeList<Types...>;
};

}  // namespace detail

template<typename... Types>
struct Tuple
{
    constexpr Tuple() = default;
};

template<>
struct Tuple<>
{};

template<typename Type, typename... Types>
struct Tuple<Type, Types...> : Tuple<Types...>
{
    explicit constexpr Tuple(Type &&arg, Types &&...args) :
        Tuple<Types...>(std::forward<Types>(args)...),
        data(std::forward<Type>(arg))
    {}

    Type data{};
};

template<typename Type, typename... Types>
Tuple(Type &&, Types &&...)
    -> Tuple<std::unwrap_ref_decay_t<Type>, std::unwrap_ref_decay_t<Types>...>;

template<typename... Types>
decltype(auto) makeTuple(Types &&...args)
{
    return Tuple<std::unwrap_ref_decay_t<Types>...>(
        std::forward<Types>(args)...);
}

namespace detail {

/*
 * get<2>(tuple) =>
 * index = 2 Tuple<int, bool, float>
 * index = 1 Tuple<bool,float>
 * index = 0 Tuple<float>
 */

template<std::size_t index, typename Tuple>
struct getImpl : getImpl<index - 1, typename popFront<Tuple>::type>
{};

template<typename Tuple>
struct getImpl<0, Tuple>
{
    template<typename T>
    static constexpr decltype(auto) get(T &&tuple)
    {
        return static_cast<Tuple>(std::forward<Tuple>(tuple)).data;
    }
};

}  // namespace detail

template<std::size_t index, typename Tuple>
constexpr decltype(auto) get(Tuple &&tuple)
{
    return detail::getImpl<index, std::remove_cvref_t<Tuple>>::get(
        std::forward<Tuple>(tuple));
}

template<typename Tuple>
struct tupleSize
{};

template<typename... Types>
struct tupleSize<nonstd::Tuple<Types...>>
{
    static constexpr auto value = sizeof...(Types);
};

namespace detail {

template<typename Tuple, typename Func, std::size_t... I>
constexpr decltype(auto)
applyImpl(Tuple &&tuple, Func &&func, std::index_sequence<I...>)
{
    return std::invoke(
        std::forward<Func>(func),
        nonstd::get<I>(std::forward<Tuple>(tuple))...);
}

}  // namespace detail

template<typename Tuple, typename Func, std::size_t... I>
constexpr decltype(auto) apply(Tuple &&tuple, Func &&func)
{
    return detail::applyImpl(
        std::forward<Tuple>(tuple),
        std::forward<Func>(func),
        std::make_index_sequence<
            nonstd::tupleSize<std::remove_reference_t<Tuple>>::value>{});
}

}  // namespace nonstd