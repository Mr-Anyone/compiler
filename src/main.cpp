#include "parser.h"

int main(){
    // FIXME: move this into its own function!
    ContextHolder context = std::make_shared<GlobalContext>();
    Parser parser("testing.txt", context);
    StatementBase* base = parser.buildSyntaxTree();

    return 0;
}
