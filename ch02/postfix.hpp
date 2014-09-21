#ifndef POSTFIX_HPP
#define POSTFIX_HPP

#include <iostream>
#include <iterator>
#include <stdexcept>

namespace dragon {namespace ch2 {

class Postfix
{
public:
    using Iter = std::istream_iterator<char>;
    Postfix():
        curr{std::cin},eos{}
    {}

    void expr()
    {
        term();
        ++curr;
        while(true)
        {
            if( *curr == '+')
            {
                match('+');
                term();
                std::cout << '+';
                ++curr;
            }
            else if(*curr == '-')
            {
                match('-');
                term();
                std::cout << '-';
                ++curr;
            }
            else
                return;
        }
    }

private:
    Iter curr;
    Iter eos;

    void term()
    {
        if(std::isdigit(*curr))
        {
            std::cout << *curr;
//            match(*curr);
        }
        else
            throw std::runtime_error{"syntax error"};
    }

    void match(char t)
    {
        if(*curr == t)
            ++curr;
        else
            throw std::runtime_error{"syntax error"};
    }
};
}}//namespace



#endif // POSTFIX_HPP