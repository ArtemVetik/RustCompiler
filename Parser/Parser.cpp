#include "Parser.h"

Parser::Parser(const std::vector<Token *> &tokens) : tree(new Node(new NodeData(Token("AST_Root"), RuleType::None))) {
    _tokens = tokens;
    _currentToken = _tokens.begin();
}

bool Parser::BoolExpr(Node *&root) {
    if (_currentToken >= _tokens.end())
        return false;

    Node* tmpRoot = nullptr;
    Node* leftChild = nullptr, *rightChild = nullptr;

    if (BoolExprOr(leftChild)){
        while((_currentToken < _tokens.end()) &&
              ((*_currentToken)->GetType() == LOR)) {
            tmpRoot = new Node(new NodeData(**_currentToken, LogicalExpression));
            _currentToken++;

            if (!BoolExprOr(rightChild)) {
                AST_Tree::DeleteNode(tmpRoot);
                AST_Tree::DeleteNode(rightChild);
                return false;
            }

            if (root == nullptr)
                tmpRoot->AddChild(leftChild);
            else
                tmpRoot->AddChild(root);
            tmpRoot->AddChild(rightChild);
            root = new Node(*tmpRoot);
            AST_Tree::DeleteNode(rightChild);
            AST_Tree::DeleteNode(tmpRoot);
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
        while((_currentToken < _tokens.end()) &&
              ((*_currentToken)->GetType() == LAND)) {
            tmpRoot = new Node(new NodeData(**_currentToken, LogicalExpression));
            _currentToken++;

            if (!BoolExprComp(rightChild)) {
                AST_Tree::DeleteNode(tmpRoot);
                AST_Tree::DeleteNode(rightChild);
                return false;
            }

            if (root == nullptr)
                tmpRoot->AddChild(leftChild);
            else
                tmpRoot->AddChild(root);
            tmpRoot->AddChild(rightChild);
            root = new Node(*tmpRoot);
            AST_Tree::DeleteNode(rightChild);
            AST_Tree::DeleteNode(tmpRoot);
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
                AST_Tree::DeleteNode(rightChild);
                AST_Tree::DeleteNode(tmpRoot);
                return false;
            }

            if (root == nullptr)
                tmpRoot->AddChild(leftChild);
            else
                tmpRoot->AddChild(root);

            tmpRoot->AddChild(rightChild);
            root = new Node(*tmpRoot);
            AST_Tree::DeleteNode(rightChild);
            AST_Tree::DeleteNode(tmpRoot);
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
        while((_currentToken < _tokens.end()) &&
              ((*_currentToken)->GetType() == PLUS  || (*_currentToken)->GetType() == MINUS))
        {
            tmpRoot = new Node(new NodeData(**_currentToken, BinaryExpression));
            _currentToken++;
            if (!Mult(rightChild)) {
                AST_Tree::DeleteNode(tmpRoot);
                AST_Tree::DeleteNode(rightChild);
                return false;
            }

            if (root == nullptr)
                tmpRoot->AddChild(leftChild);
            else
                tmpRoot->AddChild(root);
            tmpRoot->AddChild(rightChild);
            root = new Node(*tmpRoot);
            AST_Tree::DeleteNode(rightChild);
            AST_Tree::DeleteNode(tmpRoot);
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
        while((_currentToken < _tokens.end()) &&
              ((*_currentToken)->GetType() == DIV || (*_currentToken)->GetType() == MULT || (*_currentToken)->GetType() == MOD))
        {
            tmpRoot = new Node(new NodeData(**_currentToken, BinaryExpression));
            _currentToken++;
            if (!MinTerminal(rightChild)){
                AST_Tree::DeleteNode(rightChild);
                AST_Tree::DeleteNode(tmpRoot);
                return false;
            }

            if (root == nullptr)
                tmpRoot->AddChild(leftChild);
            else
                tmpRoot->AddChild(root);
            tmpRoot->AddChild(rightChild);
            root = new Node(*tmpRoot);
            AST_Tree::DeleteNode(tmpRoot);
            AST_Tree::DeleteNode(rightChild);
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

    if ((*_currentToken)->GetType() == PLUS || (*_currentToken)->GetType() == MINUS ||
        (*_currentToken)->GetType() == EXCL) {
        root = new Node(new NodeData(**_currentToken, UnaryExpession));
        _currentToken++;

        Node* next = nullptr;
        if (MinTerminal(next)){
            root->AddChild(next);
            return true;
        }
        AST_Tree::DeleteNode(next);
        AST_Tree::DeleteNode(root);
        return false;
    }

    _currentToken = saveToken;
    if ((*_currentToken)->GetType() == LFBR) {
        _currentToken++;
        if (BoolExpr(root)){
            if (_currentToken < _tokens.end() && (*_currentToken)->GetType() == RGBR) {
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
        AST_Tree::DeleteNode(idNode);
        AST_Tree::DeleteNode(arrList);
        return true;
    }
    
    return IsString(root) || IsBool(root) || IsNum(root);
}

bool Parser::IsString(Node *&root) {
    if ((*_currentToken)->GetType() == STRING) {
        root = new Node(new NodeData(**_currentToken, Literal));
        _currentToken++;
        return true;
    }

    return false;
}

bool Parser::IsNum(Node *&root) {
    if  ((*_currentToken)->GetType() == INTNUM || (*_currentToken)->GetType() == RNUM) {
        root = new Node(new NodeData(**_currentToken, Literal));
        _currentToken++;
        return true;
    }

    return false;
}

bool Parser::IsBool(Node *&root) {
    if (_currentToken < _tokens.cend() && (*_currentToken)->GetType() == TokenType::BOOL) {
        root = new Node(new NodeData(**_currentToken, RuleType::Literal));
        _currentToken++;
        return true;
    }

    return false;

}

bool Parser::IsID(Node *&root) {
    if (_currentToken >= _tokens.end())
        return false;

    if ((*_currentToken)->GetType() == ID) {
        root = new Node(new NodeData(**_currentToken, Identifier));
        _currentToken++;
        return true;
    }

    return false;
}

bool Parser::IsCompOperation(Node *&root) {
    if (_currentToken >= _tokens.end())
        return false;

    if ((*_currentToken)->GetType() == MORE || (*_currentToken)->GetType() == LESS || (*_currentToken)->GetType() == ASMR || (*_currentToken)->GetType() == ASLS ||
        (*_currentToken)->GetType() == NASSIG || (*_currentToken)->GetType() == EQUAL)
    {
        root =  new Node(new NodeData(**_currentToken, RuleType::BinaryCompExpression));
        _currentToken++;
        return true;
    }

    return false;
}

bool Parser::Analyze() {
    Node* tmp = nullptr;
    //while (FunctionDefine(tmp)) {
    //    tree.GetRoot()->AddChild(tmp);
    //    AST_Tree::DeleteNode(tmp);
    //}
    if (!Block(tmp))
        throw ParserError((*_currentToken)->GetValue());
    tree.GetRoot()->AddChild(tmp);

    if (_currentToken == _tokens.end()) {
        tree.Traversal();
        AST_Tree::DeleteNode(tmp);
        return true;
    }
    else throw ParserError((*_currentToken)->GetValue());
}

bool Parser::LetDecl(Node *&root) {
    if (_currentToken >= _tokens.end())
        return false;

    Node* leftChild = nullptr;
    Node* rightChild = nullptr;

    if ((*_currentToken)->GetType() == LET) {
        _currentToken++;
        if (Pat(leftChild)){
            auto saveToken = _currentToken;
            if (!Init(rightChild)){
                _currentToken = saveToken;
            }
            if (_currentToken < _tokens.end() && (*_currentToken)->GetType() == SEMICOLON){
                _currentToken++;
                root = new Node(new NodeData(Token("VerDecl"), VariableDeclaration));
                root->AddChild(leftChild);
                root->AddChild(rightChild);
                AST_Tree::DeleteNode(leftChild);
                AST_Tree::DeleteNode(rightChild);
                return true;
            }
        }
    }
    AST_Tree::DeleteNode(leftChild);
    AST_Tree::DeleteNode(rightChild);
    return false;
}

bool Parser::Pat(Node *&root) {
    if (_currentToken >= _tokens.end())
        return false;

    Node* mutNode = nullptr;
    Node* idNode = nullptr;
    Node* typeNode = nullptr;

    if ((*_currentToken)->GetType() == MUT){
        mutNode = new Node(new NodeData(**_currentToken, None));
        _currentToken++;
    }

    if (IsID(idNode)) {
        if ((*_currentToken)->GetType() == COLON) {
            _currentToken++;
            if (!Type(typeNode)){
                AST_Tree::DeleteNode(mutNode);
                AST_Tree::DeleteNode(idNode);
                return false;
            }
        }
        root = new Node(new NodeData(Token("Pat"),None));
        root->AddChild(mutNode);
        root->AddChild(idNode);
        root->AddChild(typeNode);
        AST_Tree::DeleteNode(mutNode);
        AST_Tree::DeleteNode(idNode);
        AST_Tree::DeleteNode(typeNode);
        return true;
    }
    AST_Tree::DeleteNode(mutNode);
    AST_Tree::DeleteNode(idNode);

    return GroupLet(root);
}

bool Parser::GroupLet(Node *&root) {
    if (_currentToken >= _tokens.end())
        return false;

    if ((*_currentToken)->GetType() == LFBR) {
        _currentToken++;
        if (VarList(root)){
            if (_currentToken < _tokens.end() && (*_currentToken)->GetType() == RGBR) {
                _currentToken++;
                return true;
            }
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

    if ((*_currentToken)->GetType() == MUT) {
        mutNode = new Node(new NodeData(**_currentToken, None));
        _currentToken++;
    }

    if (IsID(idNode)) {
        while(_currentToken < _tokens.end()) {
            patNode = new Node(new NodeData(Token("Pat"), None));
            patNode->AddChild(mutNode);
            patNode->AddChild(idNode);
            root->AddChild(patNode);

            AST_Tree::DeleteNode(mutNode);
            AST_Tree::DeleteNode(idNode);
            AST_Tree::DeleteNode(patNode);

            if ((*_currentToken)->GetType() == COM)
                _currentToken++;
            else
                break;

            if ((*_currentToken)->GetType() == MUT) {
                mutNode = new Node(new NodeData(**_currentToken, None));
                _currentToken++;
            }
            if (!IsID(idNode)){
                AST_Tree::DeleteNode(mutNode);
                AST_Tree::DeleteNode(idNode);
                AST_Tree::DeleteNode(patNode);
                return false;
            }
        }
        return true;
    }
    AST_Tree::DeleteNode(mutNode);
    AST_Tree::DeleteNode(idNode);
    AST_Tree::DeleteNode(patNode);
    return false;
}

bool Parser::Init(Node *&root) {
    if (_currentToken >= _tokens.end())
        return false;

    if ((*_currentToken)->GetType() == ASSIG) {
        _currentToken++;
        auto saveToken = _currentToken;
        if (Expr(root))
            return true;
        _currentToken = saveToken;
        if (GroupInit(root))
            return true;
        _currentToken = saveToken;
    }
    return false;
}

bool Parser::Expr(Node *&root) {
    auto saveToken = _currentToken;

    Node *idNode = nullptr, *arrayExprNode = nullptr;

    if (BoolExpr(root))
        return true;
    _currentToken = saveToken;

    if (IsID(idNode) && ArrayExpr(arrayExprNode)) {
        root = new Node(new NodeData(Token("MemberExpression"), RuleType::MemberExpression));
        root->AddChild(idNode);
        root->AddChild(arrayExprNode);
        AST_Tree::DeleteNode(idNode);
        AST_Tree::DeleteNode(arrayExprNode);
        return true;
    }

    _currentToken = saveToken;
    if (ArrayExpr(root))
        return true;

    AST_Tree::DeleteNode(idNode);
    AST_Tree::DeleteNode(arrayExprNode);
    return false;
}

bool Parser::Type(Node *&root) {
    if (_currentToken >= _tokens.end())
        return false;

    auto saveToken = _currentToken;

    Node *bandNode = nullptr, *mutNode = nullptr, *typeNode = nullptr;

    if ((*_currentToken)->GetType() == BAND) {
        bandNode = new Node(new NodeData(**_currentToken, RuleType::None));
        _currentToken++;
        if (_currentToken < _tokens.end() && (*_currentToken)->GetType() == MUT) {
            mutNode = new Node(new NodeData(**_currentToken, RuleType::None));
            _currentToken++;
        }
    }

    if (_currentToken >= _tokens.end()) {
        AST_Tree::DeleteNode(bandNode);
        AST_Tree::DeleteNode(mutNode);
        _currentToken = saveToken;
        return false;
    }

    if ((*_currentToken)->GetType() == INTEGER || (*_currentToken)->GetType() == REAL || (*_currentToken)->GetType() == UINT) {
        typeNode = new Node(new NodeData(**_currentToken, RuleType::None));
        _currentToken++;
        root = new Node(new NodeData(Token("TypeData"), RuleType::None));
        root->AddChild(bandNode);
        root->AddChild(mutNode);
        root->AddChild(typeNode);
        AST_Tree::DeleteNode(bandNode);
        AST_Tree::DeleteNode(mutNode);
        AST_Tree::DeleteNode(typeNode);
        return true;
    }

    _currentToken = saveToken;
    AST_Tree::DeleteNode(bandNode);
    AST_Tree::DeleteNode(mutNode);
    AST_Tree::DeleteNode(typeNode);
    return false;
}

bool Parser::GroupInit(Node *&root) {
    if (_currentToken >= _tokens.end())
        return false;

    if ((*_currentToken)->GetType() == LFBR) {
        _currentToken++;
        if (LitList(root)){
            if (_currentToken < _tokens.end() && (*_currentToken)->GetType() == RGBR) {
                _currentToken++;
                return true;
            }
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

            if ((*_currentToken)->GetType() == COM) {
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

    if ((*_currentToken)->GetType() == ELSE) {
        _currentToken++;
        if (IfExpr(root)) return true;
        else if ((*_currentToken)->GetType() == LBLBR) {
            _currentToken++;
            if (Block(root)) {
                if ((*_currentToken)->GetType() == RBLBR) {
                    _currentToken++;
                    return true;
                }
            }
        }
    }

    return false;
}

bool Parser::IfExpr(Node *&root) {
    if (_currentToken >= _tokens.end())
        return false;

    Node *boolExprNode = nullptr, *blockNode = nullptr, *elseTailNode = nullptr;

    if ((*_currentToken)->GetType() == IF) {
        root = new Node(new NodeData(**_currentToken, RuleType::IfExpr));
        _currentToken++;
        if (BoolExpr(boolExprNode)) {
            if ((*_currentToken)->GetType() == LBLBR) {
                _currentToken++;
                if (Block(blockNode)) {
                    if (_currentToken < _tokens.end() && (*_currentToken)->GetType() == RBLBR) {
                        _currentToken++;
                        auto saveToken = _currentToken;
                        if (!ElseTail(elseTailNode)) {
                            _currentToken = saveToken;
                        }
                        root->AddChild(boolExprNode);
                        root->AddChild(blockNode);
                        root->AddChild(elseTailNode);
                        AST_Tree::DeleteNode(boolExprNode);
                        AST_Tree::DeleteNode(blockNode);
                        AST_Tree::DeleteNode(elseTailNode);
                        return true;
                    }
                }
            }
        }
    }

    AST_Tree::DeleteNode(root);
    AST_Tree::DeleteNode(boolExprNode);
    AST_Tree::DeleteNode(blockNode);
    AST_Tree::DeleteNode(elseTailNode);
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
                                    if (!Expr(root) || _currentToken >= _tokens.end() ||
                                        (*_currentToken++)->GetType() != SEMICOLON) {
                                        _currentToken = saveToken;
                                        if ((*_currentToken++)->GetType() != SEMICOLON) {
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

    return  true;
}

bool Parser::Println(Node *&root) {
    if (_currentToken >= _tokens.end())
        return false;

    _currentToken;

    Node *stringNode = nullptr, *exprListNode = nullptr;

    if ((*_currentToken)->GetType() == PRINTLN || (*_currentToken)->GetType() == PRINT) {
        _currentToken++;
        if (_currentToken < _tokens.end() && (*_currentToken)->GetType() == EXCL) {
            _currentToken++;
            if (_currentToken < _tokens.end() && (*_currentToken)->GetType() == LFBR) {
                _currentToken++;
                if (IsString(stringNode)) {
                    auto saveToken = _currentToken;
                    if (!ExprList(exprListNode)) {
                        _currentToken = saveToken;
                    }
                    if (_currentToken < _tokens.end() && (*_currentToken)->GetType() == RGBR) {
                        _currentToken++;
                        if (_currentToken < _tokens.end() && (*_currentToken)->GetType() == SEMICOLON) {
                            _currentToken++;
                            root = new Node(new NodeData(Token("Print"), RuleType::Print));
                            root->AddChild(stringNode);
                            root->AddChild(exprListNode);
                            AST_Tree::DeleteNode(stringNode);
                            AST_Tree::DeleteNode(exprListNode);
                            return true;
                        }
                    }
                }
            }
        }
    }

    AST_Tree::DeleteNode(stringNode);
    AST_Tree::DeleteNode(exprListNode);
    return false;
}

bool Parser::ExprList(Node *&root) {
    root = new Node(new NodeData(Token("ExprList"), RuleType::None));

    Node *exprNode = nullptr;

    while (_currentToken < _tokens.end()) {
        auto saveToken = _currentToken;
        if ((*_currentToken)->GetType() == COM) {
            _currentToken++;
            if (!Expr(exprNode)) {
                _currentToken = saveToken;
                AST_Tree::DeleteNode(exprNode);
                AST_Tree::DeleteNode(root);
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

    AST_Tree::DeleteNode(exprNode);
    AST_Tree::DeleteNode(root);
    return false;
}


bool Parser::LetArrayDecl(Node *&root) {
    if (_currentToken >= _tokens.end())
        return false;

    Node *arrPatNode = nullptr, *mutArrNode = nullptr, *idArrNode = nullptr, *arrTypeNode = nullptr, *arrInitNode = nullptr;

    if ((*_currentToken)->GetType() == LET) {
        root = new Node(new NodeData(Token("ArrDecl"), RuleType::ArrayDeclaration));
        _currentToken++;
        if ((*_currentToken)->GetType() == MUT) {
            mutArrNode = new Node(new NodeData(**_currentToken, RuleType::None));
            _currentToken++;
        }
        if (IsID(idArrNode)) {
            if ((*_currentToken)->GetType() == COLON) {
                _currentToken++;
                if (ArrayType(arrTypeNode)) {
                    auto saveToken = _currentToken;
                    if (!Init(arrInitNode))
                        _currentToken = saveToken;
                    if ((*_currentToken)->GetType() == SEMICOLON) {
                        _currentToken++;
                        arrPatNode = new Node(new NodeData(Token("ArrPat"), RuleType::None));
                        arrPatNode->AddChild(mutArrNode);
                        arrPatNode->AddChild(idArrNode);
                        arrPatNode->AddChild(arrTypeNode);
                        root->AddChild(arrPatNode);
                        root->AddChild(arrInitNode);
                        AST_Tree::DeleteNode(mutArrNode);
                        AST_Tree::DeleteNode(idArrNode);
                        AST_Tree::DeleteNode(arrTypeNode);
                        AST_Tree::DeleteNode(arrInitNode);
                        AST_Tree::DeleteNode(arrPatNode);
                        return true;
                    }
                }
            }
        }
    }

    AST_Tree::DeleteNode(mutArrNode);
    AST_Tree::DeleteNode(idArrNode);
    AST_Tree::DeleteNode(arrTypeNode);
    AST_Tree::DeleteNode(arrInitNode);
    AST_Tree::DeleteNode(arrPatNode);
    AST_Tree::DeleteNode(root);
    return false;
}

bool Parser::ArrayType(Node *&root) {
    if (_currentToken >= _tokens.end())
        return false;

    Node *typeNode = nullptr, *countNode = nullptr;

    auto saveToken = _currentToken;

    if ((*_currentToken)->GetType() == SLBR) {
        _currentToken++;
        if (Type(typeNode)) {
            if (_currentToken < _tokens.end() && (*_currentToken)->GetType() == SEMICOLON) {
               _currentToken++;
               if (!Expr(countNode)) {
                   _currentToken = saveToken;
                   AST_Tree::DeleteNode(typeNode);
                   AST_Tree::DeleteNode(countNode);
                   return false;
               }
            }
            if (_currentToken < _tokens.end() && (*_currentToken)->GetType() == SRBR) {
                _currentToken++;
                root = new Node(new NodeData(Token("ArrType"), RuleType::None));
                root->AddChild(typeNode);
                root->AddChild(countNode);
                AST_Tree::DeleteNode(typeNode);
                AST_Tree::DeleteNode(countNode);
                return true;
            }
        }
    }

    _currentToken = saveToken;
    AST_Tree::DeleteNode(typeNode);
    AST_Tree::DeleteNode(countNode);
    return false;
}

bool Parser::ArrayExpr(Node *&root) {
    if (_currentToken >= _tokens.end())
        return false;


    auto saveToken = _currentToken;
    if ((*_currentToken)->GetType() == SLBR) {
        _currentToken++;
        if (ArrayElems(root)){
            if (_currentToken < _tokens.end() && (*_currentToken)->GetType() == SRBR) {
                _currentToken++;
                return true;
            }
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
            if ((*_currentToken)->GetType() == COM || (*_currentToken)->GetType() == SEMICOLON) {
                _currentToken++;
                if (!Expr(exprNode)) {
                    _currentToken = saveToken2;
                    AST_Tree::DeleteNode(exprNode);
                    AST_Tree::DeleteNode(root);
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
    AST_Tree::DeleteNode(exprNode);
    AST_Tree::DeleteNode(root);
    return false;
}

bool Parser::Assignment(Node *&root) {
    if (_currentToken >= _tokens.end())
        return false;

    root = new Node(new NodeData(Token("AssignmentExpr"), RuleType::AssignmentExpression));
    Node *idNode = nullptr, *memberExprNode = nullptr, *arrayExprNode = nullptr, *initNode = nullptr;

    auto saveToken = _currentToken;
    if (IsID(idNode) && ArrayExpr(arrayExprNode)) {
        memberExprNode = new Node(new NodeData(Token("MemberExpression"), RuleType::MemberExpression));
        memberExprNode->AddChild(idNode);
        memberExprNode->AddChild(arrayExprNode);
        if (Init(initNode)) {
            if (_currentToken < _tokens.end() && (*_currentToken)->GetType() == SEMICOLON) {
                _currentToken++;
                root->AddChild(memberExprNode);
                root->AddChild(initNode);
                AST_Tree::DeleteNode(idNode);
                AST_Tree::DeleteNode(arrayExprNode);
                AST_Tree::DeleteNode(memberExprNode);
                AST_Tree::DeleteNode(initNode);
                return true;
            }
        }
    }

    _currentToken = saveToken;
    if (IsID(idNode) && Init(initNode)) {
        if (_currentToken < _tokens.end() && (*_currentToken)->GetType() == SEMICOLON) {
            _currentToken++;
            root->AddChild(idNode);
            root->AddChild(initNode);
            AST_Tree::DeleteNode(idNode);
            AST_Tree::DeleteNode(initNode);
            return true;
        }
    }

    _currentToken = saveToken;
    AST_Tree::DeleteNode(idNode);
    AST_Tree::DeleteNode(arrayExprNode);
    AST_Tree::DeleteNode(memberExprNode);
    AST_Tree::DeleteNode(initNode);
    AST_Tree::DeleteNode(root);
    return false;
}

bool Parser::WhileExpr(Node *&root) {
    if (_currentToken >= _tokens.end())
        return false;

    Node *boolExprNode = nullptr, *blockNode = nullptr;

    auto saveToken = _currentToken;
    if ((*_currentToken)->GetType() == WHILE) {
        root = new Node(new NodeData(**_currentToken, RuleType::WhileExpr));
        _currentToken++;
        if (BoolExpr(boolExprNode)){
            if (_currentToken < _tokens.end() && (*_currentToken)->GetType() == LBLBR) {
                _currentToken++;
                if (Block(blockNode)) {
                    if (_currentToken < _tokens.end() && (*_currentToken)->GetType() == RBLBR) {
                        _currentToken++;
                        root->AddChild(boolExprNode);
                        root->AddChild(blockNode);
                        AST_Tree::DeleteNode(boolExprNode);
                        AST_Tree::DeleteNode(blockNode);
                        return true;
                    }
                }
            }
        }
    }

    _currentToken = saveToken;
    AST_Tree::DeleteNode(boolExprNode);
    AST_Tree::DeleteNode(blockNode);
    AST_Tree::DeleteNode(root);
    return false;
}

bool Parser::LoopExpr(Node *&root) {
    if (_currentToken >= _tokens.end())
        return false;

    auto saveToken = _currentToken;

    Node *blockNode = nullptr;

    if ((*_currentToken)->GetType() == LOOP) {
        root = new Node(new NodeData(**_currentToken, RuleType::LoopExpr));
        _currentToken++;
        if (_currentToken < _tokens.end() && (*_currentToken)->GetType() == LBLBR) {
            _currentToken++;
            if (Block(blockNode)) {
                if (_currentToken < _tokens.end() && (*_currentToken)->GetType() == RBLBR) {
                    _currentToken++;
                    root->AddChild(blockNode);
                    AST_Tree::DeleteNode(blockNode);
                    return true;
                }
            }
        }
    }

    _currentToken = saveToken;
    AST_Tree::DeleteNode(root);
    AST_Tree::DeleteNode(blockNode);
    return false;
}

bool Parser::FunctionInvoke(Node *&root) {
    if (_currentToken >= _tokens.end())
        return false;

    auto saveToken1 = _currentToken;

    Node *idNode = nullptr, *argumentNode = nullptr;
    Node *parameterNode = new Node(new NodeData(Token("ParamList"), RuleType::None));

    if (IsID(idNode)) {
        if (_currentToken < _tokens.end() && (*_currentToken)->GetType() == LFBR) {
            _currentToken++;
            if (FuncArgument(argumentNode)) {
                while (_currentToken < _tokens.end()) {
                    parameterNode->AddChild(argumentNode);
                    AST_Tree::DeleteNode(argumentNode);
                    auto saveToken2 = _currentToken;
                    if (_currentToken < _tokens.end() && (*_currentToken)->GetType() == COM) {
                        _currentToken++;
                        if (FuncArgument(argumentNode)) {
                            continue;
                        }
                        else {
                            _currentToken = saveToken2;
                            AST_Tree::DeleteNode(idNode);
                            AST_Tree::DeleteNode(argumentNode);
                            AST_Tree::DeleteNode(parameterNode);
                            return false;
                        }
                    }
                    else break;
                }
            }
            if (_currentToken < _tokens.end() && (*_currentToken)->GetType() == RGBR) {
                _currentToken++;
                root = new Node(new NodeData(Token("FuncInvoke"), RuleType::FuncInvoke));
                root->AddChild(idNode);
                root->AddChild(parameterNode);
                AST_Tree::DeleteNode(idNode);
                AST_Tree::DeleteNode(argumentNode);
                AST_Tree::DeleteNode(parameterNode);
                return true;
            }
        }
    }

    _currentToken = saveToken1;
    AST_Tree::DeleteNode(idNode);
    AST_Tree::DeleteNode(parameterNode);
    AST_Tree::DeleteNode(argumentNode);
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

    if ((*_currentToken)->GetType() == BAND) {
        bandNode = new Node(new NodeData(**_currentToken, RuleType::None));
        _currentToken++;
        if (_currentToken < _tokens.end() && (*_currentToken)->GetType() == MUT) {
            mutNode = new Node(new NodeData(**_currentToken, RuleType::None));
            _currentToken++;
        }
        if (IsID(idNode)) {
            root = new Node(new NodeData(Token("ArrayArg"), RuleType::ArrayArg));
            root->AddChild(bandNode);
            root->AddChild(mutNode);
            root->AddChild(idNode);
            AST_Tree::DeleteNode(bandNode);
            AST_Tree::DeleteNode(mutNode);
            AST_Tree::DeleteNode(idNode);
            return true;
        }
    }

    _currentToken = saveToken;
    AST_Tree::DeleteNode(bandNode);
    AST_Tree::DeleteNode(mutNode);
    AST_Tree::DeleteNode(idNode);
    return false;
}

bool Parser::InternalFunctionInvoke(Node *&root) {
    if (_currentToken >= _tokens.end())
        return false;

    auto saveToken = _currentToken;

    Node *idNode = nullptr, *funcInvokeNode = nullptr;

    if (IsID(idNode)) {
        if (_currentToken < _tokens.end() && (*_currentToken)->GetType() == DOT) {
            _currentToken++;
            if (FunctionInvoke(funcInvokeNode)) {
                root = new Node(new NodeData(Token("InternalFuncInvoke"), RuleType::InternalFuncInvoke));
                root->AddChild(idNode);
                root->AddChild(funcInvokeNode);
                AST_Tree::DeleteNode(idNode);
                AST_Tree::DeleteNode(funcInvokeNode);
                return true;
            }
        }
    }

    _currentToken = saveToken;
    AST_Tree::DeleteNode(idNode);
    AST_Tree::DeleteNode(funcInvokeNode);
    return false;
}

bool Parser::FunctionDefine(Node *&root) {
    if (_currentToken >= _tokens.end())
        return false;

    Node *idNode = nullptr, *argumentNode = nullptr, *returnTypeNode = nullptr, *blockNode = nullptr;
    Node *parameterNode = new Node(new NodeData(Token("ParamListDefine"), RuleType::None));

    if ((*_currentToken)->GetType() == FUNCTION) {
        _currentToken++;
        if (IsID(idNode)) {
            if (_currentToken < _tokens.end() && (*_currentToken)->GetType() == LFBR) {
                _currentToken++;
                if (FunctionDefineArg(argumentNode)) {
                    while (_currentToken < _tokens.end()) {
                        parameterNode->AddChild(argumentNode);
                        AST_Tree::DeleteNode(argumentNode);
                        if (_currentToken < _tokens.end() && (*_currentToken)->GetType() == COM) {
                            _currentToken++;
                            if (FunctionDefineArg(argumentNode)) {
                                continue;
                            }
                            else {
                                AST_Tree::DeleteNode(idNode);
                                AST_Tree::DeleteNode(argumentNode);
                                AST_Tree::DeleteNode(parameterNode);
                                return false;
                            }
                        }
                        else break;
                    }
                }
                if (_currentToken < _tokens.end() && (*_currentToken)->GetType() == RGBR) {
                    _currentToken++;
                    FunctionReturn(returnTypeNode);
                    if (_currentToken < _tokens.end() && (*_currentToken)->GetType() == LBLBR) {
                        _currentToken++;
                        Block(blockNode);
                        if (_currentToken < _tokens.end() && (*_currentToken)->GetType() == RBLBR) {
                            _currentToken++;
                            root = new Node(new NodeData(Token("FuncDeclaration"), RuleType::FuncDeclaration));
                            root->AddChild(idNode);
                            root->AddChild(parameterNode);
                            root->AddChild(returnTypeNode);
                            root->AddChild(blockNode);
                            AST_Tree::DeleteNode(idNode);
                            AST_Tree::DeleteNode(argumentNode);
                            AST_Tree::DeleteNode(parameterNode);
                            AST_Tree::DeleteNode(returnTypeNode);
                            AST_Tree::DeleteNode(blockNode);
                            return true;
                        }
                    }
                }
            }
        }
    }

    AST_Tree::DeleteNode(idNode);
    AST_Tree::DeleteNode(argumentNode);
    AST_Tree::DeleteNode(parameterNode);
    AST_Tree::DeleteNode(returnTypeNode);
    AST_Tree::DeleteNode(blockNode);
    return false;
}

bool Parser::FunctionDefineArg(Node *&root) {
    if (_currentToken >= _tokens.end())
        return false;

    Node *bandNode = nullptr, *mutNode = nullptr, *idNode = nullptr, *typeNode = nullptr;

    auto saveToken = _currentToken;

    if ((*_currentToken)->GetType() == BAND) {
        bandNode = new Node(new NodeData(**_currentToken, RuleType::None));
        _currentToken++;
    }
    if (_currentToken < _tokens.end() && (*_currentToken)->GetType() == MUT) {
        mutNode = new Node(new NodeData(**_currentToken, RuleType::None));
        _currentToken++;
    }

    if (IsID(idNode)) {
        if (_currentToken < _tokens.end() && (*_currentToken)->GetType() == COLON) {
            _currentToken++;
            if (Type(typeNode) || ArrayType(typeNode)) {
                root = new Node(new NodeData(Token("DefinePat"), RuleType::None));
                root->AddChild(bandNode);
                root->AddChild(mutNode);
                root->AddChild(idNode);
                root->AddChild(typeNode);
                AST_Tree::DeleteNode(bandNode);
                AST_Tree::DeleteNode(mutNode);
                AST_Tree::DeleteNode(idNode);
                AST_Tree::DeleteNode(typeNode);
                return true;
            }
        }
    }

    _currentToken = saveToken;
    AST_Tree::DeleteNode(bandNode);
    AST_Tree::DeleteNode(mutNode);
    AST_Tree::DeleteNode(idNode);
    AST_Tree::DeleteNode(typeNode);
    return false;
}

bool Parser::FunctionReturn(Node *&root) {
    if (_currentToken >= _tokens.end())
        return false;

    auto saveToken = _currentToken;
    
    if ((*_currentToken)->GetType() == ARROW) {
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

    if ((*_currentToken)->GetType() == RETURN) {
        _currentToken++;
        auto saveToken2 = _currentToken;
        if (!Expr(exprNode)) _currentToken = saveToken2;
        if (_currentToken < _tokens.end() && (*_currentToken)->GetType() == SEMICOLON) {
            _currentToken++;
            root = new Node(new NodeData(Token("Return"), RuleType::Return));
            root->AddChild(exprNode);
            AST_Tree::DeleteNode(exprNode);
            return true;
        }
    }

    if ((*_currentToken)->GetType() == BREAK) {
        _currentToken++;
        if (_currentToken < _tokens.end() && (*_currentToken)->GetType() == SEMICOLON) {
            _currentToken++;
            root = new Node(new NodeData(Token("Break"), RuleType::Break));
            return true;
        }
    }
    _currentToken = saveToken;

    AST_Tree::DeleteNode(exprNode);
    return false;
}

Node *const &Parser::GetTreeRoot() const {
    return tree.GetRoot();
}
