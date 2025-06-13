#include <iostream>
#include "context.h"

GlobalContext::GlobalContext():
    context(), builder(context), module("my module", context), symbol_table(){
    // module related settings
    // FIXME: this maybe incorrect
    std::cout << "setting data and target triple" << std::endl;
    module.setTargetTriple("x86_64-pc-linux-gnu");
    module.setDataLayout("e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128");
}
