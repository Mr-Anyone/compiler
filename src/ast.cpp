#include <cassert>
#include <iostream>
#include <llvm/IR/DerivedTypes.h>
#include "ast.h"

void StatementBase::dump(){
    std::cout << "not implemented" << std::endl;
    return;
}

FunctionDecl::FunctionDecl(std::vector<StatementBase*>&& expression, FunctionArgLists* arg_list, std::string&& name): 
    StatementBase(), m_statements(expression), m_arg_list(arg_list), m_name(name){

}

FunctionArgLists::FunctionArgLists(std::vector<TypeInfo>&& args):
    m_args(args) {
    
}

FunctionArgLists::ArgsIter FunctionArgLists::begin() const {
    return m_args.cbegin();
}

FunctionArgLists::ArgsIter FunctionArgLists::end() const {
    return m_args.cend();
}

void FunctionArgLists::codegen(ContextHolder holder){
    // stack allocate space for parameters 
    // create something like this: 
    // %0 = alloc i32, align 4 
    // store i32 %func_arg, i32* %0, align 4

    for(int i = 0;i<m_args.size(); ++i){
        llvm::Value* value =
            holder->builder.CreateAlloca(llvm::Type::getInt32Ty(holder->context));

        const std::string& name = m_args[i].name;
        if(holder->symbol_table.find(name) == holder->symbol_table.end()){
            std::cerr << "cannot find symobol exit early!"; 
            std::exit(-1);
        }

        holder->builder.CreateStore(holder->symbol_table[name], value);
    }

}

template<typename T>
static bool trivialCodeGen(StatementBase* base, ContextHolder holder){
    T* statement = nullptr;
    if((statement = dynamic_cast<T*>(base))){
        statement->codegen(holder);
        return false;
    }

    return false;
}

llvm::Value* FunctionDecl::codegen(ContextHolder holder){
    // FIXME: make this more efficient 
    std::vector<llvm::Type*> args; 
    std::vector<std::string> names;

    // creating function signature
     for(auto it = m_arg_list->begin(), ie = m_arg_list->end(); it != ie; ++it){
        args.push_back(llvm::Type::getInt32Ty(holder->context));
        names.push_back(it->name);
    }   
    
 llvm::FunctionType* function_type = 
        llvm::FunctionType::get(llvm::Type::getInt32Ty(holder->context), args, /*isVarArg=*/false);

    llvm::Function* function = 
        llvm::Function::Create(function_type, llvm::Function::ExternalLinkage, m_name, holder->module);

    // appending to symbol table
    int count = 0;
    for(llvm::Argument& arg: function->args()){
        const std::string& name = names[count++];
        arg.setName(name);

        // adding this to symbol table
        holder->symbol_table[name] = &arg;
    }

    llvm::BasicBlock *BB = llvm::BasicBlock::Create(holder->context, "main", function);
    holder->builder.SetInsertPoint(BB);

    m_arg_list->codegen(holder);
    // generate the code for the statements
    for(auto* statement: m_statements){
        // FIXME: this is really ugly!
        if(trivialCodeGen<AssignmentStatement>(statement, holder)){

        }else if(trivialCodeGen<ReturnStatement>(statement, holder)){

        }else{
            assert(false && "cannot code gen!");
        }
    }
    
    return function;
}

void AssignmentStatement::codegen(ContextHolder holder){
    // holder->builder.CreateLoad();
}

AssignmentStatement::AssignmentStatement(const std::string& name, long long value): 
    StatementBase(), m_name(name), m_value(value) {
}

const std::string& AssignmentStatement::getName(){
    return m_name;
}

long long AssignmentStatement::getValue(){
    assert(m_type == Constant && "the value must be a constant");
    return m_value;
}

void FunctionDecl::dump(){
    std::cout << "unimplemented!" << std::endl;
}

ReturnStatement::ReturnStatement(const std::string& name): 
    StatementBase(), m_name(name), m_type(ReturnType::Identifier){
}

void ReturnStatement::codegen(ContextHolder holder){
    holder->builder.CreateRet(holder->symbol_table[m_name]);
}
