#ifndef PARSER_H
#define PARSER_H

#include "lex.h"
#include "ast.h"
#include "context.h"

#include <istream>

class Parser{
public:
    Parser(const char* filename, ContextHolder context);

    // nullptr if failed
    StatementBase* buildSyntaxTree();
private:
    // building the function decl
    FunctionDecl* buildFunctionDecl();
    FunctionArgLists* buildFunctionArgList();

    StatementBase* buildAssignmentStatement();
    StatementBase* buildReturnStatement();
    StatementBase* buildStatement();

    ContextHolder m_context;
    lex::Tokenizer m_tokenizer;
};

#endif
