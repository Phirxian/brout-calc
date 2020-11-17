#ifndef __MATH_HEADER__
#define __MATH_HEADER__

#include <cmath>
#include <functional>
#include <experimental/string_view>
#include <utility>

namespace warper
{
    using string_view = std::experimental::string_view;

    enum class EXPR_TYPE
    {
        EXT_SUB_EXPR,
        EXT_NUMBER,
        EXT_UNARY_OPS,
        EXT_BINARY_OPS,
        EXT_UNARY_BUILDER,
        EXT_BINARY_BUILDER,
        EXT_FUNCTION_PTR,
        EXT_FUNCTION_CALL,
        EXT_COUNT
    };
    
    static const std::pair<std::string, float> const_varable_name[] = {
        {"false", 0.f},
        {"true",  1.f},
        {"zero",  0.f},
        {"one",   1.f},
        {"pi",    3.14159265358979f},
        {"phi",   1.61803398874989f},
        {"e",     2.71828182845904f},
        {"inf",   1.f/0.f},
        {"nan",   0.f/0.f}
    };

    static const std::string unary_operators_name[] = {
        "cos", "sin", "tan", "exp", "log",
        "sqrt", "acos", "asin", "atan",
        "fact", "fib", "!", "abs", "floor",
        "ceil", "round", "gamma", "lgamma",
        "fint", "fract", "~"
    };
    
    static const std::string binary_operators_name[] = {
        "+", "-", "*", "/", "^",
        "><", "<>", "&", "|",
        "%", "\\", ">>", "<<",
        ">", "<", "::"
    };
    
    template <class T> struct pow : std::binary_function <T,T,T> {
        T operator() (const T& x, const T& y) const noexcept { return std::pow(x,y); }
    };
    template <class T> struct max : std::binary_function <T,T,T> {
        T operator() (const T& x, const T& y) const noexcept { return std::max(x,y); }
    };
    template <class T> struct min : std::binary_function <T,T,T> {
        T operator() (const T& x, const T& y) const noexcept { return std::min(x,y); }
    };
    template <class T> struct mod : std::binary_function <T,T,T> {
        T operator() (const T& x, const T& y) const noexcept { return std::fmod(x,y); }
    };
    template <class T> struct remainder : std::binary_function <T,T,T> {
        T operator() (const T& x, const T& y) const noexcept { return std::remainder(x,y); }
    };
    template <class T> struct lshift : std::binary_function <T,T,T> {
        T operator() (const T& x, const T& y) const noexcept { return int(x) >> int(y); }
    };
    template <class T> struct rshift : std::binary_function <T,T,T> {
        T operator() (const T& x, const T& y) const noexcept { return int(x) << int(y); }
    };
    template <class T> struct greater : std::binary_function <T,T,T> {
        T operator() (const T& x, const T& y) const noexcept { return x > y; }
    };
    template <class T> struct lower : std::binary_function <T,T,T> {
        T operator() (const T& x, const T& y) const noexcept { return x < y; }
    };
    template <class T> struct equals : std::binary_function <T,T,T> {
        T operator() (const T& x, const T& y) const noexcept { return x == y; }
    };
    
    using std::plus;
    using std::minus;
    using std::multiplies;
    using std::divides;
    using std::logical_or;
    using std::logical_and;
    
    template <class T> struct sqrt : std::unary_function <T,T> {
        T operator() (const T& x) const noexcept { return std::sqrt(x); }
    };
    template <class T> struct cos : std::unary_function <T,T> {
        T operator() (const T& x) const noexcept { return std::cos(x); }
    };
    template <class T> struct sin : std::unary_function <T,T> {
        T operator() (const T& x) const noexcept { return std::sin(x); }
    };
    template <class T> struct tan : std::unary_function <T,T> {
        T operator() (const T& x) const noexcept { return std::tan(x); }
    };
    template <class T> struct log : std::unary_function <T,T> {
        T operator() (const T& x) const noexcept { return std::log(x); }
    };
    template <class T> struct exp : std::unary_function <T,T> {
        T operator() (const T& x) const noexcept { return std::exp(x); }
    };
    template <class T> struct acos : std::unary_function <T,T> {
        T operator() (const T& x) const noexcept { return std::acos(x); }
    };
    template <class T> struct asin : std::unary_function <T,T> {
        T operator() (const T& x) const noexcept { return std::asin(x); }
    };
    template <class T> struct atan : std::unary_function <T,T> {
        T operator() (const T& x) const noexcept { return std::atan(x); }
    };
    template <class T> struct fact : std::unary_function <T,T> {
        T operator() (const T& x) const noexcept {
            T tmp = 1;
            for(auto i = 0; i<x; ++i)
                tmp *= (x-i);
            return tmp;
        }
    };
    template <class T> struct fib : std::unary_function <T,T> {
        T operator() (const T& x) const noexcept {
            static const double phi = (1 + std::sqrt(5))*0.5;
            double fib = (std::pow(phi,x) - std::pow(1-phi,x))/std::sqrt(5);
            return std::round(fib);
        }
    };
    template <class T> struct neg : std::unary_function <T,T> {
        T operator() (const T& x) const noexcept {
            return -x;
        }
    };
    
    template <class T> struct abs : std::unary_function <T,T> {
        T operator() (const T& x) const noexcept { return std::abs(x); }
    };
    template <class T> struct floor : std::unary_function <T,T> {
        T operator() (const T& x) const noexcept { return std::floor(x); }
    };
    template <class T> struct ceil : std::unary_function <T,T> {
        T operator() (const T& x) const noexcept { return std::ceil(x); }
    };
    template <class T> struct round : std::unary_function <T,T> {
        T operator() (const T& x) const noexcept { return std::round(x); }
    };
    template <class T> struct gamma : std::unary_function <T,T> {
        T operator() (const T& x) const noexcept { return std::tgamma(x); }
    };
    template <class T> struct lgamma : std::unary_function <T,T> {
        T operator() (const T& x) const noexcept { return std::lgamma(x); }
    };
    template <class T> struct fint : std::unary_function <T,T> {
        T operator() (const T& x) const noexcept {
            double param = x, fractpart, intpart;
            fractpart = std::modf(param , &intpart);
            return intpart;
        }
    };
    template <class T> struct fract : std::unary_function <T,T> {
        T operator() (const T& x) const noexcept {
            double param = x, fractpart, intpart;
            fractpart = std::modf(param , &intpart);
            return fractpart;
        }
    };
    template <class T> struct inv : std::unary_function <T,T> {
        T operator() (const T& x) const noexcept { return 1.f/x; }
    };
}

#endif