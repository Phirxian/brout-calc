#include "Memory.h"
#include "Operator.h"
#include "Function.h"

#include <stdexcept>
#include <assert.h>

namespace math
{
    Memory::~Memory()
    {
        for(auto const& it : func)
            delete it.second;
    }
    void Memory::addPrototype(const std::string &name, ExprToken *f)
    {
        assert(
            f->getType() == warper::EXPR_TYPE::EXT_FUNCTION_PTR &&
            "addPrototype must take an EXT_FUNCTION_CALL"
        );
        func[name] = f;
    }
    ExprToken* Memory::prototype(const std::string &i)
    {
        //if(func.find(i) == func.end())
        //    throw std::runtime_error("unknow function \""+i+"\"");
        return func[i];
    }
    void Memory::setVariable(const std::string &i, float j)
    {
        mem[i] = j;
    }
    ExprToken* Memory::getVariable(const std::string &i)
    {
        if(mem.find(i) == mem.end())
            throw std::runtime_error("unknow variable \""+i+"\"");
        return new ExprNumber(mem[i]);
    }
    void Memory::unset(const std::string &name)
    {
        mem.erase(name);
    }
    Memory* Memory::clone()
    {
        Memory *c = new Memory();
        
        // seem to be not necessary ???
        
        //for(auto const &it : func)
            //std::cout << it.first << std::endl;
            //c->addPrototype(it.first, it.second);
        
        //for(auto const &it : mem)
            //std::cout << it.first << std::endl;
            //c->setVariable(it.first, it.second);
            
        return c;
    }
}
