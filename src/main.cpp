#include <iostream>
#include <iomanip>
#include <sstream>
#include <regex>

#include "Function.h"
#include "Token.h"

/**
    TODO: Context can include Context
        so it may be usefull to add a syntax to switch to a new underlying Context like { } ?
        
        { // new contex
            a=4;
            b=log(a);
            ... bla bla
            result
        } // close context
        
    in this case function declaration might be
    
        f(x,y) = gamma(x)^log(b)
        
                OR
        
        g(nx, ny, gx, gy) = { // context call
            D=1
            D2=sqrt(2)
            delta(a,b) = abs(a - b)
            dx = delta(nx, gx)
            dy = delta(ny, gy)
            D * (dx + dy) + (D2 - 2 * D) * min(dx, dy)
        }
        
                OR
        
        g(nx, ny, gx, gy) = // considering empty expr as new ctx
        \t D=1
        \t D2=sqrt(2)
        \t delta(a,b) = abs(a - b)
        \t dx = delta(nx, gx)
        \t dy = delta(ny, gy)
        \t D * (dx + dy) + (D2 - 2 * D) * min(dx, dy)
        // no \t -> exit context
        
    considering the last expr as the final function result
    Todo: add operator "ans" to get the last result
 */
    
template<class T>
auto operator<<(std::ostream& os, const T& t) noexcept -> decltype(t.print(os), os)
{ 
    t.print(os); 
    return os; 
}

void help()
{
    std::cout << "Copyright © 2016-2017 Vayssade Jehan-Antoine zlib/libpng License" << std::endl
              << "this software provide an interface for basic mathematics expression" << std::endl
              << "you must write your expression in infixe and the evualuation would be printed in the next line" << std::endl
              << "otherwise an error is printed with small information about it" << std::endl
              << "please note that all evaluated string has there whitespace removed" << std::endl
              << std::endl;
              
    std::cout << "-this software inlude the following math function/operator:" << std::endl;
              
    std::cout << "    binary: ";
    for(auto &it : warper::binary_operators_name)
        std::cout << it << " ";
    std::cout << std::endl;
    
    std::cout << "    unary: ";
    for(auto &it : warper::unary_operators_name)
        std::cout << it << " ";
    std::cout << std::endl;
    
    std::cout << "    const: ";
    for(auto &it : warper::const_varable_name)
        std::cout << it.first << " ";
    std::cout << std::endl;
        
    std::cout << "-you could also add your own in runtime by the following syntaxe:" << std::endl
              << "    functionName(p1, p2, ...) = expr as like (logb, pow, hypot, lerp)" << std::endl
              << "    recursive function called are now supported make sur the terminal value can be reach" << std::endl
              << "    variadic function called are now supported (sum, prod, polynome)" << std::endl
              << "-you can define/erase variable by the following syntaxe:" << std::endl
              << "    varName = expr" << std::endl
              << "-you can access to the last result by the variable \"ans\"" << std::endl;
              
    std::cout << std::endl << std::endl;
}

void eval(math::Context *mem, std::string &str, bool hasComa = false)
{
    std::string var, param;
    int i = str.size()-1;
    std::size_t hasEq = str.find('=');
    std::size_t hasParam = std::string::npos;
    
    try
    {
        if(hasEq != std::string::npos)
        {
            var = str.substr(0, hasEq);
            hasParam = var.find('(');
            
            if(hasParam != std::string::npos)
            {
                std::size_t size = var.size()-hasParam;
                param = math::parenthesis_block(
                    warper::string_view{&var[hasParam], size}
                ).to_string();
                var.resize(hasParam);
            }
            
            std::regex rr("[[:alpha:]]+");
            if(!regex_match(var, rr))
                throw std::runtime_error("variable must be (a-Z)+");
                
            str = str.substr(hasEq+1, str.size()-hasEq-1);
        }
    
        if(hasParam != std::string::npos)
        {
            auto params = math::split(param, ',');
            
            bool recursive = str.find(var+"(") != std::string::npos;
            auto *fun = new math::Function(mem, std::move(params), str, recursive);
            mem->addPrototype(var, fun);
            
            if(recursive)
            {
                // little hack
                // add an internal function in the function memorie
                // called each time that return the terminal value or inf to continue
                // if we need to have the 0 as terminal value you can return -nan insted
                // for hacking the pseudo-if calculus
                
                std::string terminal = "5/(x<5)";
                std::cout << "recursive function depth test: ";
                std::cin >> terminal;
                
                fun->getMemory()->addPrototype("endCall",
                    new math::Function(mem, std::move(params), terminal)
                );
            }
        }
        else
        {
            math::Expr expression{new math::Token{mem, str}};
            
            #ifdef __DEBUG
                std::cout << "   " << expression << std::endl;
            #endif
            
            float eval = expression.eval();
            if(!hasComa) std::cout << std::setprecision(20) << eval << std::endl;
            if(hasEq != std::string::npos) mem->setVariable(var, eval);
            
            mem->setVariable("ans", eval);
        }
    }
    catch(std::runtime_error e)
    {
        std::cout << e.what() << std::endl;
    }
}

int main(int argc, const char *argv[])
{
    math::Context mem;
    
    for(int i = 1; i<argc; ++i)
        if(std::string("--help") == argv[i])
            help();

    #ifdef false//__DEBUG
        std::string str {argc > 1 ? argv[1] : "123 / 4 + 123 * 4 - 3"};
        std::cout << "<< " << str << std::endl;
        eval(&mem, str);
    #else
        std::string data;
        while(std::getline(std::cin, data))
        {
            data.erase(
                std::remove_if(
                    data.begin(), data.end(),
                    (int(*)(int))isspace
                ),
                data.end()
            );
            
            std::istringstream line(data);
            
            for(std::string str; std::getline(line, str, ';');)
            {
                line.unget();
                eval(&mem, str, line.get() == ';');
            }
        }
    #endif
}