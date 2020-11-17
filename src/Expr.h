#ifndef __EXPR_HEADER__
#define __EXPR_HEADER__

#include "math.h"
#include <ostream>

namespace math
{
    class Token;
    class ExprToken
    {
        public:
            virtual warper::EXPR_TYPE getType() const noexcept = 0;
            virtual void print(std::ostream& out) const noexcept = 0;
            virtual int priority() const noexcept { return 0; }
            virtual float eval() const = 0; // throw(std::runtime_error) = 0;
    };
    class Expr : public ExprToken
    {
        public:
            Expr(const Token*);
            ~Expr() noexcept;
            
            virtual int priority() const noexcept { return 0; }
            warper::EXPR_TYPE getType() const noexcept { return warper::EXPR_TYPE::EXT_SUB_EXPR; }
            
            void print(std::ostream& out) const noexcept;
            float eval() const; // throw(std::runtime_error);
        protected:
            const Token *data;
            ExprToken *root;
    };
}

#endif
