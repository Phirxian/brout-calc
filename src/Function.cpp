#include "Function.h"
#include "Token.h"
#include <iostream>

namespace math
{
    Function::Function(Context *c, const std::vector<std::string> &&param, const std::string &expr, bool rec)
        : ctx(c), param(param), expr(expr), recursive(rec)
    {
    }
    
    void Function::release()
    {
        for(size_t i = 0u; i<param.size(); ++i)
            ctx->front()->unset(param[i]);
            
        if(recursive)
        {
            auto *call = (Function*)mem.prototype("endCall");
            if(call) call->release();
            else throw std::runtime_error("endCall terminal check not found");
        }
    }
    
    void Function::setParameter(const std::vector<float> &values)
    {
        if(values.size() != param.size())
        {
            std::string error("function call use ");
                error += std::to_string(param.size());
                error += " arguments but only ";
                error += std::to_string(values.size());
                error += " detected";
            throw std::runtime_error(error);
        }
        
        release();
        for(size_t i = 0u; i<param.size(); ++i)
            ctx->front()->setVariable(param[i], values[i]);
            
        if(recursive)
        {
            auto *call = (Function*)mem.prototype("endCall");
            if(call) call->setParameter(values);
            else throw std::runtime_error("endCall terminal check not found");
        }
    }
    
    void Function::print(std::ostream& out) const noexcept
    {
        out << expr;
    }
    
    float Function::eval() const // throw(std::runtime_error)
    { 
        constexpr float inf = std::numeric_limits<float>::infinity();
        constexpr float nan = std::numeric_limits<float>::quiet_NaN();
                
        if(recursive)
        {
            auto *ptr = const_cast<Function*>(this);
            auto *ctx = static_cast<Context*>(ptr->ctx);
            auto *call = static_cast<Function*>(ptr->mem.prototype("endCall"));
            
            if(call)
            {
                Context::guard g(ctx, &ptr->mem, recursive);
                float endCall = call->eval();
                
                if(endCall == nan || endCall == -nan)
                    return 0.0f;
                    
                if(endCall != inf)
                    return endCall;
            }
            else throw std::runtime_error("endCall terminal check not found");
        }
        
        return Expr(new math::Token{ctx, expr}).eval();
    }
    
    Context *Function::getContext() const
    {
        return ctx;
    }
    
    Memory *Function::getMemory()
    {
        return &mem;
    }
    
    /****************************/
    
    ExprCall::ExprCall(Function *f, const warper::string_view &i) noexcept
        : func(f), data(i)
    {
    }
    
    void ExprCall::print(std::ostream& out) const noexcept
    {
        out << "{";
        func->print(out);
        out << "}";
    }
    
    float ExprCall::eval() const // throw(std::runtime_error)
    {
        std::vector<float> values;
        auto param = split(data, ',');
        
        auto *ctx = func->getContext();
        auto *mem = func->getMemory();
        
        // func->isRecursive(), seem to create a new empty memory each time would work
        Context::guard g(ctx, mem, func->depth == 0);
        
        values.resize(param.size());
        
        #pragma omp parallel for
        for(unsigned i = 0; i<param.size(); ++i)
        {
            auto value = Expr(new Token(ctx, param[i])).eval();
            values[i] = value;
        }
        
        func->setParameter(values);
        func->depth++;
        
        if(func->depth > 30)
            throw std::runtime_error("stackoverflow");
        
        float val = func->eval();
        
        func->depth = 0;
        return val;
    }
    
    Function *ExprCall::getFunction() const
    {
        return func;
    }
}