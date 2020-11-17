#ifndef __MEMORY_HEADER__
#define __MEMORY_HEADER__

#include <string>
#include <map>

namespace math
{
    class ExprToken;
    class Memory
    {
        public:
            Memory() = default;
            ~Memory();
        
            void addPrototype(const std::string&, ExprToken*);
            ExprToken* prototype(const std::string&);
            
            void setVariable(const std::string&, float);
            ExprToken* getVariable(const std::string&);
            void unset(const std::string&);
            
            Memory* clone();
        private:
            std::map<std::string, ExprToken*> func;
            std::map<std::string, float> mem;
    };
    
}

#endif