//! @file   lexer.hpp
//!
//! @author Yue Wang
//! @date   26 Sep 2014
//! @note   exercises 2.6.1~3 have been nested in the code below.
//!
#ifndef LEXER_HPP
#define LEXER_HPP

#include "token.hpp"
#include <string>
#include <unordered_map>

namespace dragon {namespace ch2 {

/**
 * @brief   The Lexer class
 * @note    generics Lexer
 *          using iterator rather char as java version did
 */
template<typename Iter>
class Lexer
{
public:
    using HashTable =   std::unordered_map<std::string, TokenSptr>;

    //! @Ctor
    Lexer(Iter first, Iter last):
        curr_line{1},
        peek{first},
        end{last},
        words{}
    {
        reserve(Word{Tag::TRUE, "true"});
        reserve(Word{Tag::FALSE, "false"});
    }

    /**
     * @brief check if peek is still in range
     */
    bool not_end() const
    {
        return peek < end;
    }

    int lines()const
    {
        return curr_line;
    }

    /**
     * @brief scan
     * @return  token's shared pointer
     */
    TokenSptr scan()
    {
        //! for white spaces
        for( ; not_end(); ++peek)
        {
            if( *peek == ' ' || *peek == '\t')
                continue;
            else if (*peek == '\n')
                ++curr_line;
            else break;
        }

        //! for comments "\\"
        //! ex2.6.1 a)
        if(end - peek > 1   &&  *peek == '\\'   &&  *(peek + 1) == '\\')
            for( peek += 2; not_end() &&  *peek != '\n'; ++peek );
        if(not_end()    &&  *peek == '\n')
        {
            ++curr_line;
            ++peek;
        }

        //! for comments "\**\"
        //! ex2.6.1 b)
        if(end - peek > 1   &&  *peek == '\\'   &&  *(peek + 1) == '*')
            for(peek += 2;
                not_end()  &&  !((end - peek > 2) && *peek == '*' && *(peek + 1) == '\\');
                ++peek)
                if(*peek    ==  '\n')
                    ++curr_line;

        //! for operators : < <= == != => >
        //! ex2.6.2
        if(not_end()    &&  *peek == '<')
        {
            if(end - peek > 1   &&  *(peek + 1) == '=')                     //  <=
            {
                peek += 2;
                return std::make_shared<Op>(Tag::LESSEQ);
            }
            else                                                            //  <
            {
                ++peek;
                return std::make_shared<Op>(Tag::LESS);
            }
        }
        if(end - peek > 1   &&  *peek == '='    &&  *(peek + 1) == '=')     //  ==
        {
            peek += 2;
            return std::make_shared<Op>(Tag::EQ);
        }
        if(end - peek > 1   &&  *peek == '!'    &&  *(peek + 1) == '=')     //  !=
        {
            peek += 2;
            return std::make_shared<Op>(Tag::NOTEQ);
        }
        if(not_end()    &&  *peek == '>')
        {
            if(end - peek > 1   &&  *(peek + 1) == '=')                     //  >=
            {
                peek += 2;
                return std::make_shared<Op>(Tag::GREEQ);
            }
            ++peek;
            return std::make_shared<Op>(Tag::GRE);                          //  >
        }

        //! for real number, like .5
        //! ex2.6.3
        if(end - peek > 1   &&  *peek == '.')
            return handle_real(0);


        //! for num or real
        if(not_end()    &&  std::isdigit(*peek))
        {
            int val = 0;
            while(not_end()   &&  std::isdigit(*peek))
                val = val * 10  +   *peek++    - 48;

            //! when '.' appears
            if(not_end()    &&  *peek == '.')
                return handle_real(val);

            return std::make_shared<Num>(val);
        }

        //! for word
        if(not_end() && std::isalpha(*peek))
        {
            std::string key{};
            while(not_end() && (std::isalpha(*peek)   ||  std::isdigit(*peek)))
                key.push_back(*peek++);

            auto w = words.find(key);
            if(w != words.end())    return w->second;

            auto ret = std::make_shared<Word>(Tag::ID, key);
            words.insert({key, ret});
            return ret;
        }

        //! for the rest cases
        return std::make_shared<Token>
                (not_end()?   Token{*peek++}    :   Token{Tag::END});
    }

private:
    int curr_line;
    Iter peek;
    const Iter end;
    HashTable words;

    //! to save reserved word like "if" "else" and "class", etc.
    void reserve(const Word& w)
    {
        TokenSptr t = std::make_shared<Word>(w);
        words.insert({w.lexeme, t});
    }

    //! implemented for ex2.6.3
    TokenSptr handle_real(int int_part)
    {
        float val = int_part;
        ++peek;
        for(float frac = 0.1f; not_end() && std::isdigit(*peek); frac/=10)
            val +=  (*peek++ - 48) * frac;

        return std::make_shared<Real>(val);
    }
};
}}//namespace
#endif // LEXER_HPP
