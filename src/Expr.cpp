#include "Expr.h"
#include "Token.h"
#include "Function.h"
#include "Operator.h"

#include <limits>
#include <deque>
#include <stack>

namespace math
{
    Expr::Expr(const Token *str)
        : data(str), root(nullptr)
    {
        const auto &token = str->split();
        std::deque<ExprToken*> out;
        std::stack<ExprToken*> main;
        
        //! do not delete all ptr of UnaryBuilder/BinaryBuilder
        //! because some are shared with @Memory and it's at the same @
        
        constexpr auto freeToken = [](ExprToken *it){
            if(it->getType() != warper::EXPR_TYPE::EXT_UNARY_BUILDER &&
               it->getType() != warper::EXPR_TYPE::EXT_BINARY_BUILDER &&
               it->getType() != warper::EXPR_TYPE::EXT_FUNCTION_PTR)
                delete it;
        };
        
        for(auto &it : token)
        {
            switch(it->getType())
            {
                case warper::EXPR_TYPE::EXT_FUNCTION_PTR:
                    throw std::runtime_error("function ptr found not supported");
                break;
                case warper::EXPR_TYPE::EXT_SUB_EXPR:
                case warper::EXPR_TYPE::EXT_NUMBER:
                case warper::EXPR_TYPE::EXT_UNARY_OPS:
                case warper::EXPR_TYPE::EXT_BINARY_OPS:
                    // pre-compiled
                    out.push_back(it);
                break;
                case warper::EXPR_TYPE::EXT_FUNCTION_CALL:
                    ((ExprCall*)it)->getFunction()->depth = 0;
                    out.push_back(it);
                break;
                case warper::EXPR_TYPE::EXT_UNARY_BUILDER:
                case warper::EXPR_TYPE::EXT_BINARY_BUILDER:
                    while(!main.empty() && main.top()->priority() > it->priority())
                    {
                        out.push_back(main.top());
                        main.pop();
                    }
                    main.push(it);
                break;
            }
        }
        
        while(!main.empty())
        {
            out.push_back(main.top());
            main.pop();
        }
        
        for(unsigned i = 0; i<out.size();)
        {
            switch(out[i]->getType())
            {
                case warper::EXPR_TYPE::EXT_SUB_EXPR:
                case warper::EXPR_TYPE::EXT_NUMBER:
                case warper::EXPR_TYPE::EXT_UNARY_OPS:
                case warper::EXPR_TYPE::EXT_BINARY_OPS:
                case warper::EXPR_TYPE::EXT_FUNCTION_CALL:
                    ++i;
                break;
                case warper::EXPR_TYPE::EXT_UNARY_BUILDER:
                {
                    if(i < 1)
                    {
                        for(auto &it : out) freeToken(it);
                        throw std::runtime_error("bad expression : unary over zero value");
                    }
                    
                    auto tmp = static_cast<UnaryBuilder*>(out[i]);
                    out[i-1] = tmp->create(out[i-1]);
                    out.erase(out.begin()+i); // O(1)
                    // do not delete @tmp it's shared with @Memory
                    i--;
                }
                break;
                case warper::EXPR_TYPE::EXT_BINARY_BUILDER:
                {
                    if(i < 2)
                    {
                        for(auto &it : out) freeToken(it);
                        throw std::runtime_error("bad expression : binary over zero/one value");
                    }
                    
                    auto tmp = static_cast<BinaryBuilder*>(out[i]);
                    out[i-2] = tmp->create(out[i-2], out[i-1]);
                    out.erase(out.begin()+i);   // O(2)
                    out.erase(out.begin()+i-1); // O(1)
                    // do not delete @tmp it's shared with @Memory
                    i-=2;
                }
                break;
            }
        }
            
        if(out.size() != 1)
        {
            for(auto &it : out) freeToken(it);
            throw std::runtime_error("bad expression : too less binary operators");
        }
                        
        // compiled expr through a tree ;)
        root = out[0];
    }
    Expr::~Expr() noexcept
    {
        delete root;
        delete data;
    }
    void Expr::print(std::ostream& out) const noexcept
    {
        if(root) root->print(out);
        else out << eval();
    }
    float Expr::eval() const // throw(std::runtime_error)
    {
        return root ? root->eval() :
            std::numeric_limits<float>::quiet_NaN();
    }
}
