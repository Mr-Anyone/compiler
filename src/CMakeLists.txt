set(
    SOURCE_FILES
    main.cpp
    lex.cpp
    parser.cpp 
    context.cpp
    ast.cpp
    symbol_table.cpp
)

add_executable(
    comp
    ${SOURCE_FILES}
)

target_include_directories(
    comp PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(comp LLVMCore LLVMSupport)
