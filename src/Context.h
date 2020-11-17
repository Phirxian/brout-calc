#pragma once

#include "Memory.h"
#include "Expr.h"
#include <deque>

namespace math
{
    class UnaryBuilder : public ExprToken
    {
        public:
            virtual ExprToken* create(ExprToken*) noexcept = 0;
            virtual int priority() const noexcept = 0;
            virtual float eval() const noexcept { return 0.f; }
    };
    
    class BinaryBuilder : public ExprToken
    {
        public:
            virtual ExprToken* create(ExprToken*,ExprToken*) noexcept = 0;
            virtual int priority() const noexcept = 0;
            virtual float eval() const noexcept { return 0.f; }
    };
    
    class Context : public Memory
    {
        public:
            struct guard
            {
                guard(Context *c, Memory *m, bool rec = false)
                    : ctx(c), recursiv(rec), mem(m)
                {
                    mem = recursiv ? mem->clone() : mem;
                    ctx->push(mem);
                }
                
                ~guard()
                {
                    ctx->pop();
                    if(recursiv)
                        delete mem;
                }
                
                bool recursiv;
                Context *ctx;
                Memory *mem;
            };
        public:
            Context() noexcept;
            ~Context() noexcept;
            
            UnaryBuilder* unary(const warper::string_view&);
            BinaryBuilder* binary(const warper::string_view&);
            
            void push(Memory *m);
            void pop();
            
            inline Memory* front() { return stack.front(); }
            
            void addPrototype(const std::string&, ExprToken*);
            ExprToken* prototype(const std::string&);
            
            void setVariable(const std::string &name, float val);
            ExprToken* getVariable(const std::string &name);
        private:
            std::map<warper::string_view, ExprToken*> single;
            std::map<warper::string_view, ExprToken*> dual;
            std::deque<Memory*> stack;
    };
}