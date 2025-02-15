// -*- C++ -*-
//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef _LIBCPP___RANGES_RANGE_ADAPTOR_H
#define _LIBCPP___RANGES_RANGE_ADAPTOR_H

#include <__config>
#include <__functional/compose.h>
#include <__functional/invoke.h>
#include <__ranges/concepts.h>
#include <__utility/forward.h>
#include <__utility/move.h>
#include <concepts>
#include <type_traits>

#if !defined(_LIBCPP_HAS_NO_PRAGMA_SYSTEM_HEADER)
#  pragma GCC system_header
#  pragma clang include_instead(<ranges>)
#endif

_LIBCPP_BEGIN_NAMESPACE_STD

#if !defined(_LIBCPP_HAS_NO_CONCEPTS) && !defined(_LIBCPP_HAS_NO_INCOMPLETE_RANGES)

// CRTP base that one can derive from in order to be considered a range adaptor closure
// by the library. When deriving from this class, a pipe operator will be provided to
// make the following hold:
// - `x | f` is equivalent to `f(x)`
// - `f1 | f2` is an adaptor closure `g` such that `g(x)` is equivalent to `f2(f1(x))`
template <class _Tp>
struct __range_adaptor_closure;

// Type that wraps an arbitrary function object and makes it into a range adaptor closure,
// i.e. something that can be called via the `x | f` notation.
template <class _Fn>
struct __range_adaptor_closure_t : _Fn, __range_adaptor_closure<__range_adaptor_closure_t<_Fn>> {
    constexpr explicit __range_adaptor_closure_t(_Fn&& __f) : _Fn(std::move(__f)) { }
};

template <class _Tp>
concept _RangeAdaptorClosure = derived_from<remove_cvref_t<_Tp>, __range_adaptor_closure<remove_cvref_t<_Tp>>>;

template <class _Tp>
struct __range_adaptor_closure {
    template <ranges::viewable_range _View, _RangeAdaptorClosure _Closure>
        requires same_as<_Tp, remove_cvref_t<_Closure>> &&
                 invocable<_Closure, _View>
    [[nodiscard]] _LIBCPP_HIDE_FROM_ABI
    friend constexpr decltype(auto) operator|(_View&& __view, _Closure&& __closure)
        noexcept(is_nothrow_invocable_v<_Closure, _View>)
    { return std::invoke(std::forward<_Closure>(__closure), std::forward<_View>(__view)); }

    template <_RangeAdaptorClosure _Closure, _RangeAdaptorClosure _OtherClosure>
        requires same_as<_Tp, remove_cvref_t<_Closure>> &&
                 constructible_from<decay_t<_Closure>, _Closure> &&
                 constructible_from<decay_t<_OtherClosure>, _OtherClosure>
    [[nodiscard]] _LIBCPP_HIDE_FROM_ABI
    friend constexpr auto operator|(_Closure&& __c1, _OtherClosure&& __c2)
        noexcept(is_nothrow_constructible_v<decay_t<_Closure>, _Closure> &&
                 is_nothrow_constructible_v<decay_t<_OtherClosure>, _OtherClosure>)
    { return __range_adaptor_closure_t(std::__compose(std::forward<_OtherClosure>(__c2), std::forward<_Closure>(__c1))); }
};

#endif // !defined(_LIBCPP_HAS_NO_CONCEPTS) && !defined(_LIBCPP_HAS_NO_INCOMPLETE_RANGES)

_LIBCPP_END_NAMESPACE_STD

#endif // _LIBCPP___RANGES_RANGE_ADAPTOR_H
