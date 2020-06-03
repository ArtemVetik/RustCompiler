cmake_minimum_required(VERSION 3.13)
project(RustCompiler)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_EXE_LINKER_FLAGS ${CMAKE_EXE_LINKER_FLAGS} "-static")

add_executable(${PROJECT_NAME} main.cpp Lexer/StateMachine.cpp Lexer/StateMachine.h Lexer/State.cpp Lexer/State.h Lexer/Transition.cpp Lexer/Transition.h Lexer/IDState.cpp Lexer/IDState.h Lexer/NumState.cpp Lexer/NumState.h Lexer/StringState.cpp Lexer/StringState.h Lexer/IgnoreState.cpp Lexer/IgnoreState.h Lexer/SeparatorState.cpp Lexer/SeparatorState.h Lexer/TokenType.h Lexer/Token.cpp Lexer/Token.h Exceptions/LexError.h Lexer/Lexer.cpp Lexer/Lexer.h Parser/Parser.cpp Parser/Parser.h AST_Tree/AST_Tree.cpp AST_Tree/AST_Tree.h AST_Tree/Node.cpp AST_Tree/Node.h Exceptions/ParserError.h Semantics/RuleType.h Semantics/SemanticAnalyzer.cpp Semantics/SemanticAnalyzer.h Semantics/TableData.h Semantics/TypeData.h Semantics/Table.h Exceptions/SemanticError.h Exceptions/TokenLinePosition.h Semantics/SemanticErrorDebugger.cpp Semantics/SemanticErrorDebugger.h Semantics/ProgramBlock.h CodeGenerator/CodeGenerator.cpp CodeGenerator/CodeGenerator.h CodeGenerator/MasmTypeData.h)
