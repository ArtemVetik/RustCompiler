#include "Parser.h"

Parser::Parser(const std::vector<Token *> &tokens) : _tree(new Node(new NodeData(Token("AST_Root"), RuleType::None))) {
    _tokens = tokens;
    _currentToken = _tokens.begin();
}

bool Parser::TryGetToken(const TokenType &type) {
    return (_currentToken < _tokens.end() && (*_currentToken)->GetType() == type);
}

bool Parser::BoolExpr(Node *&root) {
    if (_currentToken >= _tokens.end())
        return false;

    Node* tmpRoot = nullptr;
    Node* leftChild = nullptr, *rightChild = nullptr;

    if (BoolExprOr(leftChild)){
        while(TryGetToken(TokenType::LOR)) {
            tmpRoot = new Node(new NodeData(**_currentToken, LogicalExpression));
            _currentToken++;

            if (!BoolExprOr(rightChild)) {
                AST_Tree::DeleteNode(tmpRoot, rightChild);
                return false;
            }

            if (root == nullptr)
                tmpRoot->AddChild(leftChild);
            else
                tmpRoot->AddChild(root);
            tmpRoot->AddChild(rightChild);
            root = new Node(*tmpRoot);
            AST_Tree::DeleteNode(rightChild, tmpRoot);
        }
        if (root == nullptr)
            root = leftChild;
        return true;
    }
    AST_Tree::DeleteNode(leftChild);
    return false;
}

bool Parser::BoolExprOr(Node *&root) {
    if (_currentToken >= _tokens.end())
        return false;

    Node* tmpRoot = nullptr;
    Node* leftChild = nullptr, *rightChild = nullptr;

    if (BoolExprComp(leftChild)){
        while(TryGetToken(TokenType::LAND)) {
            tmpRoot = new Node(new NodeData(**_currentToken, LogicalExpression));
            _currentToken++;

            if (!BoolExprComp(rightChild)) {
                AST_Tree::DeleteNode(tmpRoot, rightChild);
                return false;
            }

            if (root == nullptr)
                tmpRoot->AddChild(leftChild);
            else
                tmpRoot->AddChild(root);
            tmpRoot->AddChild(rightChild);
            root = new Node(*tmpRoot);
            AST_Tree::DeleteNode(rightChild, tmpRoot);
        }
        if (root == nullptr)
            root = leftChild;
        return true;
    }
    AST_Tree::DeleteNode(leftChild);
    return false;
}

bool Parser::BoolExprComp(Node *&root) {
    if (_currentToken >= _tokens.end())
        return false;

    Node* tmpRoot = nullptr;
    Node* leftChild = nullptr, *rightChild = nullptr;

    if (Add(leftChild))
    {
        while (IsCompOperation(tmpRoot))
        {
            if (!Add(rightChild)){
                AST_Tree::DeleteNode(rightChild, tmpRoot);
                return false;
            }

            if (root == nullptr)
                tmpRoot->AddChild(leftChild);
            else
                tmpRoot->AddChild(root);

            tmpRoot->AddChild(rightChild);
            root = new Node(*tmpRoot);
            AST_Tree::DeleteNode(rightChild, tmpRoot);
        }
        if (root == nullptr)
            root = leftChild;
        return true;
    }

    AST_Tree::DeleteNode(leftChild);
    return false;
}

bool Parser::Add(Node *&root) {
    if (_currentToken >= _tokens.end())
        return false;

    Node* tmpRoot = nullptr;
    Node* leftChild = nullptr, *rightChild = nullptr;

    if (Mult(leftChild)) {
        while(TryGetToken(TokenType::PLUS)  || TryGetToken(TokenType::MINUS))
        {
            tmpRoot = new Node(new NodeData(**_currentToken, BinaryExpression));
            _currentToken++;
            if (!Mult(rightChild)) {
                AST_Tree::DeleteNode(tmpRoot, rightChild);
                return false;
            }

            if (root == nullptr)
                tmpRoot->AddChild(leftChild);
            else
                tmpRoot->AddChild(root);
            tmpRoot->AddChild(rightChild);
            root = new Node(*tmpRoot);
            AST_Tree::DeleteNode(rightChild, tmpRoot);
        }
        if (root == nullptr)
            root = leftChild;
        return true;
    }

    AST_Tree::DeleteNode(leftChild);
    return false;
}

bool Parser::Mult(Node *&root) {
    if (_currentToken >= _tokens.end())
        return false;

    Node* tmpRoot = nullptr;
    Node* leftChild = nullptr, *rightChild = nullptr;

    if (MinTerminal(leftChild)){
        while(TryGetToken(TokenType::DIV) || TryGetToken(TokenType::MULT) || TryGetToken(TokenType::MOD))
        {
            tmpRoot = new Node(new NodeData(**_currentToken, BinaryExpression));
            _currentToken++;
            if (!MinTerminal(rightChild)) {
                AST_Tree::DeleteNode(rightChild, tmpRoot);
                return false;
            }

            if (root == nullptr)
                tmpRoot->AddChild(leftChild);
            else
                tmpRoot->AddChild(root);
            tmpRoot->AddChild(rightChild);
            root = new Node(*tmpRoot);
            AST_Tree::DeleteNode(tmpRoot, rightChild);
        }
        if (root == nullptr)
            root = leftChild;
        return true;
    }

    AST_Tree::DeleteNode(leftChild);
    return false;
}

bool Parser::MinTerminal(Node *&root) {

    if (_currentToken >= _tokens.end())
        return false;
    auto saveToken = _currentToken;

    if (IsLiteral(root)) {
        return true;
    }

    if (TryGetToken(TokenType::PLUS) || TryGetToken(TokenType::MINUS) || TryGetToken(TokenType::EXCL)) {
        root = new Node(new NodeData(**_currentToken, UnaryExpession));
        _currentToken++;

        Node* next = nullptr;
        if (MinTerminal(next)){
            root->AddChild(next);
            return true;
        }

        AST_Tree::DeleteNode(next, root);
        return false;
    }

    _currentToken = saveToken;
    if (TryGetToken(TokenType::LRBR)) {
        _currentToken++;
        if (BoolExpr(root)) {
            if (TryGetToken(TokenType::RRBR)) {
                _currentToken++;
                return true;
            }
        }
    }

    AST_Tree::DeleteNode(root);
    return false;
}

bool Parser::IsLiteral(Node *&root) {
    if (_currentToken >= _tokens.end())
        return false;

    if (FunctionInvoke(root) || InternalFunctionInvoke(root))
        return true;

    Node* idNode = nullptr;
    Node* arrList = nullptr;

    if (IsID(idNode)){
        auto saveToken = _currentToken;
        if (!ArrayExpr(arrList))
            _currentToken = saveToken;

        if (arrList == nullptr) {
            root = new Node(*idNode);
            AST_Tree::DeleteNode(idNode);
            return true;
        }

        root = new Node(new NodeData(Token("MemberExpression"), RuleType::MemberExpression));
        root->AddChild(idNode);
        root->AddChild(arrList);
        AST_Tree::DeleteNode(idNode, arrList);
        return true;
    }
    
    return IsString(root) || IsBool(root) || IsNum(root);
}

bool Parser::IsString(Node *&root) {
    if (TryGetToken(TokenType::STRINGLIT)) {
        root = new Node(new NodeData(**_currentToken, Literal));
        _currentToken++;
        return true;
    }
    return false;
}

bool Parser::IsNum(Node *&root) {
    if  (TryGetToken(TokenType::INTNUM) || TryGetToken(TokenType::RNUM)) {
        root = new Node(new NodeData(**_currentToken, Literal));
        _currentToken++;
        return true;
    }
    return false;
}

bool Parser::IsBool(Node *&root) {
    if (TryGetToken(TokenType::BOOLLIT)) {
        root = new Node(new NodeData(**_currentToken, RuleType::Literal));
        _currentToken++;
        return true;
    }
    return false;

}

bool Parser::IsID(Node *&root) {
    if (_currentToken >= _tokens.end())
        return false;

    if (TryGetToken(TokenType::ID)) {
        root = new Node(new NodeData(**_currentToken, Identifier));
        _currentToken++;
        return true;
    }
    return false;
}

bool Parser::IsCompOperation(Node *&root) {
    if (_currentToken >= _tokens.end())
        return false;

    if (TryGetToken(TokenType::MORE) || TryGetToken(TokenType::LESS) || TryGetToken(TokenType::MOREEQUAL) || TryGetToken(TokenType::LESSEQUAL) ||
        TryGetToken(TokenType::NASSIG) || TryGetToken(TokenType::EQUAL))
    {
        root =  new Node(new NodeData(**_currentToken, RuleType::BinaryCompExpression));
        _currentToken++;
        return true;
    }
    return false;
}

bool Parser::Analyze() {
    Node* tmp = nullptr;
    while (FunctionDefine(tmp)) {
        _tree.GetRoot()->AddChild(tmp);
        AST_Tree::DeleteNode(tmp);
    }

    if (_currentToken == _tokens.end()) {
        _tree.Traversal();
        AST_Tree::DeleteNode(tmp);
        return true;
    }
    else throw ParserError();
}

bool Parser::LetDecl(Node *&root) {
    if (_currentToken >= _tokens.end())
        return false;

    Node* leftChild = nullptr;
    Node* rightChild = nullptr;

    if (TryGetToken(TokenType::LET)) {
        _currentToken++;
        if (Pat(leftChild)) {
            auto saveToken = _currentToken;
            if (!Init(rightChild)) {
                _currentToken = saveToken;
            }
            if (TryGetToken(TokenType::SEMICOLON)) {
                _currentToken++;
                root = new Node(new NodeData(Token("VerDecl"), VariableDeclaration));
                root->AddChild(leftChild);
                root->AddChild(rightChild);
                AST_Tree::DeleteNode(leftChild, rightChild);
                return true;
            }
            else throw ParserError(';', _currentToken < _tokens.end() ? *_currentToken : nullptr);
        }
    }

    AST_Tree::DeleteNode(leftChild, rightChild);
    return false;
}

bool Parser::Pat(Node *&root) {
    if (_currentToken >= _tokens.end())
        return false;

    Node* mutNode = nullptr;
    Node* idNode = nullptr;
    Node* typeNode = nullptr;

    if (TryGetToken(TokenType::MUT)){
        mutNode = new Node(new NodeData(**_currentToken, None));
        _currentToken++;
    }

    if (IsID(idNode)) {
        if (TryGetToken(TokenType::COLON)) {
            _currentToken++;
            if (!Type(typeNode)){
                AST_Tree::DeleteNode(mutNode, idNode);
                return false;
            }
        }
        root = new Node(new NodeData(Token("Pat"),None));
        root->AddChild(mutNode);
        root->AddChild(idNode);
        root->AddChild(typeNode);
        AST_Tree::DeleteNode(mutNode, idNode, typeNode);
        return true;
    }

    AST_Tree::DeleteNode(mutNode, idNode);
    return GroupLet(root);
}

bool Parser::GroupLet(Node *&root) {
    if (_currentToken >= _tokens.end())
        return false;

    if (TryGetToken(TokenType::LRBR)) {
        _currentToken++;
        if (VarList(root)){
            if (TryGetToken(TokenType::RRBR)) {
                _currentToken++;
                return true;
            }
            else throw ParserError(')', _currentToken < _tokens.end() ? *_currentToken : nullptr);
        }
    }

    return false;
}

bool Parser::VarList(Node *&root) {
    if (_currentToken >= _tokens.end())
        return false;

    Node* mutNode = nullptr;
    Node* idNode = nullptr;
    Node* patNode = nullptr;
    root = new Node(new NodeData(Token("GroupVarDeclaration"), GroupVarDeclaration));

    if (TryGetToken(TokenType::MUT)) {
        mutNode = new Node(new NodeData(**_currentToken, None));
        _currentToken++;
    }

    if (IsID(idNode)) {
        while(_currentToken < _tokens.end()) {
            patNode = new Node(new NodeData(Token("Pat"), None));
            patNode->AddChild(mutNode);
            patNode->AddChild(idNode);
            root->AddChild(patNode);

            AST_Tree::DeleteNode(mutNode, idNode, patNode);

            if (TryGetToken(TokenType::COM))
                _currentToken++;
            else
                break;

            if (TryGetToken(TokenType::MUT)) {
                mutNode = new Node(new NodeData(**_currentToken, None));
                _currentToken++;
            }
            if (!IsID(idNode)){
                AST_Tree::DeleteNode(mutNode, idNode, patNode);
                throw ParserError("id", _currentToken < _tokens.end() ? *_currentToken : nullptr);
                return false;
            }
        }
        return true;
    }
    AST_Tree::DeleteNode(mutNode, idNode, patNode);
    return false;
}

bool Parser::Init(Node *&root) {
    if (_currentToken >= _tokens.end())
        return false;

    if (TryGetToken(TokenType::ASSIG)) {
        _currentToken++;
        auto saveToken = _currentToken;

        if (Expr(root)) {
            return true;
        }
        _currentToken = saveToken;
        if (GroupInit(root))
            return true;
        _currentToken = saveToken;
    }

    return false;
}

bool Parser::Expr(Node *&root) {
    auto saveToken = _currentToken;

    if (BoolExpr(root))
        return true;
    _currentToken = saveToken;

    if (MemberExpr(root))
        return true;
    _currentToken = saveToken;

    if (ArrayExpr(root))
        return true;

    _currentToken = saveToken;
    return false;
}

bool Parser::SimpleExpr(Node *&root) {
    if (_currentToken >= _tokens.end())
        return false;

    if (Expr(root)) {
        if (TryGetToken(TokenType::SEMICOLON)) {
            _currentToken++;
            return true;
        }
        throw ParserError(';', _currentToken < _tokens.end() ? *_currentToken : nullptr);
    }

    return false;
}

bool Parser::MemberExpr(Node *&root) {
    Node* idNode = nullptr, *arrayExprNode = nullptr;

    auto saveToken = _currentToken;

    if (IsID(idNode) && ArrayExpr(arrayExprNode)) {
        root = new Node(new NodeData(Token("MemberExpression"), RuleType::MemberExpression));
        root->AddChild(idNode);
        root->AddChild(arrayExprNode);
        AST_Tree::DeleteNode(idNode, arrayExprNode);
        return true;
    }

    _currentToken = saveToken;
    AST_Tree::DeleteNode(idNode, arrayExprNode);
    return false;
}

bool Parser::Type(Node *&root) {
    if (_currentToken >= _tokens.end())
        return false;

    auto saveToken = _currentToken;

    Node *bandNode = nullptr, *mutNode = nullptr, *typeNode = nullptr;

    if (TryGetToken(TokenType::BAND)) {
        bandNode = new Node(new NodeData(**_currentToken, RuleType::None));
        _currentToken++;
        if (TryGetToken(TokenType::MUT)) {
            mutNode = new Node(new NodeData(**_currentToken, RuleType::None));
            _currentToken++;
        }
    }

    if (_currentToken >= _tokens.end()) {
        AST_Tree::DeleteNode(bandNode, mutNode);
        _currentToken = saveToken;
        return false;
    }

    if (TryGetToken(TokenType::INTEGER) || TryGetToken(TokenType::REAL) || TryGetToken(TokenType::UINT) || TryGetToken(TokenType::BOOL)) {
        typeNode = new Node(new NodeData(**_currentToken, RuleType::None));
        _currentToken++;
        root = new Node(new NodeData(Token("TypeData"), RuleType::IdType));
        root->AddChild(bandNode);
        root->AddChild(mutNode);
        root->AddChild(typeNode);
        AST_Tree::DeleteNode(bandNode, mutNode, typeNode);
        return true;
    }

    _currentToken = saveToken;
    AST_Tree::DeleteNode(bandNode, mutNode, typeNode);

    return false;
}

bool Parser::GroupInit(Node *&root) {
    if (_currentToken >= _tokens.end())
        return false;

    if (TryGetToken(TokenType::LRBR)) {
        _currentToken++;
        if (LitList(root)){
            if (TryGetToken(TokenType::RRBR)) {
                _currentToken++;
                return true;
            }
            else throw ParserError(')', _currentToken < _tokens.end() ? *_currentToken : nullptr);
        }
    }

    return false;
}

bool Parser::LitList(Node *&root) {
    if (_currentToken >= _tokens.end())
        return false;

    Node* expr = nullptr;
    root = new Node(new NodeData(Token("GroupInit"), RuleType::GroupInit));

    if (Expr(expr)) {
        while(_currentToken < _tokens.end()) {
            root->AddChild(expr);
            AST_Tree::DeleteNode(expr);

            if ((*_currentToken)->GetType() == TokenType::COM) {
                _currentToken++;
            }
            else
                break;

            if (_currentToken < _tokens.end() && Expr(expr))
                continue;
            else
                return false;
        }
        return true;
    }

    AST_Tree::DeleteNode(expr);
    return false;
}

bool Parser::ElseTail(Node *&root) {
    if (_currentToken >= _tokens.end())
        return false;

    if (TryGetToken(TokenType::ELSE)) {
        _currentToken++;
        auto saveToken = _currentToken;
        if (IfExpr(root)) return true;
        _currentToken = saveToken;
        if (TryGetToken(TokenType::LFIGBR)) {
            _currentToken++;
            if (Block(root)) {
                if (TryGetToken(TokenType::RFIGBR)) {
                    _currentToken++;
                    return true;
                }
                else throw ParserError('}', _currentToken < _tokens.end() ? *_currentToken : nullptr);
            }
        }
        else throw ParserError('{', _currentToken < _tokens.end() ? *_currentToken : nullptr);
    }

    return false;
}

bool Parser::IfExpr(Node *&root) {
    if (_currentToken >= _tokens.end())
        return false;

    Node *boolExprNode = nullptr, *blockNode = nullptr, *elseTailNode = nullptr;

    if (TryGetToken(TokenType::IF)) {
        root = new Node(new NodeData(**_currentToken, RuleType::IfExpr));
        _currentToken++;
        if (BoolExpr(boolExprNode)) {
            if (TryGetToken(TokenType::LFIGBR)) {
                _currentToken++;
                if (Block(blockNode)) {
                    if (TryGetToken(TokenType::RFIGBR)) {
                        _currentToken++;
                        auto saveToken = _currentToken;
                        if (!ElseTail(elseTailNode)) {
                            _currentToken = saveToken;
                        }
                        root->AddChild(boolExprNode);
                        root->AddChild(blockNode);
                        root->AddChild(elseTailNode);
                        AST_Tree::DeleteNode(boolExprNode, blockNode, elseTailNode);
                        return true;
                    }
                    else throw ParserError('}', _currentToken < _tokens.end() ? *_currentToken : nullptr);
                }
            }
            else throw ParserError('{', _currentToken < _tokens.end() ? *_currentToken : nullptr);
        }
    }

    AST_Tree::DeleteNode(root, boolExprNode, blockNode, elseTailNode);
    return false;
}

bool Parser::Block(Node *&root) {
    Node* rule = nullptr;
    if (!BlockChecker(rule)) {
        AST_Tree::DeleteNode(rule);
        return false;
    }

    if (root == nullptr)
        root = new Node(new NodeData(Token("BlockRoot"), RuleType::Block));

    root->AddChild(rule);
    AST_Tree::DeleteNode(rule);

    auto saveToken = _currentToken;
    if (!Block(root))
        _currentToken = saveToken;

    return true;
}

    bool Parser::BlockChecker(Node *&root) {
    if (_currentToken >= _tokens.end())
        return false;

    auto saveToken = _currentToken;

    if (!LetDecl(root)){
        _currentToken = saveToken;
        if (!Assignment(root)) {
            _currentToken = saveToken;
            if (!IfExpr(root)) {
                _currentToken = saveToken;
                if (!WhileExpr(root)) {
                    _currentToken = saveToken;
                    if (!LoopExpr(root)) {
                        _currentToken = saveToken;
                        if (!Println(root)) {
                            _currentToken = saveToken;
                            if (!LetArrayDecl(root)) {
                                _currentToken = saveToken;
                                if (!BlockExit(root)) {
                                    _currentToken = saveToken;
                                    if (!SimpleExpr(root)) {
                                        _currentToken = saveToken;
                                        if ((*_currentToken++)->GetType() != TokenType::SEMICOLON) {
                                            _currentToken = saveToken;
                                            if (!NestedBlock(root)) {
                                                _currentToken = saveToken;
                                                AST_Tree::DeleteNode(root);
                                                return false;
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    return  true;
}

bool Parser::NestedBlock(Node *& root) {
    if (_currentToken >= _tokens.end())
        return false;

    auto saveToken = _currentToken;
    if (TryGetToken(TokenType::LFIGBR)) {
        _currentToken++;
        Block(root);
        if (TryGetToken(TokenType::RFIGBR)) {
            _currentToken++;
            return true;
        }
        else throw ParserError('}', _currentToken < _tokens.end() ? *_currentToken : nullptr);
    }

    _currentToken = saveToken;
    return false;
}

bool Parser::Println(Node *&root) {
    if (_currentToken >= _tokens.end())
        return false;

    Node *stringNode = nullptr, *exprListNode = nullptr;

    if (TryGetToken(TokenType::PRINTLN) || TryGetToken(TokenType::PRINT)) {
        _currentToken++;
        if (TryGetToken(TokenType::EXCL)) {
            _currentToken++;
            if (TryGetToken(TokenType::LRBR)) {
                _currentToken++;
                if (IsString(stringNode)) {
                    auto saveToken = _currentToken;
                    if (!ExprList(exprListNode))
                        _currentToken = saveToken;
                    if (TryGetToken(TokenType::RRBR)) {
                        _currentToken++;
                        if (TryGetToken(TokenType::SEMICOLON)) {
                            _currentToken++;
                            root = new Node(new NodeData(Token("Print"), RuleType::Print));
                            root->AddChild(stringNode);
                            root->AddChild(exprListNode);
                            AST_Tree::DeleteNode(stringNode, exprListNode);
                            return true;
                        }
                        else throw ParserError(';', _currentToken < _tokens.end() ? *_currentToken : nullptr);
                    }
                    else throw ParserError(')', _currentToken < _tokens.end() ? *_currentToken : nullptr);
                }
            }
            else throw ParserError('(', _currentToken < _tokens.end() ? *_currentToken : nullptr);
        }
        else throw ParserError('!', _currentToken < _tokens.end() ? *_currentToken : nullptr);
    }

    AST_Tree::DeleteNode(stringNode, exprListNode);
    return false;
}


bool Parser::ExprList(Node *&root) {
    root = new Node(new NodeData(Token("ExprList"), RuleType::None));

    Node *exprNode = nullptr;

    while (_currentToken < _tokens.end()) {
        auto saveToken = _currentToken;
        if (TryGetToken(TokenType::COM)) {
            _currentToken++;
            if (!Expr(exprNode)) {
                _currentToken = saveToken;
                AST_Tree::DeleteNode(exprNode, root);
                return false;
            }
            root->AddChild(exprNode);
            AST_Tree::DeleteNode(exprNode);
        }
        else {
            AST_Tree::DeleteNode(exprNode);
            return true;
        }
    }

    AST_Tree::DeleteNode(exprNode, root);
    return false;
}

bool Parser::LetArrayDecl(Node *&root) {
    if (_currentToken >= _tokens.end())
        return false;

    Node *arrPatNode = nullptr, *mutArrNode = nullptr, *idArrNode = nullptr, *arrTypeNode = nullptr, *arrInitNode = nullptr;

    if (TryGetToken(TokenType::LET)) {
        root = new Node(new NodeData(Token("ArrDecl"), RuleType::ArrayDeclaration));
        _currentToken++;
        if (TryGetToken(TokenType::MUT)) {
            mutArrNode = new Node(new NodeData(**_currentToken, RuleType::None));
            _currentToken++;
        }
        if (IsID(idArrNode)) {
            if (TryGetToken(TokenType::COLON)) {
                _currentToken++;
                if (ArrayType(arrTypeNode)) {
                    auto saveToken = _currentToken;
                    if (!Init(arrInitNode))
                        _currentToken = saveToken;
                    if (TryGetToken(TokenType::SEMICOLON)) {
                        _currentToken++;
                        arrPatNode = new Node(new NodeData(Token("ArrPat"), RuleType::None));
                        arrPatNode->AddChild(mutArrNode);
                        arrPatNode->AddChild(idArrNode);
                        arrPatNode->AddChild(arrTypeNode);
                        root->AddChild(arrPatNode);
                        root->AddChild(arrInitNode);
                        AST_Tree::DeleteNode(mutArrNode, idArrNode, arrTypeNode, arrInitNode, arrPatNode);
                        return true;
                    }
                    else throw ParserError(';', _currentToken < _tokens.end() ? *_currentToken : nullptr);
                }
            }
            else throw ParserError(':', _currentToken < _tokens.end() ? *_currentToken : nullptr); // TODO не выводит
        }
    }

    AST_Tree::DeleteNode(mutArrNode, idArrNode, arrTypeNode, arrInitNode, arrPatNode, root);
    return false;
}

bool Parser::ArrayType(Node *&root) {
    if (_currentToken >= _tokens.end())
        return false;

    Node *typeNode = nullptr, *countNode = nullptr;

    auto saveToken = _currentToken;

    if (TryGetToken(TokenType::LSQRBR)) {
        _currentToken++;
        if (Type(typeNode)) {
            if (TryGetToken(TokenType::SEMICOLON)) {
               _currentToken++;
               if (!Expr(countNode)) {
                   _currentToken = saveToken;
                   AST_Tree::DeleteNode(typeNode, countNode);
                   return false;
               }
            }
            if (TryGetToken(TokenType::RSQRBR)) {
                _currentToken++;
                root = new Node(new NodeData(Token("ArrType"), RuleType::ArrayType));
                root->AddChild(typeNode);
                root->AddChild(countNode);
                AST_Tree::DeleteNode(typeNode, countNode);
                return true;
            }
            else throw ParserError(']', _currentToken < _tokens.end() ? *_currentToken : nullptr);
        }
    }

    _currentToken = saveToken;
    AST_Tree::DeleteNode(typeNode, countNode);
    return false;
}

bool Parser::ArrayExpr(Node *&root) {
    if (_currentToken >= _tokens.end())
        return false;


    auto saveToken = _currentToken;
    if (TryGetToken(TokenType::LSQRBR)) {
        _currentToken++;
        if (ArrayElems(root)){
            if (TryGetToken(TokenType::RSQRBR)) {
                _currentToken++;
                return true;
            }
            else throw ParserError(']', _currentToken < _tokens.end() ? *_currentToken : nullptr);
        }
    }

    _currentToken = saveToken;
    return false;
}

bool Parser::ArrayElems(Node *&root) {
    if (_currentToken >= _tokens.end())
        return false;

    root = new Node(new NodeData(Token("ArrayElem"), RuleType::ArrayElems));
    Node *exprNode = nullptr;

    auto saveToken = _currentToken;
    if (Expr(exprNode)) {
        while (_currentToken < _tokens.end()) {
            root->AddChild(exprNode);
            AST_Tree::DeleteNode(exprNode);
            auto saveToken2 = _currentToken;
            if (TryGetToken(TokenType::COM) || TryGetToken(TokenType::SEMICOLON)) {
                _currentToken++;
                if (!Expr(exprNode)) {
                    _currentToken = saveToken2;
                    AST_Tree::DeleteNode(exprNode, root);
                    return false;
                }
            }
            else
                break;
        }
        AST_Tree::DeleteNode(exprNode);
        return true;
    }

    _currentToken = saveToken;
    AST_Tree::DeleteNode(exprNode, root);
    return false;
}

bool Parser::Assignment(Node *&root) {
    if (_currentToken >= _tokens.end())
        return false;

    root = new Node(new NodeData(Token("AssignmentExpr"), RuleType::AssignmentExpression));
    Node *idNode = nullptr, *memberExprNode = nullptr, *initNode = nullptr;

    auto saveToken = _currentToken;
    if (MemberExpr(memberExprNode) && Init(initNode)) {
        if (TryGetToken(TokenType::SEMICOLON)) {
            _currentToken++;
            root->AddChild(memberExprNode);
            root->AddChild(initNode);
            AST_Tree::DeleteNode(idNode, memberExprNode, initNode);
            return true;
        }
        else throw ParserError(';', _currentToken < _tokens.end() ? *_currentToken : nullptr);
    }

    _currentToken = saveToken;
    if (IsID(idNode) && Init(initNode)) {
        if (TryGetToken(TokenType::SEMICOLON)) {
            _currentToken++;
            root->AddChild(idNode);
            root->AddChild(initNode);
            AST_Tree::DeleteNode(idNode, initNode);
            return true;
        }
        else throw ParserError(';', _currentToken < _tokens.end() ? *_currentToken : nullptr);
    }

    _currentToken = saveToken;
    AST_Tree::DeleteNode(idNode, memberExprNode, initNode, root);
    return false;
}

bool Parser::WhileExpr(Node *&root) {
    if (_currentToken >= _tokens.end())
        return false;

    Node *boolExprNode = nullptr, *blockNode = nullptr;

    auto saveToken = _currentToken;
    if (TryGetToken(TokenType::WHILE)) {
        root = new Node(new NodeData(**_currentToken, RuleType::WhileExpr));
        _currentToken++;
        if (BoolExpr(boolExprNode)){
            if (TryGetToken(TokenType::LFIGBR)) {
                _currentToken++;
                if (Block(blockNode)) {
                    if (TryGetToken(TokenType::RFIGBR)) {
                        _currentToken++;
                        root->AddChild(boolExprNode);
                        root->AddChild(blockNode);
                        AST_Tree::DeleteNode(boolExprNode, blockNode);
                        return true;
                    }
                    else throw ParserError('}', _currentToken < _tokens.end() ? *_currentToken : nullptr);
                }
            }
            else throw ParserError('{', _currentToken < _tokens.end() ? *_currentToken : nullptr);
        }
    }

    _currentToken = saveToken;
    AST_Tree::DeleteNode(boolExprNode, blockNode, root);
    return false;
}

bool Parser::LoopExpr(Node *&root) {
    if (_currentToken >= _tokens.end())
        return false;

    auto saveToken = _currentToken;

    Node *blockNode = nullptr;

    if (TryGetToken(TokenType::LOOP)) {
        root = new Node(new NodeData(**_currentToken, RuleType::LoopExpr));
        _currentToken++;
        if (TryGetToken(TokenType::LFIGBR)) {
            _currentToken++;
            if (Block(blockNode)) {
                if (TryGetToken(TokenType::RFIGBR)) {
                    _currentToken++;
                    root->AddChild(blockNode);
                    AST_Tree::DeleteNode(blockNode);
                    return true;
                }
                else throw ParserError('}', _currentToken < _tokens.end() ? *_currentToken : nullptr);
            }
        }
        else throw ParserError('{', _currentToken < _tokens.end() ? *_currentToken : nullptr);
    }

    _currentToken = saveToken;
    AST_Tree::DeleteNode(root, blockNode);
    return false;
}

bool Parser::FunctionInvoke(Node *&root) {
    if (_currentToken >= _tokens.end())
        return false;

    auto saveToken1 = _currentToken;

    Node *idNode = nullptr, *argumentNode = nullptr;
    Node *parameterNode = new Node(new NodeData(Token("ParamList"), RuleType::None));

    if (IsID(idNode)) {
        if (TryGetToken(TokenType::LRBR)) {
            _currentToken++;
            if (FuncArgument(argumentNode)) {
                while (_currentToken < _tokens.end()) {
                    parameterNode->AddChild(argumentNode);
                    AST_Tree::DeleteNode(argumentNode);
                    auto saveToken2 = _currentToken;
                    if (TryGetToken(TokenType::COM)) {
                        _currentToken++;
                        if (FuncArgument(argumentNode)) {
                            continue;
                        }
                        else {
                            _currentToken = saveToken2;
                            AST_Tree::DeleteNode(idNode, argumentNode, parameterNode);
                            return false;
                        }
                    }
                    else break;
                }
            }
            if (TryGetToken(TokenType::RRBR)) {
                _currentToken++;
                root = new Node(new NodeData(Token("FuncInvoke"), RuleType::FuncInvoke));
                root->AddChild(idNode);
                root->AddChild(parameterNode);
                AST_Tree::DeleteNode(idNode, argumentNode, parameterNode);
                return true;
            }
            else throw ParserError(')', _currentToken < _tokens.end() ? *_currentToken : nullptr);
        }
    }

    _currentToken = saveToken1;
    AST_Tree::DeleteNode(idNode, parameterNode, argumentNode);
    return false;
}

bool Parser::FuncArgument(Node *&root) {
    if (_currentToken >= _tokens.end())
        return false;

    auto saveToken = _currentToken;

    if (!Expr(root)) {
        _currentToken = saveToken;
        if (!ArrayFuncArgument(root)) {
            _currentToken = saveToken;
            return false;
        }
    }

    return true;
}

bool Parser::ArrayFuncArgument(Node *&root) {
    if (_currentToken >= _tokens.end())
        return false;

    auto saveToken = _currentToken;

    Node *bandNode = nullptr, *mutNode = nullptr, *idNode = nullptr;

    if (TryGetToken(TokenType::BAND)) {
        bandNode = new Node(new NodeData(**_currentToken, RuleType::None));
        _currentToken++;
        if (TryGetToken(TokenType::MUT)) {
            mutNode = new Node(new NodeData(**_currentToken, RuleType::None));
            _currentToken++;
        }
        if (IsID(idNode)) {
            root = new Node(new NodeData(Token("ArrayArg"), RuleType::ArrayArg));
            root->AddChild(bandNode);
            root->AddChild(mutNode);
            root->AddChild(idNode);
            AST_Tree::DeleteNode(bandNode, mutNode, idNode);
            return true;
        }
    }

    _currentToken = saveToken;
    AST_Tree::DeleteNode(bandNode, mutNode, idNode);
    return false;
}

bool Parser::InternalFunctionInvoke(Node *&root) {
    if (_currentToken >= _tokens.end())
        return false;

    auto saveToken = _currentToken;

    Node *idNode = nullptr, *funcInvokeNode = nullptr;

    if (IsID(idNode)) {
        if (TryGetToken(TokenType::DOT)) {
            _currentToken++;
            if (FunctionInvoke(funcInvokeNode)) {
                root = new Node(new NodeData(Token("InternalFuncInvoke"), RuleType::InternalFuncInvoke));
                root->AddChild(idNode);
                root->AddChild(funcInvokeNode);
                AST_Tree::DeleteNode(idNode, funcInvokeNode);
                return true;
            }
        }
    }

    _currentToken = saveToken;
    AST_Tree::DeleteNode(idNode, funcInvokeNode);
    return false;
}

bool Parser::FunctionParameters(Node *& parameterNode) {
    Node* argumentNode = nullptr;
    parameterNode = new Node(new NodeData(Token("ParamListDefine"), RuleType::None));

    auto saveToken = _currentToken;

    if (FunctionDefineArg(argumentNode)) {
        while (_currentToken < _tokens.end()) {
            parameterNode->AddChild(argumentNode);
            AST_Tree::DeleteNode(argumentNode);
            if (TryGetToken(TokenType::COM)) {
                _currentToken++;
                if (FunctionDefineArg(argumentNode)) {
                    continue;
                }
                else {
                    AST_Tree::DeleteNode(argumentNode, parameterNode);
                    _currentToken = saveToken;
                    return false;
                }
            }
            else break;
        }
    }

    return true;
}

bool Parser::FunctionDefine(Node *&root) {
    if (_currentToken >= _tokens.end())
        return false;

    Node *idNode = nullptr, *returnTypeNode = nullptr, *blockNode = nullptr;
    Node *parameterNode = nullptr;

    auto saveToken = _currentToken;

    if (TryGetToken(TokenType::FUNCTION)) {
        _currentToken++;
        if (IsID(idNode)) {
            if (TryGetToken(TokenType::LRBR)) {
                _currentToken++;
                FunctionParameters(parameterNode);
                if (TryGetToken(TokenType::RRBR)) {
                    _currentToken++;
                    FunctionReturn(returnTypeNode);
                    if (TryGetToken(TokenType::LFIGBR)) {
                        _currentToken++;
                        Block(blockNode);
                        if (TryGetToken(TokenType::RFIGBR)) {
                            _currentToken++;
                            root = new Node(new NodeData(Token("FuncDeclaration"), RuleType::FuncDeclaration));
                            root->AddChild(idNode);
                            root->AddChild(parameterNode);
                            root->AddChild(returnTypeNode);
                            root->AddChild(blockNode);
                            AST_Tree::DeleteNode(idNode, parameterNode, returnTypeNode, blockNode);
                            return true;
                        }
                        else throw ParserError('}', _currentToken < _tokens.end() ? *_currentToken : nullptr);
                    }
                    else throw ParserError('{', _currentToken < _tokens.end() ? *_currentToken : nullptr);
                }
                else throw ParserError(')', _currentToken < _tokens.end() ? *_currentToken : nullptr);
            }
            else throw ParserError('(', _currentToken < _tokens.end() ? *_currentToken : nullptr);
        }
        else throw ParserError("id", _currentToken < _tokens.end() ? *_currentToken : nullptr);
    }

    _currentToken = saveToken;
    AST_Tree::DeleteNode(idNode, parameterNode, returnTypeNode, blockNode);
    return false;
}

bool Parser::FunctionDefineArg(Node *&root) {
    if (_currentToken >= _tokens.end())
        return false;

    Node *bandNode = nullptr, *mutNode = nullptr, *idNode = nullptr, *typeNode = nullptr;

    auto saveToken = _currentToken;

    if (TryGetToken(TokenType::BAND)) {
        bandNode = new Node(new NodeData(**_currentToken, RuleType::None));
        _currentToken++;
    }
    if (TryGetToken(TokenType::MUT)) {
        mutNode = new Node(new NodeData(**_currentToken, RuleType::None));
        _currentToken++;
    }

    if (IsID(idNode)) {
        if (TryGetToken(TokenType::COLON)) {
            _currentToken++;
            if (Type(typeNode) || ArrayType(typeNode)) {
                root = new Node(new NodeData(Token("DefinePat"), RuleType::None));
                root->AddChild(bandNode);
                root->AddChild(mutNode);
                root->AddChild(idNode);
                root->AddChild(typeNode);
                AST_Tree::DeleteNode(bandNode, mutNode, idNode, typeNode);
                return true;
            }
        }
        else throw ParserError(':', _currentToken < _tokens.end() ? *_currentToken : nullptr);
    }

    _currentToken = saveToken;
    AST_Tree::DeleteNode(bandNode, mutNode, idNode, typeNode);
    return false;
}

bool Parser::FunctionReturn(Node *&root) {
    if (_currentToken >= _tokens.end())
        return false;

    auto saveToken = _currentToken;

    if (TryGetToken(TokenType::ARROW)) {
        _currentToken++;
        if (Type(root)) {
            return true;
        }
    }

    _currentToken = saveToken;
    return false;
}

bool Parser::BlockExit(Node *&root) {
    if (_currentToken >= _tokens.end())
        return false;

    auto saveToken = _currentToken;

    Node *exprNode = nullptr;

    if (TryGetToken(TokenType::RETURN)) {
        auto returnToken = _currentToken;
        _currentToken++;
        auto saveToken2 = _currentToken;
        if (!Expr(exprNode)) _currentToken = saveToken2;
        if (TryGetToken(TokenType::SEMICOLON)) {
            _currentToken++;
            root = new Node(new NodeData(**returnToken, RuleType::Return));
            root->AddChild(exprNode);
            AST_Tree::DeleteNode(exprNode);
            return true;
        }
        else throw ParserError(';', _currentToken < _tokens.end() ? *_currentToken : nullptr);
    }

    if (TryGetToken(TokenType::BREAK)) {
        _currentToken++;
        if (TryGetToken(TokenType::SEMICOLON)) {
            _currentToken++;
            root = new Node(new NodeData(Token("Break"), RuleType::Break));
            return true;
        }
        else throw ParserError(';', _currentToken < _tokens.end() ? *_currentToken : nullptr);
    }
    _currentToken = saveToken;

    AST_Tree::DeleteNode(exprNode);
    return false;
}

const AST_Tree &Parser::GetASTTree() const {
    return _tree;
}
