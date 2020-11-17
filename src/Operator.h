#ifndef __OPERATOR_HEADER__
#define __OPERATOR_HEADER__

#include "math.h"
#include "Expr.h"

#include <iostream>

namespace math
{
    class ExprNumber : public ExprToken
    {
        public:
            ExprNumber(float i) noexcept : data(i) {}
            void print(std::ostream& out) const noexcept { out << data; }
            virtual int priority() const noexcept { return 0; }
            virtual warper::EXPR_TYPE getType() const noexcept { return warper::EXPR_TYPE::EXT_NUMBER; }
            float eval() const noexcept { return data; }
        protected:
            float data;
    };
    
    template<typename ops, int p, int i>
    class ExprBinary : public ExprToken
    {
        public:
            static const constexpr std::string* operator_name = warper::binary_operators_name;
            static constexpr int _priority = p;
            static constexpr int _index = i;
            using operators = ops;
        public:
            ExprBinary(ExprToken *l, ExprToken *r) noexcept
                : left(l), right(r)
            {
            }
            ~ExprBinary() noexcept
            {
                delete left,
                delete right;
            }
            virtual int priority() const noexcept
            {
                return p;
            }
            virtual warper::EXPR_TYPE getType() const noexcept
            {
                return warper::EXPR_TYPE::EXT_BINARY_OPS;
            }
            void print(std::ostream& out) const noexcept
            {
                out << "(";
                left->print(out);
                out << warper::binary_operators_name[i];
                right->print(out);
                out << ")";
            }
            float eval() const noexcept
            {
                return ops()(left->eval(), right->eval());
            }
        protected:
            ExprToken *left;
            ExprToken *right;
    };
    
    template<typename ops, int p, int i>
    class ExprUnary : public ExprToken
    {
        public:
            static const constexpr std::string* operator_name = warper::unary_operators_name;
            static constexpr int _priority = p;
            static constexpr int _index = i;
            using operators = ops;
        public:
            ExprUnary(ExprToken *d) noexcept
                : data(d)
            {
            }
            ~ExprUnary() noexcept
            {
                delete data;
            }
            virtual int priority() const noexcept
            {
                return p;
            }
            virtual warper::EXPR_TYPE getType() const noexcept
            {
                return warper::EXPR_TYPE::EXT_UNARY_OPS;
            }
            void print(std::ostream& out) const noexcept
            {
                out << warper::unary_operators_name[i] << "(";
                data->print(out);
                out << ")";
            }
            float eval() const noexcept
            {
                return ops()(data->eval());
            }
        protected:
            ExprToken *data;
    };
    
    using ExprAdd = ExprBinary<warper::plus<float>,       0,  0>;
    using ExprSub = ExprBinary<warper::minus<float>,      0,  1>;
    using ExprMul = ExprBinary<warper::multiplies<float>, 1,  2>;
    using ExprDiv = ExprBinary<warper::divides<float>,    1,  3>;
    using ExprPow = ExprBinary<warper::pow<float>,        2,  4>;
    using ExprMax = ExprBinary<warper::max<float>,        3,  5>;
    using ExprMin = ExprBinary<warper::min<float>,        3,  6>;
    using ExprAnd = ExprBinary<warper::logical_and<float>,3,  7>;
    using ExprOr  = ExprBinary<warper::logical_or<float>, 3,  8>;
    using ExprMod = ExprBinary<warper::mod<float>,        1,  9>;
    using ExprRem = ExprBinary<warper::remainder<float>,  1, 10>;
    using ExprLSF = ExprBinary<warper::rshift<float>,     5, 11>;
    using ExprRSF = ExprBinary<warper::lshift<float>,     5, 12>;
    using ExprSup = ExprBinary<warper::greater<float>,    5, 13>;
    using ExprInf = ExprBinary<warper::lower<float>,      5, 14>;
    using ExprEq  = ExprBinary<warper::equals<float>,     5, 15>;
    
    using ExprCos = ExprUnary<warper::cos<float>,   9, 0>;
    using ExprSin = ExprUnary<warper::sin<float>,   9, 1>;
    using ExprTan = ExprUnary<warper::tan<float>,   9, 2>;
    using ExprExp = ExprUnary<warper::exp<float>,   9, 3>;
    using ExprLog = ExprUnary<warper::log<float>,   9, 4>;
    
    using ExprSqrt = ExprUnary<warper::sqrt<float>, 9, 5>;
    using ExprACos = ExprUnary<warper::acos<float>, 9, 6>;
    using ExprASin = ExprUnary<warper::asin<float>, 9, 7>;
    using ExprATan = ExprUnary<warper::atan<float>, 9, 8>;
    using ExprFact = ExprUnary<warper::fact<float>, 9, 9>;
    
    using ExprFib = ExprUnary<warper::fib<float>, 9, 10>;
    using ExprNeg = ExprUnary<warper::neg<float>, 9, 11>;
    using ExprAbs = ExprUnary<warper::abs<float>, 9, 12>;
    
    using ExprFloor = ExprUnary<warper::floor<float>, 9, 13>;
    using ExprCeil  = ExprUnary<warper::ceil<float>,  9, 14>;
    using ExprRound = ExprUnary<warper::round<float>, 9, 15>;
    using ExprGamma = ExprUnary<warper::gamma<float>, 9, 16>;
    using ExprLGamma = ExprUnary<warper::lgamma<float>, 9, 17>;
    using ExprFint  = ExprUnary<warper::fint<float>, 9, 18>;
    using ExprFract = ExprUnary<warper::fract<float>, 9, 19>;
    using ExprInv   = ExprUnary<warper::inv<float>, 9, 20>;
}

#endif
