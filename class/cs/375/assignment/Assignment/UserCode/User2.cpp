/******************************************************************\
 * Author: 
 * Copyright 2015, DigiPen Institute of Technology
\******************************************************************/
#include "../Drivers/Driver2.hpp"

bool Block();
bool Class();
bool Var();
bool Function();
bool Parameter();
bool SpecifiedType();
bool Type();
bool NamedType();
bool FunctionType();
bool Scope();
bool Statement();
bool DelimitedStatement();
bool FreeStatement();
bool Label();
bool Goto();
bool Return();
bool If();
bool Else();
bool While();
bool For();
bool GroupedExpression();
bool Literal();
bool NameReference();
bool Value();
bool Expression();
bool Expression1();
bool Expression2();
bool Expression3();
bool Expression4();
bool Expression5();
bool Expression6();
bool Expression7();
bool MemberAccess();
bool Call();
bool Cast();
bool Index();

class Acceptor
{
public:
    static void Initialize(std::vector<Token> & token_stream);
    static bool Accept(TokenType::Enum next_token);
    static bool Expect(TokenType::Enum next_token);
    static bool Expect(bool found);
    static unsigned _index;
    static std::vector<Token> * _token_stream;
};

unsigned Acceptor::_index = 0;
std::vector<Token> * Acceptor::_token_stream = nullptr;

void Acceptor::Initialize(std::vector<Token> & token_stream)
{
    _index = 0;
    _token_stream = &token_stream;
}

bool Acceptor::Accept(TokenType::Enum accepted_token)
{
    if (_index >= _token_stream->size())
    {
        return false;
    }

    Token & current_token = (*_token_stream)[_index];
    if(current_token.mTokenType == accepted_token)
    {
        PrintRule::AcceptedToken(current_token);
        ++_index;
        return true;
    }
    return false;
}

bool Acceptor::Expect(TokenType::Enum accepted_token)
{

    if(Accept(accepted_token))
    {
        return true;
    }
    throw ParsingException();
}

bool Acceptor::Expect(bool found)
{
    if (found)
    {
        return true;
    }
    throw ParsingException();
}

bool FunctionType()
{
    return false;
}

bool NamedType()
{
    return false;
}

bool Type()
{
    return NamedType() || FunctionType();
}

bool SpecifiedType()
{
    PrintRule print_rule("SpecifiedType");
    if (Acceptor::Accept(TokenType::Colon))
    {
        Acceptor::Expect(Type());
    }
    print_rule.Accept();
    return true;
}

bool Parameter()
{
  return false;
}

bool Function()
{
    return false;
}

bool Var()
{
    PrintRule print_rule("Var");
    if (!Acceptor::Accept(TokenType::Var))
    {
      return false;
    }

    Acceptor::Expect(TokenType::Identifier);
    Acceptor::Expect(SpecifiedType);

    if (Acceptor::Accept(TokenType::Assignment))
    {
      Acceptor::Expect(Expression());
    }

    print_rule.Accept();
    return true;
}

bool Class()
{
    PrintRule print_rule("Class");
    if(!Acceptor::Accept(TokenType::Class))
    {
        return false;
    }

    Acceptor::Expect(TokenType::Identifier);
    Acceptor::Expect(TokenType::OpenCurley);

    while(true)
    {
        if(Var())
        {
            Acceptor::Expect(TokenType::Semicolon);
        }
        else if(!Function())
        {
            break;
        }
    }

    Acceptor::Expect(TokenType::CloseCurley);

    print_rule.Accept();
    return true;
}


bool Block()
{
    PrintRule print_rule("Block");

    if(Class() || Function() || (Var() && Acceptor::Expect(TokenType::Semicolon)))
    {
        print_rule.Accept();
        return true;
    }
    return false;
}

void RemoveWhitespaceAndComments(std::vector<Token>& tokens)
{
    // Create new vector for all non whitespace and non comment tokens.
    std::vector<Token> stripped;
    stripped.reserve(tokens.size());

    // Add acceptable tokens to the stripped vector.
    for(const Token & token : tokens)
    {
        if(token.mTokenType != TokenType::Whitespace && 
           token.mTokenType != TokenType::SingleLineComment &&
           token.mTokenType != TokenType::MultiLineComment)
        {
            stripped.push_back(token);
        }
    }

    stripped.shrink_to_fit();
    tokens = stripped;
}

void Recognize(std::vector<Token>& tokens)
{
    Acceptor::Initialize(tokens);
    if(!Block())
    {
        throw ParsingException();
    }

   
}
