#pragma once

#include "Function.h"
#include <iostream>
#include <algorithm>
#include <assert.h>

namespace math
{
    class VariadicFunction : public Function
    {
        public:
            VariadicFunction(Context *c, const std::vector<std::string> &&param, const std::string &def, const std::string &close,
                              std::function<std::string(float val, size_t index)> exprBuilder)
                : Function(c, std::move(param), ""), def(def), close(close), exprBuilder(exprBuilder)
            {
                assert(
                    std::count_if(
                        param.begin(), param.end(), 
                        [](const std::string &i) {
                            return i == "...";
                        }
                    )
                    <= 1
                );
            }
            
            void setParameter(const std::vector<float> &values)
            {
                expr = def;
                
                if(values.size() < param.size()-1)
                {
                    std::string error("function call use ");
                        error += std::to_string(param.size()-1);
                        error += " arguments but only ";
                        error += std::to_string(values.size());
                        error += " detected";
                    throw std::runtime_error(error);
                }
                
                size_t i = 0u, j = 0u;
                
                for(; i<param.size() && param[i] != "..."; ++i)
                    mem.setVariable(param[i], values[i]);
                    
                for(j = i; j<values.size()-(param.size()-i-1); ++j)
                    expr += exprBuilder(values[j], j);
                expr += close;
                i++;
                
                std::cout << expr << std::endl;
                
                for(size_t u = i; u<param.size(); ++u)
                    mem.setVariable(param[u], values[u+j-i]);
            }
        private:
            std::string def, close;
            std::function<std::string(float val, size_t index)> exprBuilder;
    };
}