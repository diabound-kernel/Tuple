#pragma once

#include <functional>
#include <utility>

namespace nonstd {

template<typename... Args>
struct Tuple
{
    constexpr Tuple() = default;
};

template<typename Arg, typename... Args>
struct Tuple<Arg, Args...> : Tuple<Args...>
{
    explicit constexpr Tuple(Arg &&arg, Args &&...args) :
        Tuple<Args...>(std::forward<Args>(args)...),
        data(std::forward<Arg>(arg))
    {}

    Arg data{};
};

template<typename... Args>
decltype(auto) makeTuple(Args &&...args)
{
    return Tuple<std::unwrap_ref_decay_t<Args>...>(std::forward<Args>(args)...);
}

namespace detail {

template<typename Tuple, typename Func, std::size_t... I>
constexpr decltype(auto)
applyImpl(Tuple &&tuple, Func &&func, std::index_sequence<I...>)
{
    return std::invoke(
        std::forward<Func>(func), std::get<I>(std::forward<Tuple>(tuple))...);
}

}  // namespace detail

template<typename Tuple, typename Func, std::size_t... I>
constexpr auto apply(Tuple &&tuple, Func &&func)
{
    return detail::applyImpl(
        std::forward<Tuple>(tuple),
        std::forward<Func>(func),
        std::make_index_sequence<
            std::tuple_size_v<std::remove_reference_t<Tuple>>>{});
}

}  // namespace nonstd
