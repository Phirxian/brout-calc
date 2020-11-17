#include "Token.h"
#include "Expr.h"
#include "math.h"

#include <iostream>
#include <algorithm>
#include <cctype>
#include <limits>
#include <stack>
#include <utility>

#include "Operator.h"
#include "Function.h"
#include <sstream>

namespace math
{
    warper::string_view parenthesis_block(const warper::string_view &p)
    {
        const char *ptr = p.data();
        unsigned int pos = 0;
        int count = 0;
        
        do
        {
            count += *ptr == '(';
            count -= *ptr == ')';
            *ptr++;
            pos++;
        }
        while(count > 0 && pos < p.size());
        
        if(count != 0 /*|| ptr - p.data() <= 2*/)
            throw std::runtime_error("bad expression : wrong parenthesis number");
        
        return {p.data()+1, size_t(ptr - p.data())-2};
    }
    
    std::vector<std::string> split(const std::string &data, char delim)
    {
        std::vector<std::string> buf;
        std::istringstream line(data);
        for(std::string str; std::getline(line, str, delim);)
            if(str.size() > 0)
                buf.push_back(std::move(str));
        return std::move(buf);
    }
    
    std::vector<warper::string_view> split(const warper::string_view &data, char delim)
    {
        std::vector<warper::string_view> buf;
        
        auto i = 0;
        auto pos = data.find(delim);
        
        while(pos != std::string::npos)
        {
            if(pos-i > 0)
                buf.push_back(data.substr(i, pos-i));
            i = ++pos;
            pos = data.find(delim, pos);
        }
        
        if(data.length()-i > 0)
            buf.push_back(data.substr(i, data.length()));
        
        return std::move(buf);
    }
    
    inline std::pair<ExprToken*, int> searchOperator(Context *m, const char *data)
    {
        for(auto it : warper::binary_operators_name)
        {
            warper::string_view v{data, it.size()};
            if(it == v) return std::make_pair(m->binary({data, it.size()}), it.size());
        }
        
        for(auto it : warper::unary_operators_name)
        {
            warper::string_view v{data, it.size()};
            if(it == v) return std::make_pair(m->unary({data, it.size()}), it.size());
        }
        
        return std::make_pair(nullptr, 0);
    }

    Token::Token(Context *_m, std::string &data, bool hasComa) : m(_m)
    {
        // this contructor would be used only by the first pass
        data.erase(
            std::remove_if(
                data.begin(), data.end(),
                (int(*)(int))isspace
            ),
            data.end()
        );
        loopcut({&data[0], data.size()-hasComa});
    }
    
    Token::Token(Context *_m, const warper::string_view &data) : m(_m)
    {
        loopcut(data);
    }
    
    void Token::loopcut(const warper::string_view &data)
    {
        for(unsigned int i = 0 ; i<data.size(); ++i)
        {
            if(data[i] == '(')
            {
                auto tmp = parenthesis_block({&data[i], data.size() - i});
                if(tmp.size() == 0)
                    throw std::runtime_error("bad expression : wrong parenthesis number");
                token.push_back(new Expr(new Token{m, tmp}));
                i += tmp.size()+1; // skip ')'
                continue;
            }
            
            if(isdigit(data[i]))
            {
                unsigned int j = i;
                while(isdigit(data[j])) j++;
                if(data[j] == '.')
                {
                    j++;
                    while(isdigit(data[j])) j++;
                }
                token.push_back(new ExprNumber(std::atof(&data[i])));
                i = j-1;
                continue;
            }
            
            if(isalpha(data[i]))
            {
                unsigned int j = i;
                while(isalpha(data[j])) j++;
                warper::string_view tmp{&data[i], j-i};
                
                auto *uops = m->unary(tmp);
                auto *bops = m->binary(tmp);
                auto *fops = m->prototype(tmp.to_string());
                
                if(uops)
                    token.push_back(uops);
                else if(bops)
                    token.push_back(bops);
                else if(fops)
                {
                    i = j;
                    
                    if(data[i] == '(')
                    {
                        warper::string_view tmp = parenthesis_block({&data[i], data.size() - i});
                        token.push_back(new ExprCall(static_cast<Function*>(fops), tmp));
                        i += tmp.size()+1; // skip ')'
                        continue;
                    }
                    else
                        token.push_back(m->getVariable(tmp.to_string()));
                        //throw std::runtime_error("functions args must be in parenthesis block");
                }
                else
                    token.push_back(m->getVariable(tmp.to_string()));
                    
                i = j-1;
                continue;
            }
            else
            {
                auto found = searchOperator(m, &data[i]);
            
                if(!found.first)
                {
                    std::string tmp("bad character at ");
                        tmp += std::to_string(i+1);
                        tmp += "@";
                        tmp += data[i];
                        tmp += " :: ";
                        tmp += data.to_string();
                    throw std::runtime_error(tmp);
                }
                else
                {
                    token.push_back(found.first);
                    i += found.second-1;
                }
            }
        }
    }
    
    Context *Token::getContext() const
    {
        return m;
    }
    
    const std::vector<ExprToken*>& Token::split() const noexcept
    {
        return token;
    }
}