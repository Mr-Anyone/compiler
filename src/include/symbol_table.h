#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include <string> 
#include <llvm/IR/Value.h>
#include <unordered_map>

class SymbolTable{
public:
    SymbolTable();


private:
    std::unordered_map<std::string, llvm::Value*> m_local_symbol_table;
};


#endif
