#ifndef AST_H
#define AST_H

#include <string>
#include <vector>

// llvm includes
#include "llvm/IR/Value.h"
#include "llvm/IR/Function.h"

#include "context.h"

// for dynamic casting purposes
class StatementBase{
public:
    virtual void dump();
private:
};

enum Type{
    Int32
};

struct TypeInfo{
    TypeInfo() = default;

    Type kind;
    std::string name; 
};

//============================== Miscellaneous ==============================

// FIXME: remove this class in the future. This makes
// this makes codegen have non trivial behavior!
// We are already expected a function declaration in the LLVM IR 
// level already, so what is the point of this class?
class FunctionArgLists{
public:
    using ArgsIter = std::vector<TypeInfo>::const_iterator;

    FunctionArgLists(std::vector<TypeInfo>&& args);
    void codegen(ContextHolder holder);

    ArgsIter begin() const;
    ArgsIter end() const;
private:
    std::vector<TypeInfo> m_args;
};

class FunctionDecl : public StatementBase{
public:
    FunctionDecl(std::vector<StatementBase*>&& statements, FunctionArgLists* arg_list, std::string&& name);

    llvm::Value* codegen(ContextHolder holder);
    void dump() override; 
private:
    std::vector<StatementBase*> m_statements;
    FunctionArgLists* m_arg_list;
    std::string m_name; 
};

//============================== Statements ==============================
enum AssignmentType{
    Constant, 
};

class AssignmentStatement : public StatementBase{
public: 
    AssignmentStatement(const std::string& name, long long value) ;
    void codegen(ContextHolder holder);

    const std::string& getName();
    long long getValue();
private:
    std::string m_name; 
    long long m_value; 
    AssignmentType m_type = Constant;
};

class ReturnStatement : public StatementBase{
public:
    // returning an identifier
    ReturnStatement(const std::string&name);

    void codegen(ContextHolder holder);
private: 
    enum ReturnType{
        Identifier,
        Expression // Add this!
    };

    // Are we returning a value or a type?
    std::string m_name;
    ReturnType m_type;
};

#endif
