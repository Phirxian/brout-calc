#ifndef __TOKEN_HEADER__
#define __TOKEN_HEADER__

#include "Context.h"
#include <vector>

namespace math
{
    class Token
    {
        public:
            Token(Context*, std::string&, bool hasComa = false);
            Token(Context*, const warper::string_view&);
            const std::vector<ExprToken*>& split() const noexcept;
            Context *getContext() const;
        private:
            void loopcut(const warper::string_view&);
        private:
            Context *m;
            std::vector<ExprToken*> token;
    };
    
    warper::string_view parenthesis_block(const warper::string_view &p);
    std::vector<std::string> split(const std::string&, char delim);
    std::vector<warper::string_view> split(const warper::string_view&, char delim);
}

#endif