//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

// UNSUPPORTED: c++03, c++11, c++14, c++17

// template<class T, class U>
// concept regular_invocable;

#include <concepts>
#include <cstddef>
#include <functional>
#include <memory>
#include <type_traits>

template <class R, class... Args>
constexpr bool check_invocable() {
  constexpr bool result = std::regular_invocable<R(Args...), Args...>;
  static_assert(std::regular_invocable<R(Args...) noexcept, Args...> == result);
  static_assert(std::regular_invocable<R (*)(Args...), Args...> == result);
  static_assert(std::regular_invocable<R (*)(Args...) noexcept, Args...> ==
                result);
  static_assert(std::regular_invocable<R (&)(Args...), Args...> == result);
  static_assert(std::regular_invocable<R (&)(Args...) noexcept, Args...> ==
                result);

  return result;
}

static_assert(check_invocable<void>());
static_assert(check_invocable<void, int>());
static_assert(check_invocable<void, int&>());
static_assert(check_invocable<void, int*, double>());
static_assert(check_invocable<int>());
static_assert(check_invocable<int, int[]>());

struct S;
static_assert(check_invocable<int, int S::*, std::nullptr_t>());
static_assert(check_invocable<int, int (S::*)(), int (S::*)(int), int>());
static_assert(std::regular_invocable<void (*)(int const&), int&>);
static_assert(std::regular_invocable<void (*)(int const&), int&&>);
static_assert(std::regular_invocable<void (*)(int volatile&), int&>);
static_assert(std::regular_invocable<void (*)(int const volatile&), int&>);

static_assert(!std::regular_invocable<void(), int>);
static_assert(!std::regular_invocable<void(int)>);
static_assert(!std::regular_invocable<void(int*), double*>);
static_assert(!std::regular_invocable<void (*)(int&), double*>);
static_assert(std::regular_invocable<int S::*, std::unique_ptr<S> >);
static_assert(std::regular_invocable<int S::*, std::shared_ptr<S> >);
static_assert(!std::regular_invocable<void (*)(int&&), int&>);
static_assert(!std::regular_invocable<void (*)(int&&), int const&>);

static_assert(!std::regular_invocable<void>);
static_assert(!std::regular_invocable<void*>);
static_assert(!std::regular_invocable<int>);
static_assert(!std::regular_invocable<int&>);
static_assert(!std::regular_invocable<int&&>);

namespace function_objects {
struct function_object {
  void operator()();
};
static_assert(std::regular_invocable<function_object>);
static_assert(!std::regular_invocable<function_object const>);
static_assert(!std::regular_invocable<function_object volatile>);
static_assert(!std::regular_invocable<function_object const volatile>);
static_assert(std::regular_invocable<function_object&>);
static_assert(!std::regular_invocable<function_object const&>);
static_assert(!std::regular_invocable<function_object volatile&>);
static_assert(!std::regular_invocable<function_object const volatile&>);

struct const_function_object {
  void operator()(int) const;
};
static_assert(std::regular_invocable<const_function_object, int>);
static_assert(std::regular_invocable<const_function_object const, int>);
static_assert(!std::regular_invocable<const_function_object volatile, int>);
static_assert(
    !std::regular_invocable<const_function_object const volatile, int>);
static_assert(std::regular_invocable<const_function_object&, int>);
static_assert(std::regular_invocable<const_function_object const&, int>);
static_assert(!std::regular_invocable<const_function_object volatile&, int>);
static_assert(
    !std::regular_invocable<const_function_object const volatile&, int>);

struct volatile_function_object {
  void operator()(int, int) volatile;
};
static_assert(std::regular_invocable<volatile_function_object, int, int>);
static_assert(
    !std::regular_invocable<volatile_function_object const, int, int>);
static_assert(
    std::regular_invocable<volatile_function_object volatile, int, int>);
static_assert(
    !std::regular_invocable<volatile_function_object const volatile, int, int>);
static_assert(std::regular_invocable<volatile_function_object&, int, int>);
static_assert(
    !std::regular_invocable<volatile_function_object const&, int, int>);
static_assert(
    std::regular_invocable<volatile_function_object volatile&, int, int>);
static_assert(!std::regular_invocable<volatile_function_object const volatile&,
                                      int, int>);

struct cv_function_object {
  void operator()(int[]) const volatile;
};
static_assert(std::regular_invocable<cv_function_object, int*>);
static_assert(std::regular_invocable<cv_function_object const, int*>);
static_assert(std::regular_invocable<cv_function_object volatile, int*>);
static_assert(std::regular_invocable<cv_function_object const volatile, int*>);
static_assert(std::regular_invocable<cv_function_object&, int*>);
static_assert(std::regular_invocable<cv_function_object const&, int*>);
static_assert(std::regular_invocable<cv_function_object volatile&, int*>);
static_assert(std::regular_invocable<cv_function_object const volatile&, int*>);

struct lvalue_function_object {
  void operator()() &;
};
static_assert(!std::regular_invocable<lvalue_function_object>);
static_assert(!std::regular_invocable<lvalue_function_object const>);
static_assert(!std::regular_invocable<lvalue_function_object volatile>);
static_assert(!std::regular_invocable<lvalue_function_object const volatile>);
static_assert(std::regular_invocable<lvalue_function_object&>);
static_assert(!std::regular_invocable<lvalue_function_object const&>);
static_assert(!std::regular_invocable<lvalue_function_object volatile&>);
static_assert(!std::regular_invocable<lvalue_function_object const volatile&>);

struct lvalue_const_function_object {
  void operator()(int) const&;
};
static_assert(std::regular_invocable<lvalue_const_function_object, int>);
static_assert(std::regular_invocable<lvalue_const_function_object const, int>);
static_assert(
    !std::regular_invocable<lvalue_const_function_object volatile, int>);
static_assert(
    !std::regular_invocable<lvalue_const_function_object const volatile, int>);
static_assert(std::regular_invocable<lvalue_const_function_object&, int>);
static_assert(std::regular_invocable<lvalue_const_function_object const&, int>);
static_assert(
    !std::regular_invocable<lvalue_const_function_object volatile&, int>);
static_assert(
    !std::regular_invocable<lvalue_const_function_object const volatile&, int>);

struct lvalue_volatile_function_object {
  void operator()(int, int) volatile&;
};
static_assert(
    !std::regular_invocable<lvalue_volatile_function_object, int, int>);
static_assert(
    !std::regular_invocable<lvalue_volatile_function_object const, int, int>);
static_assert(!std::regular_invocable<lvalue_volatile_function_object volatile,
                                      int, int>);
static_assert(!std::regular_invocable<
              lvalue_volatile_function_object const volatile, int, int>);
static_assert(
    std::regular_invocable<lvalue_volatile_function_object&, int, int>);
static_assert(
    !std::regular_invocable<lvalue_volatile_function_object const&, int, int>);
static_assert(std::regular_invocable<lvalue_volatile_function_object volatile&,
                                     int, int>);
static_assert(!std::regular_invocable<
              lvalue_volatile_function_object const volatile&, int, int>);

struct lvalue_cv_function_object {
  void operator()(int[]) const volatile&;
};
static_assert(!std::regular_invocable<lvalue_cv_function_object, int*>);
static_assert(!std::regular_invocable<lvalue_cv_function_object const, int*>);
static_assert(
    !std::regular_invocable<lvalue_cv_function_object volatile, int*>);
static_assert(
    !std::regular_invocable<lvalue_cv_function_object const volatile, int*>);
static_assert(std::regular_invocable<lvalue_cv_function_object&, int*>);
static_assert(std::regular_invocable<lvalue_cv_function_object const&, int*>);
static_assert(
    std::regular_invocable<lvalue_cv_function_object volatile&, int*>);
static_assert(
    std::regular_invocable<lvalue_cv_function_object const volatile&, int*>);
//
struct rvalue_function_object {
  void operator()() &&;
};
static_assert(std::regular_invocable<rvalue_function_object>);
static_assert(!std::regular_invocable<rvalue_function_object const>);
static_assert(!std::regular_invocable<rvalue_function_object volatile>);
static_assert(!std::regular_invocable<rvalue_function_object const volatile>);
static_assert(!std::regular_invocable<rvalue_function_object&>);
static_assert(!std::regular_invocable<rvalue_function_object const&>);
static_assert(!std::regular_invocable<rvalue_function_object volatile&>);
static_assert(!std::regular_invocable<rvalue_function_object const volatile&>);

struct rvalue_const_function_object {
  void operator()(int) const&&;
};
static_assert(std::regular_invocable<rvalue_const_function_object, int>);
static_assert(std::regular_invocable<rvalue_const_function_object const, int>);
static_assert(
    !std::regular_invocable<rvalue_const_function_object volatile, int>);
static_assert(
    !std::regular_invocable<rvalue_const_function_object const volatile, int>);
static_assert(!std::regular_invocable<rvalue_const_function_object&, int>);
static_assert(
    !std::regular_invocable<rvalue_const_function_object const&, int>);
static_assert(
    !std::regular_invocable<rvalue_const_function_object volatile&, int>);
static_assert(
    !std::regular_invocable<rvalue_const_function_object const volatile&, int>);

struct rvalue_volatile_function_object {
  void operator()(int, int) volatile&&;
};
static_assert(
    std::regular_invocable<rvalue_volatile_function_object, int, int>);
static_assert(
    !std::regular_invocable<rvalue_volatile_function_object const, int, int>);
static_assert(
    std::regular_invocable<rvalue_volatile_function_object volatile, int, int>);
static_assert(!std::regular_invocable<
              rvalue_volatile_function_object const volatile, int, int>);
static_assert(
    !std::regular_invocable<rvalue_volatile_function_object&, int, int>);
static_assert(
    !std::regular_invocable<rvalue_volatile_function_object const&, int, int>);
static_assert(!std::regular_invocable<rvalue_volatile_function_object volatile&,
                                      int, int>);
static_assert(!std::regular_invocable<
              rvalue_volatile_function_object const volatile&, int, int>);

struct rvalue_cv_function_object {
  void operator()(int[]) const volatile&&;
};
static_assert(std::regular_invocable<rvalue_cv_function_object, int*>);
static_assert(std::regular_invocable<rvalue_cv_function_object const, int*>);
static_assert(std::regular_invocable<rvalue_cv_function_object volatile, int*>);
static_assert(
    std::regular_invocable<rvalue_cv_function_object const volatile, int*>);
static_assert(!std::regular_invocable<rvalue_cv_function_object&, int*>);
static_assert(!std::regular_invocable<rvalue_cv_function_object const&, int*>);
static_assert(
    !std::regular_invocable<rvalue_cv_function_object volatile&, int*>);
static_assert(
    !std::regular_invocable<rvalue_cv_function_object const volatile&, int*>);

struct multiple_overloads {
  struct A {};
  struct B { B(int); };
  struct AB : A, B {};
  struct O {};
  void operator()(A) const;
  void operator()(B) const;
};
static_assert(std::regular_invocable<multiple_overloads, multiple_overloads::A>);
static_assert(std::regular_invocable<multiple_overloads, multiple_overloads::B>);
static_assert(std::regular_invocable<multiple_overloads, int>);
static_assert(!std::regular_invocable<multiple_overloads, multiple_overloads::AB>);
static_assert(!std::regular_invocable<multiple_overloads, multiple_overloads::O>);
} // namespace function_objects

namespace pointer_to_member_functions {
  template<class Member, class T, class... Args>
  constexpr bool check_member_is_invocable()
  {
    constexpr bool result = std::regular_invocable<Member, T&&, Args...>;
    using uncv_t = std::remove_cvref_t<T>;
    static_assert(std::regular_invocable<Member, uncv_t*, Args...> == result);
    static_assert(std::regular_invocable<Member, std::unique_ptr<uncv_t>, Args...> == result);
    static_assert(std::regular_invocable<Member, std::reference_wrapper<uncv_t>, Args...> == result);
    static_assert(!std::regular_invocable<Member, std::nullptr_t, Args...>);
    static_assert(!std::regular_invocable<Member, int, Args...>);
    static_assert(!std::regular_invocable<Member, int*, Args...>);
    static_assert(!std::regular_invocable<Member, double*, Args...>);
    struct S2 {};
    static_assert(!std::regular_invocable<Member, S2*, Args...>);
    return result;
  }

static_assert(check_member_is_invocable<int S::*, S>());
static_assert(std::regular_invocable<int S::*, S&>);
static_assert(std::regular_invocable<int S::*, S const&>);
static_assert(std::regular_invocable<int S::*, S volatile&>);
static_assert(std::regular_invocable<int S::*, S const volatile&>);
static_assert(std::regular_invocable<int S::*, S&&>);
static_assert(std::regular_invocable<int S::*, S const&&>);
static_assert(std::regular_invocable<int S::*, S volatile&&>);
static_assert(std::regular_invocable<int S::*, S const volatile&&>);

static_assert(check_member_is_invocable<int (S::*)(int), S, int>());
static_assert(!check_member_is_invocable<int (S::*)(int), S>());
using unqualified = void (S::*)();
static_assert(std::regular_invocable<unqualified, S&>);
static_assert(!std::regular_invocable<unqualified, S const&>);
static_assert(!std::regular_invocable<unqualified, S volatile&>);
static_assert(!std::regular_invocable<unqualified, S const volatile&>);
static_assert(std::regular_invocable<unqualified, S&&>);
static_assert(!std::regular_invocable<unqualified, S const&&>);
static_assert(!std::regular_invocable<unqualified, S volatile&&>);
static_assert(!std::regular_invocable<unqualified, S const volatile&&>);

static_assert(check_member_is_invocable<int (S::*)(double) const, S, double>());
using const_qualified = void (S::*)() const;
static_assert(std::regular_invocable<const_qualified, S&>);
static_assert(std::regular_invocable<const_qualified, S const&>);
static_assert(!std::regular_invocable<const_qualified, S volatile&>);
static_assert(!std::regular_invocable<const_qualified, S const volatile&>);
static_assert(std::regular_invocable<const_qualified, S&&>);
static_assert(std::regular_invocable<const_qualified, S const&&>);
static_assert(!std::regular_invocable<const_qualified, S volatile&&>);
static_assert(!std::regular_invocable<const_qualified, S const volatile&&>);

static_assert(
    check_member_is_invocable<int (S::*)(double[]) volatile, S, double*>());
using volatile_qualified = void (S::*)() volatile;
static_assert(std::regular_invocable<volatile_qualified, S&>);
static_assert(!std::regular_invocable<volatile_qualified, S const&>);
static_assert(std::regular_invocable<volatile_qualified, S volatile&>);
static_assert(!std::regular_invocable<volatile_qualified, S const volatile&>);
static_assert(std::regular_invocable<volatile_qualified, S&&>);
static_assert(!std::regular_invocable<volatile_qualified, S const&&>);
static_assert(std::regular_invocable<volatile_qualified, S volatile&&>);
static_assert(!std::regular_invocable<volatile_qualified, S const volatile&&>);

static_assert(check_member_is_invocable<int (S::*)(int, S&) const volatile, S,
                                        int, S&>());
using cv_qualified = void (S::*)() const volatile;
static_assert(std::regular_invocable<cv_qualified, S&>);
static_assert(std::regular_invocable<cv_qualified, S const&>);
static_assert(std::regular_invocable<cv_qualified, S volatile&>);
static_assert(std::regular_invocable<cv_qualified, S const volatile&>);
static_assert(std::regular_invocable<cv_qualified, S&&>);
static_assert(std::regular_invocable<cv_qualified, S const&&>);
static_assert(std::regular_invocable<cv_qualified, S volatile&&>);
static_assert(std::regular_invocable<cv_qualified, S const volatile&&>);

static_assert(check_member_is_invocable<int (S::*)() &, S&>());
using lvalue_qualified = void (S::*)() &;
static_assert(std::regular_invocable<lvalue_qualified, S&>);
static_assert(!std::regular_invocable<lvalue_qualified, S const&>);
static_assert(!std::regular_invocable<lvalue_qualified, S volatile&>);
static_assert(!std::regular_invocable<lvalue_qualified, S const volatile&>);
static_assert(!std::regular_invocable<lvalue_qualified, S&&>);
static_assert(!std::regular_invocable<lvalue_qualified, S const&&>);
static_assert(!std::regular_invocable<lvalue_qualified, S volatile&&>);
static_assert(!std::regular_invocable<lvalue_qualified, S const volatile&&>);

static_assert(check_member_is_invocable<int (S::*)() const&, S>());
using lvalue_const_qualified = void (S::*)() const&;
static_assert(std::regular_invocable<lvalue_const_qualified, S&>);
static_assert(std::regular_invocable<lvalue_const_qualified, S const&>);
static_assert(!std::regular_invocable<lvalue_const_qualified, S volatile&>);
static_assert(
    !std::regular_invocable<lvalue_const_qualified, S const volatile&>);
static_assert(std::regular_invocable<lvalue_const_qualified, S&&>);
static_assert(std::regular_invocable<lvalue_const_qualified, S const&&>);
static_assert(!std::regular_invocable<lvalue_const_qualified, S volatile&&>);
static_assert(
    !std::regular_invocable<lvalue_const_qualified, S const volatile&&>);

static_assert(check_member_is_invocable<int (S::*)() volatile&, S&>());
using lvalue_volatile_qualified = void (S::*)() volatile&;
static_assert(std::regular_invocable<lvalue_volatile_qualified, S&>);
static_assert(!std::regular_invocable<lvalue_volatile_qualified, S const&>);
static_assert(std::regular_invocable<lvalue_volatile_qualified, S volatile&>);
static_assert(
    !std::regular_invocable<lvalue_volatile_qualified, S const volatile&>);
static_assert(!std::regular_invocable<lvalue_volatile_qualified, S&&>);
static_assert(!std::regular_invocable<lvalue_volatile_qualified, S const&&>);
static_assert(!std::regular_invocable<lvalue_volatile_qualified, S volatile&&>);
static_assert(
    !std::regular_invocable<lvalue_volatile_qualified, S const volatile&&>);

static_assert(check_member_is_invocable<int (S::*)() const volatile&, S&>());
using lvalue_cv_qualified = void (S::*)() const volatile&;
static_assert(std::regular_invocable<lvalue_cv_qualified, S&>);
static_assert(std::regular_invocable<lvalue_cv_qualified, S const&>);
static_assert(std::regular_invocable<lvalue_cv_qualified, S volatile&>);
static_assert(std::regular_invocable<lvalue_cv_qualified, S const volatile&>);
static_assert(!std::regular_invocable<lvalue_cv_qualified, S&&>);
static_assert(!std::regular_invocable<lvalue_cv_qualified, S const&&>);
static_assert(!std::regular_invocable<lvalue_cv_qualified, S volatile&&>);
static_assert(!std::regular_invocable<lvalue_cv_qualified, S const volatile&&>);

using rvalue_unqualified = void (S::*)() &&;
static_assert(!std::regular_invocable<rvalue_unqualified, S&>);
static_assert(!std::regular_invocable<rvalue_unqualified, S const&>);
static_assert(!std::regular_invocable<rvalue_unqualified, S volatile&>);
static_assert(!std::regular_invocable<rvalue_unqualified, S const volatile&>);
static_assert(std::regular_invocable<rvalue_unqualified, S&&>);
static_assert(!std::regular_invocable<rvalue_unqualified, S const&&>);
static_assert(!std::regular_invocable<rvalue_unqualified, S volatile&&>);
static_assert(!std::regular_invocable<rvalue_unqualified, S const volatile&&>);

using rvalue_const_unqualified = void (S::*)() const&&;
static_assert(!std::regular_invocable<rvalue_const_unqualified, S&>);
static_assert(!std::regular_invocable<rvalue_const_unqualified, S const&>);
static_assert(!std::regular_invocable<rvalue_const_unqualified, S volatile&>);
static_assert(
    !std::regular_invocable<rvalue_const_unqualified, S const volatile&>);
static_assert(std::regular_invocable<rvalue_const_unqualified, S&&>);
static_assert(std::regular_invocable<rvalue_const_unqualified, S const&&>);
static_assert(!std::regular_invocable<rvalue_const_unqualified, S volatile&&>);
static_assert(
    !std::regular_invocable<rvalue_const_unqualified, S const volatile&&>);

using rvalue_volatile_unqualified = void (S::*)() volatile&&;
static_assert(!std::regular_invocable<rvalue_volatile_unqualified, S&>);
static_assert(!std::regular_invocable<rvalue_volatile_unqualified, S const&>);
static_assert(
    !std::regular_invocable<rvalue_volatile_unqualified, S volatile&>);
static_assert(
    !std::regular_invocable<rvalue_volatile_unqualified, S const volatile&>);
static_assert(std::regular_invocable<rvalue_volatile_unqualified, S&&>);
static_assert(!std::regular_invocable<rvalue_volatile_unqualified, S const&&>);
static_assert(
    std::regular_invocable<rvalue_volatile_unqualified, S volatile&&>);
static_assert(
    !std::regular_invocable<rvalue_volatile_unqualified, S const volatile&&>);

using rvalue_cv_unqualified = void (S::*)() const volatile&&;
static_assert(!std::regular_invocable<rvalue_cv_unqualified, S&>);
static_assert(!std::regular_invocable<rvalue_cv_unqualified, S const&>);
static_assert(!std::regular_invocable<rvalue_cv_unqualified, S volatile&>);
static_assert(
    !std::regular_invocable<rvalue_cv_unqualified, S const volatile&>);
static_assert(std::regular_invocable<rvalue_cv_unqualified, S&&>);
static_assert(std::regular_invocable<rvalue_cv_unqualified, S const&&>);
static_assert(std::regular_invocable<rvalue_cv_unqualified, S volatile&&>);
static_assert(
    std::regular_invocable<rvalue_cv_unqualified, S const volatile&&>);
} // namespace pointer_to_member_functions

// Check the concept with closure types (and also check for subsumption)
template<class F, class... Args>
constexpr bool is_regular_invocable(F, Args&&...) {
  return false;
}

template<class F, class... Args>
requires std::invocable<F, Args...>
constexpr bool is_regular_invocable(F, Args&&...) {
  return false;
}

template<class F, class... Args>
requires std::regular_invocable<F, Args...> && true
constexpr bool is_regular_invocable(F, Args&&...) {
  return true;
}

static_assert(is_regular_invocable([] {}));
static_assert(is_regular_invocable([](int) {}, 0));
static_assert(is_regular_invocable([](int) {}, 0L));
static_assert(!is_regular_invocable([](int) {}, nullptr));

int i = 0;
static_assert(is_regular_invocable([](int&) {}, i));
