#include "Context.h"
#include "Expr.h"
#include "Operator.h"

#include "VariadicFunction.h"

namespace math
{
    template<typename T>
    class UnaryBuilderWarper : public UnaryBuilder
    {
        public:
            virtual warper::EXPR_TYPE getType() const noexcept
            {
                return warper::EXPR_TYPE::EXT_UNARY_BUILDER;
            }
            virtual void print(std::ostream& out) const noexcept
            {
                out << "UnaryBuilderWarper@" << this;
            }
            virtual int priority() const noexcept
            {
                return T::_priority;
            }
            virtual ExprToken* create(ExprToken *i) noexcept
            {
                return new T(i);
            }
    };

    template<typename T>
    class BinaryBuilderWarper : public BinaryBuilder
    {
        public:
            virtual warper::EXPR_TYPE getType() const noexcept
            {
                return warper::EXPR_TYPE::EXT_BINARY_BUILDER;
            }
            virtual void print(std::ostream& out) const noexcept
            {
                out << "BinaryBuilderWarper@" << this;
            }
            virtual int priority() const noexcept
            {
                return T::_priority;
            }
            virtual ExprToken* create(ExprToken *a, ExprToken *b) noexcept
            {
                return new T(a, b);
            }
    };
    
    template<template<class> class B>
    inline constexpr void impOperator(std::map<warper::string_view, ExprToken*> &store) {
    }
    
    template<template<class> class B, typename T>
    inline constexpr void impOperator(std::map<warper::string_view, ExprToken*> &store) {
        store[T::operator_name[T::_index]] = new B<T>();
    }
    
    template<template<class> class B, typename T, typename Y, typename ...Args>
    inline constexpr void impOperator(std::map<warper::string_view, ExprToken*> &store) {
        store[T::operator_name[T::_index]] = new B<T>();
        store[Y::operator_name[Y::_index]] = new B<Y>();
        impOperator<B, Args...>(store);
    }
        
    Context::Context()  noexcept
    {
        impOperator<
            BinaryBuilderWarper,
            ExprAdd, ExprSub, ExprMul, ExprDiv, 
            ExprPow, ExprMax, ExprMin, ExprAnd,
            ExprOr,  ExprMod, ExprRem, ExprRSF,
            ExprLSF, ExprInf, ExprSup, ExprEq
        >(dual);
        
        impOperator<
            UnaryBuilderWarper,
            ExprCos,   ExprSin,   ExprTan,
            ExprExp,   ExprLog,   ExprFib,
            ExprACos,  ExprASin,  ExprATan,
            ExprSqrt,  ExprFact,  ExprNeg,
            ExprAbs,   ExprFloor, ExprCeil,
            ExprRound, ExprGamma, ExprLGamma,
            ExprFint,  ExprFract, ExprInv
        >(single);
        
        for(auto &it : warper::const_varable_name)
            setVariable(it.first, it.second);
        
        // for performance prefer a compiled implenentation of a function
        // this function definition must be used for execution purpose
        
        addPrototype("logb",  new Function{this, {"x", "y"}, "log(x)/log(y)"});
        addPrototype("pow",   new Function{this, {"x", "y"}, "x^y"});
        addPrototype("hypot", new Function{this, {"x", "y"}, "sqrt(x*x+y*y)"});
        addPrototype("lerp",  new Function{this, {"b", "e", "t"}, "(1-t)*e+t*b"});
        
        addPrototype(
            "sum",
            new VariadicFunction{this,
                {"..."}, "0", "",
                [](float v, size_t j) -> std::string {
                    std::string str = "+";
                    str += std::to_string(v);
                    str.erase(str.find_last_not_of('0')+1, std::string::npos);
                    str.erase(str.find_last_not_of('.')+1, std::string::npos);
                    return str;
                }
            }
        );
        
        addPrototype(
            "prod",
            new VariadicFunction{this,
                {"..."}, "1",  "",
                [](float v, size_t j) -> std::string {
                    std::string str = "*";
                    str += std::to_string(v);
                    str.erase(str.find_last_not_of('0')+1, std::string::npos);
                    str.erase(str.find_last_not_of('.')+1, std::string::npos);
                    return str;
                }
            }
        );
        
        /*
        for this I need a to change float to a class variable to implement type
        addPrototype(
            "transform",
            new VariadicFunction{this,
                {"f, ..."}, "",  ")",
                [](float v, size_t j) -> std::string {
                    std::string str = "f(";
                    str += std::to_string(v);
                    str.erase(str.find_last_not_of('0')+1, std::string::npos);
                    str.erase(str.find_last_not_of('.')+1, std::string::npos);
                    return str;
                }
            }
        );
        */
        
        addPrototype(
            "polynome",
            new VariadicFunction{this,
                {"b", "...", "x"}, "0", "",
                [](float v, size_t j) -> std::string {
                    std::string str = "+";
                    
                    str += std::to_string(v);
                    str.erase(str.find_last_not_of('0')+1, std::string::npos);
                    str.erase(str.find_last_not_of('.')+1, std::string::npos);
                    
                    str += "*x^";
                    str += std::to_string(j);
                    
                    return str;
                }
            }
        );
        
        //! bla bla bla etc
    }
    
    Context::~Context() noexcept
    {
        for(auto const& it : dual)
            delete it.second;
        for(auto const& it : single)
            delete it.second;
    }
    
    UnaryBuilder* Context::unary(const warper::string_view &i)
    {
        return static_cast<UnaryBuilder*>(single[i]);
    }
    
    BinaryBuilder* Context::binary(const warper::string_view &i)
    {
        return static_cast<BinaryBuilder*>(dual[i]);
    }
    
    void Context::push(Memory *m)
    {
        stack.push_front(m);
    }
    
    void Context::pop()
    {
        stack.pop_front();
    }
    
    void Context::addPrototype(const std::string &name, ExprToken *val)
    {
        if(stack.size())
            stack[0]->addPrototype(name, val);
        else
            Memory::addPrototype(name, val);
    }
    
    ExprToken* Context::prototype(const std::string &name)
    {
        for(auto const& it : stack)
        {
            auto *tmp = it->prototype(name);
            if(tmp) return tmp;
            /*
            try { return it->prototype(name); }
            catch(std::runtime_error) { }
            */
        }
        return Memory::prototype(name);
    }
    
    void Context::setVariable(const std::string &name, float val)
    {
        if(single[name] || dual[name])
            throw std::runtime_error("trying to erase operator \""+name+"\" aborded");
            
        if(stack.size())
            stack[0]->setVariable(name, val);
        else
            Memory::setVariable(name, val);
    }
    
    ExprToken* Context::getVariable(const std::string &name)
    {
        for(auto const& it : stack)
        {
            try { return it->getVariable(name); }
            catch(std::runtime_error) { }
        }
        return Memory::getVariable(name);
    }
}