#pragma once

#include "math.h"
#include "Context.h"
#include <vector>

namespace math
{
    /*
        using Function need to bind his memory to the current context
        for evaluation and/or setParameter
        auto *ctx = func->getContext();
        auto *mem = func->getMemory();
        Context::guard g(ctx, mem);
    */
    class Function : public ExprToken
    {
        public:
            Function(Context *c, const std::vector<std::string> &&param, const std::string &expr, bool rec = false);
            virtual warper::EXPR_TYPE getType() const noexcept { return warper::EXPR_TYPE::EXT_FUNCTION_PTR; };
            
            virtual void setParameter(const std::vector<float> &values);
            virtual int priority() const noexcept { return 10; }
            
            Context* getContext() const;
            Memory* getMemory();
            
            inline bool isRecursive() { return recursive; }
            void release();
            
            virtual void print(std::ostream& out) const noexcept;
            virtual float eval() const; // throw(std::runtime_error);
            
            int depth;
        protected:
            bool recursive;
            std::string expr;
            std::vector<std::string> param;
            Context *ctx;
            Memory mem;
    };
    
    class ExprCall : public ExprToken
    {
        public:
            ExprCall(Function *f, const warper::string_view &i) noexcept;
            
            virtual int priority() const noexcept { return 10; }
            virtual warper::EXPR_TYPE getType() const noexcept { return warper::EXPR_TYPE::EXT_FUNCTION_CALL; }
            
            void print(std::ostream& out) const noexcept;
            float eval() const; // throw(std::runtime_error);
            
            Function *getFunction() const;
        protected:
            Function *func;
            warper::string_view data;
    };
}