#include <cassert>
#include  <iostream>
#include "ast.h"
#include "parser.h"

Parser::Parser(const char* filename, ContextHolder context):
    m_tokenizer(filename), m_context(context){

}

StatementBase* Parser::buildSyntaxTree(){
    return buildFunctionDecl();
}


template<typename T>
static T logError(const char* message){
    std::cerr << "ERROR: " << message << "\n";
    return nullptr;
}


// statements :== <assignment_statement> | <return_statement>
StatementBase* Parser::buildStatement(){
    if(m_tokenizer.current().getType() == lex::Ret)
        return buildReturnStatement();

    // must be a assignment statement then!
    return buildAssignmentStatement();
}

// function_decl :== 'function', <identifier>, 'gives', <type_qualification>,  
//                     <function_args_list>, '{', <expression>+, ''}'
FunctionDecl* Parser::buildFunctionDecl(){
    if(m_tokenizer.current().getType() != lex::FunctionDecl){}
        return logError<FunctionDecl*>("function declaration must begin with keyword function");

    // eat function decl
    lex::Token name_token = m_tokenizer.next();
    if(name_token.getType() != lex::Identifier)
        return logError<FunctionDecl*>("function declaration does not have identifier");

    std::string name = name_token.getStringLiteral();

    if(m_tokenizer.getNextType() != lex::Gives)
        return logError<FunctionDecl*>("function declaration must provide return type");

    // FIXME: it is possible to have other types
    // currently only int is supported
    assert(m_tokenizer.getNextType() == lex::Int);
    FunctionArgLists* arg_list = buildFunctionArgList();

    if(m_tokenizer.getCurrentType() != lex::LeftBrace)
        return logError<FunctionDecl*>("expected {");
    m_tokenizer.consume();


    // currently only assignment expression is supported
    StatementBase* exp;
    std::vector<StatementBase*> expressions;
    while((exp = buildStatement())){
        assert(exp && "expression must be non nullptr");
        expressions.push_back(exp);
    }

    return new FunctionDecl (std::move(expressions)
            , arg_list, std::move(name));
}

// assignment_expression :== <identifier>, '=', <integer_literal>, ';'
StatementBase* Parser::buildAssignmentStatement(){
    if(m_tokenizer.getCurrentType() != lex::Identifier)
        return nullptr;

    assert(m_tokenizer.getCurrentType() == lex::Identifier);
    std::string name = m_tokenizer.current().getStringLiteral();
    if(m_tokenizer.getNextType() != lex::Equal)
        return nullptr;

    lex::Token number_constant = m_tokenizer.next();
    if(number_constant.getType() != lex::IntegerLiteral)
        return nullptr;

    long long value = number_constant.getIntegerLiteral();
    if(m_tokenizer.getNextType() != lex::SemiColon)
        return nullptr; 
    m_tokenizer.consume();

    return  new AssignmentStatement (name, value);
}


// FIXME: maybe put arg_declaration into its own function?
// function_args_list :== '[', args_declaration+, ']'
//  args_declaration :== <type_qualification> + identifier + ','
FunctionArgLists* Parser::buildFunctionArgList(){
    if(m_tokenizer.getNextType() != lex::LeftBracket)
        return logError<FunctionArgLists*>("expected [");

    // parsing args declaration
    // FIXME: add a way to map token into type qualification

    std::vector<TypeInfo> args {};
    while(m_tokenizer.getNextType() == lex::Int){
        // lex type_qualification = m_tokenizer.getCurrentType();
        
        lex::Token next_token = m_tokenizer.next();
        if(next_token.getType() != lex::Identifier)
            return logError<FunctionArgLists*>("expected identifier");
        std::string name = next_token.getStringLiteral();

        if(m_tokenizer.getNextType() != lex::Comma)
            return logError<FunctionArgLists*>("expected comma");

        args.push_back(TypeInfo {Int32, name});
    }

    if(m_tokenizer.getCurrentType() != lex::RightBracket)
        return logError<FunctionArgLists*>("expected ]");
    
    // pop this token ]
    m_tokenizer.consume();

   return new FunctionArgLists (std::move(args));
}

StatementBase* Parser::buildReturnStatement(){
    // we don't call logError here 
    // because this is the first time
    if(m_tokenizer.getCurrentType() != lex::Ret)
        return nullptr;
    
    if(m_tokenizer.getNextType() !=  lex::Identifier){
        return logError<StatementBase*>("expected identifier");
    }

    std::string name = m_tokenizer.current().getStringLiteral();
    if(m_tokenizer.getNextType() != lex::SemiColon){
        return logError<StatementBase*>("expected semi colon");
    }

    return new ReturnStatement (name);
}
