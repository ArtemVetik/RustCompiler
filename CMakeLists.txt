cmake_minimum_required(VERSION 3.13)
project(RustCompiler)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXE_LINKER_FLAGS ${CMAKE_EXE_LINKER_FLAGS} "-static")

add_executable(${PROJECT_NAME} main.cpp Lexer/StateMachine.cpp Lexer/StateMachine.h Lexer/States/State.cpp Lexer/States/State.h Lexer/Transition.cpp Lexer/Transition.h Lexer/States/IDState.cpp Lexer/States/IDState.h Lexer/States/NumState.cpp Lexer/States/NumState.h Lexer/States/StringState.cpp Lexer/States/StringState.h Lexer/States/IgnoreState.cpp Lexer/States/IgnoreState.h Lexer/States/SeparatorState.cpp Lexer/States/SeparatorState.h Lexer/TokenType.h Lexer/Token.cpp Lexer/Token.h Exceptions/LexError.h Lexer/Lexer.cpp Lexer/Lexer.h Parser/Parser.cpp Parser/Parser.h Parser/AST_Tree/AST_Tree.cpp Parser/AST_Tree/AST_Tree.h Parser/AST_Tree/Node.cpp Parser/AST_Tree/Node.h Exceptions/ParserError.h Parser/AST_Tree/RuleType.h Semantics/SemanticAnalyzer.cpp Semantics/SemanticAnalyzer.h Semantics/TableData.h Semantics/TypeData.h Semantics/Table.h Exceptions/SemanticError.h Exceptions/TokenLinePosition.h Semantics/SemanticErrorDebugger.cpp Semantics/SemanticErrorDebugger.h Semantics/ProgramBlock.h CodeGenerator/CodeGenerator.cpp CodeGenerator/CodeGenerator.h CodeGenerator/MasmTypeData.h Exceptions/CodeGeneratorError.h Exceptions/BaseError.h)
