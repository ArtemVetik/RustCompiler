#include "Parser.h"

Parser::Parser(const std::vector<Token *> &tokens) : tree(new Node(new NodeData())) {
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
                tree.DeleteNode(tmpRoot);
                tree.DeleteNode(rightChild);
                return false;
            }

            if (root == nullptr)
                tmpRoot->AddChild(leftChild);
            else
                tmpRoot->AddChild(root);
            tmpRoot->AddChild(rightChild);
            root = new Node(*tmpRoot);
            tree.DeleteNode(rightChild);
            tree.DeleteNode(tmpRoot);
        }
        if (root == nullptr)
            root = leftChild;
        return true;
    }
    tree.DeleteNode(leftChild);
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
                tree.DeleteNode(tmpRoot);
                tree.DeleteNode(rightChild);
                return false;
            }

            if (root == nullptr)
                tmpRoot->AddChild(leftChild);
            else
                tmpRoot->AddChild(root);
            tmpRoot->AddChild(rightChild);
            root = new Node(*tmpRoot);
            tree.DeleteNode(rightChild);
            tree.DeleteNode(tmpRoot);
        }
        if (root == nullptr)
            root = leftChild;
        return true;
    }
    tree.DeleteNode(leftChild);
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
                tree.DeleteNode(rightChild);
                tree.DeleteNode(tmpRoot);
                return false;
            }

            if (root == nullptr)
                tmpRoot->AddChild(leftChild);
            else
                tmpRoot->AddChild(root);

            tmpRoot->AddChild(rightChild);
            root = new Node(*tmpRoot);
            tree.DeleteNode(rightChild);
            tree.DeleteNode(tmpRoot);
        }
        if (root == nullptr)
            root = leftChild;
        return true;
    }

    tree.DeleteNode(leftChild);
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
                tree.DeleteNode(tmpRoot);
                tree.DeleteNode(rightChild);
                return false;
            }

            if (root == nullptr)
                tmpRoot->AddChild(leftChild);
            else
                tmpRoot->AddChild(root);
            tmpRoot->AddChild(rightChild);
            root = new Node(*tmpRoot);
            tree.DeleteNode(rightChild);
            tree.DeleteNode(tmpRoot);
        }
        if (root == nullptr)
            root = leftChild;
        return true;
    }

    tree.DeleteNode(leftChild);
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
                tree.DeleteNode(rightChild);
                tree.DeleteNode(tmpRoot);
                return false;
            }

            if (root == nullptr)
                tmpRoot->AddChild(leftChild);
            else
                tmpRoot->AddChild(root);
            tmpRoot->AddChild(rightChild);
            root = new Node(*tmpRoot);
            tree.DeleteNode(tmpRoot);
            tree.DeleteNode(rightChild);
        }
        if (root == nullptr)
            root = leftChild;
        return true;
    }

    tree.DeleteNode(leftChild);
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
        tree.DeleteNode(next);
        tree.DeleteNode(root);
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

    tree.DeleteNode(root);
    return false;
}

bool Parser::IsLiteral(Node *&root) {
    if (_currentToken >= _tokens.end())
        return false;

    if (FunctionInvoke(root) || InternalFunctionInvoke(root)) // TODO node
        return true;

    if (IsID(root)){
        auto saveToken = _currentToken;
        if (!ArrayExpr(root))
            _currentToken = saveToken;
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
    if ((*_currentToken)->GetValue() == "true" || (*_currentToken)->GetValue() == "false") {
        root = new Node(new NodeData(**_currentToken, Literal));
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
        root =  new Node(new NodeData(**_currentToken, RuleType::BinaryExpression));
        _currentToken++;
        return true;
    }

    return false;
}

bool Parser::Analyze() {
    Node* tmp = nullptr;
    while (FunctionDefine(tmp));
    //if (!Block(tmp))
        //throw ParserError((*_currentToken)->GetValue());

    if (_currentToken == _tokens.end()) {
        tmp->Traversal();
        std::cout << std::endl;
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
                tree.DeleteNode(leftChild);
                tree.DeleteNode(rightChild);
                return true;
            }
        }
    }
    tree.DeleteNode(leftChild);
    tree.DeleteNode(rightChild);
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
                tree.DeleteNode(mutNode);
                tree.DeleteNode(idNode);
                return false;
            }
        }
        root = new Node(new NodeData(Token("Pat"),None));
        root->AddChild(mutNode);
        root->AddChild(idNode);
        root->AddChild(typeNode);
        tree.DeleteNode(mutNode);
        tree.DeleteNode(idNode);
        tree.DeleteNode(typeNode);
        return true;
    }
    tree.DeleteNode(mutNode);
    tree.DeleteNode(idNode);

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
    root = new Node(new NodeData(Token("GroupLet"), None));

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

            tree.DeleteNode(mutNode);
            tree.DeleteNode(idNode);
            tree.DeleteNode(patNode);

            if ((*_currentToken)->GetType() == COM)
                _currentToken++;
            else
                break;

            if ((*_currentToken)->GetType() == MUT) {
                mutNode = new Node(new NodeData(**_currentToken, None));
                _currentToken++;
            }
            if (!IsID(idNode)){
                tree.DeleteNode(mutNode);
                tree.DeleteNode(idNode);
                tree.DeleteNode(patNode);
                return false;
            }
        }
        return true;
    }
    tree.DeleteNode(mutNode);
    tree.DeleteNode(idNode);
    tree.DeleteNode(patNode);
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

    if (IsID(idNode) && ArrayExpr(arrayExprNode)) {
        root = new Node(new NodeData(Token("MemberExpression"), RuleType::MemberExpression));
        root->AddChild(idNode);
        root->AddChild(arrayExprNode);
        tree.DeleteNode(idNode);
        tree.DeleteNode(arrayExprNode);
        return true;
    }

    _currentToken = saveToken;
    if (ArrayExpr(root))
        return true;

    _currentToken = saveToken;
    if (BoolExpr(root))
        return true;

    tree.DeleteNode(idNode);
    tree.DeleteNode(arrayExprNode);
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
        tree.DeleteNode(bandNode);
        tree.DeleteNode(mutNode);
        _currentToken = saveToken;
        return false;
    }

    if ((*_currentToken)->GetType() == INTEGER || (*_currentToken)->GetType() == REAL || (*_currentToken)->GetType() == UINT) {
        typeNode = new Node(new NodeData(**_currentToken, RuleType::None));
        _currentToken++;
        root = new Node(new NodeData(Token("Type"), RuleType::None));
        root->AddChild(bandNode);
        root->AddChild(mutNode);
        root->AddChild(typeNode);
        tree.DeleteNode(bandNode);
        tree.DeleteNode(mutNode);
        tree.DeleteNode(typeNode);
        return true;
    }

    _currentToken = saveToken;
    tree.DeleteNode(bandNode);
    tree.DeleteNode(mutNode);
    tree.DeleteNode(typeNode);
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
    root = new Node(new NodeData(Token("GroupInit"), None));

    if (Expr(expr)) {
        while(_currentToken < _tokens.end()) {
            root->AddChild(expr);
            tree.DeleteNode(expr);

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

    tree.DeleteNode(expr);
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
                        tree.DeleteNode(boolExprNode);
                        tree.DeleteNode(blockNode);
                        tree.DeleteNode(elseTailNode);
                        return true;
                    }
                }
            }
        }
    }

    tree.DeleteNode(root);
    tree.DeleteNode(boolExprNode);
    tree.DeleteNode(blockNode);
    tree.DeleteNode(elseTailNode);
    return false;
}

bool Parser::Block(Node *&root) {
    Node* rule = nullptr;
    if (!BlockChecker(rule)) {
        tree.DeleteNode(rule);
        return false;
    }

    if (root == nullptr)
        root = new Node(new NodeData(Token("BlockRoot"), RuleType::Block));

    root->AddChild(rule);
    tree.DeleteNode(rule);

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
                                            tree.DeleteNode(root);
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

    auto saveToken = _currentToken;

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
                            tree.DeleteNode(stringNode);
                            tree.DeleteNode(exprListNode);
                            return true;
                        }
                    }
                }
            }
        }
    }

    _currentToken = saveToken;
    tree.DeleteNode(stringNode);
    tree.DeleteNode(exprListNode);
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
                tree.DeleteNode(exprNode);
                tree.DeleteNode(root);
                return false;
            }
            root->AddChild(exprNode);
            tree.DeleteNode(exprNode);
        }
        else {
            tree.DeleteNode(exprNode);
            return true;
        }
    }

    tree.DeleteNode(exprNode);
    tree.DeleteNode(root);
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
                        tree.DeleteNode(mutArrNode);
                        tree.DeleteNode(idArrNode);
                        tree.DeleteNode(arrTypeNode);
                        tree.DeleteNode(arrInitNode);
                        tree.DeleteNode(arrPatNode);
                        return true;
                    }
                }
            }
        }
    }

    tree.DeleteNode(mutArrNode);
    tree.DeleteNode(idArrNode);
    tree.DeleteNode(arrTypeNode);
    tree.DeleteNode(arrInitNode);
    tree.DeleteNode(arrPatNode);
    tree.DeleteNode(root);
    return false;
}

bool Parser::ArrayType(Node *&root) {
    if (_currentToken >= _tokens.end())
        return false;

    Node *bandNode = nullptr, *mutNode = nullptr, *typeNode = nullptr, *countNode = nullptr;

    auto saveToken = _currentToken;

    if ((*_currentToken)->GetType() == BAND) {
        bandNode = new Node(new NodeData(**_currentToken, RuleType::None));
        _currentToken++;
        if (_currentToken < _tokens.end() && (*_currentToken)->GetType() == MUT) {
            mutNode = new Node(new NodeData(**_currentToken, RuleType::None));
            _currentToken++;
        }
    }

    if (_currentToken >= _tokens.end()) {
        tree.DeleteNode(bandNode);
        tree.DeleteNode(mutNode);
        _currentToken = saveToken;
        return false;
    }

    if ((*_currentToken)->GetType() == SLBR) {
        _currentToken++;
        if (Type(typeNode)) {
            if (_currentToken < _tokens.end() && (*_currentToken)->GetType() == SEMICOLON) {
               _currentToken++;
               if (!Expr(countNode)) {
                   _currentToken = saveToken;
                   tree.DeleteNode(bandNode);
                   tree.DeleteNode(mutNode);
                   tree.DeleteNode(typeNode);
                   tree.DeleteNode(countNode);
                   return false;
               }
            }
            if (_currentToken < _tokens.end() && (*_currentToken)->GetType() == SRBR) {
                _currentToken++;
                root = new Node(new NodeData(Token("ArrType"), RuleType::None));
                root->AddChild(bandNode);
                root->AddChild(mutNode);
                root->AddChild(typeNode);
                root->AddChild(countNode);
                tree.DeleteNode(bandNode);
                tree.DeleteNode(mutNode);
                tree.DeleteNode(typeNode);
                tree.DeleteNode(countNode);
                return true;
            }
        }
    }

    _currentToken = saveToken;
    tree.DeleteNode(bandNode);
    tree.DeleteNode(mutNode);
    tree.DeleteNode(typeNode);
    tree.DeleteNode(countNode);
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

    root = new Node(new NodeData(Token("ArrayElem"), RuleType::ArrayElemes));
    Node *exprNode = nullptr;

    auto saveToken = _currentToken;
    if (Expr(exprNode)) {
        while (_currentToken < _tokens.end()) {
            root->AddChild(exprNode);
            tree.DeleteNode(exprNode);
            auto saveToken2 = _currentToken;
            if ((*_currentToken)->GetType() == COM || (*_currentToken)->GetType() == SEMICOLON) {
                _currentToken++;
                if (!Expr(exprNode)) {
                    _currentToken = saveToken2;
                    tree.DeleteNode(exprNode);
                    tree.DeleteNode(root);
                    return false;
                }
            }
            else
                break;
        }
        tree.DeleteNode(exprNode);
        return true;
    }

    _currentToken = saveToken;
    tree.DeleteNode(exprNode);
    tree.DeleteNode(root);
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
                tree.DeleteNode(idNode);
                tree.DeleteNode(arrayExprNode);
                tree.DeleteNode(memberExprNode);
                tree.DeleteNode(initNode);
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
            tree.DeleteNode(idNode);
            tree.DeleteNode(initNode);
            return true;
        }
    }

    _currentToken = saveToken;
    tree.DeleteNode(idNode);
    tree.DeleteNode(arrayExprNode);
    tree.DeleteNode(memberExprNode);
    tree.DeleteNode(initNode);
    tree.DeleteNode(root);
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
                        tree.DeleteNode(boolExprNode);
                        tree.DeleteNode(blockNode);
                        return true;
                    }
                }
            }
        }
    }

    _currentToken = saveToken;
    tree.DeleteNode(boolExprNode);
    tree.DeleteNode(blockNode);
    tree.DeleteNode(root);
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
                    tree.DeleteNode(blockNode);
                    return true;
                }
            }
        }
    }

    _currentToken = saveToken;
    tree.DeleteNode(root);
    tree.DeleteNode(blockNode);
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
                    tree.DeleteNode(argumentNode);
                    auto saveToken2 = _currentToken;
                    if (_currentToken < _tokens.end() && (*_currentToken)->GetType() == COM) {
                        _currentToken++;
                        if (FuncArgument(argumentNode)) {
                            continue;
                        }
                        else {
                            _currentToken = saveToken2;
                            tree.DeleteNode(idNode);
                            tree.DeleteNode(argumentNode);
                            tree.DeleteNode(parameterNode);
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
                tree.DeleteNode(idNode);
                tree.DeleteNode(argumentNode);
                tree.DeleteNode(parameterNode);
                return true;
            }
        }
    }

    _currentToken = saveToken1;
    tree.DeleteNode(idNode);
    tree.DeleteNode(parameterNode);
    tree.DeleteNode(argumentNode);
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
            root = new Node(new NodeData(Token("ArrayArg"), RuleType::None));
            root->AddChild(bandNode);
            root->AddChild(mutNode);
            root->AddChild(idNode);
            tree.DeleteNode(bandNode);
            tree.DeleteNode(mutNode);
            tree.DeleteNode(idNode);
            return true;
        }
    }

    _currentToken = saveToken;
    tree.DeleteNode(bandNode);
    tree.DeleteNode(mutNode);
    tree.DeleteNode(idNode);
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
                tree.DeleteNode(idNode);
                tree.DeleteNode(funcInvokeNode);
                return true;
            }
        }
    }

    _currentToken = saveToken;
    tree.DeleteNode(idNode);
    tree.DeleteNode(funcInvokeNode);
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
                        tree.DeleteNode(argumentNode);
                        if (_currentToken < _tokens.end() && (*_currentToken)->GetType() == COM) {
                            _currentToken++;
                            if (FunctionDefineArg(argumentNode)) {
                                continue;
                            }
                            else {
                                tree.DeleteNode(idNode);
                                tree.DeleteNode(argumentNode);
                                tree.DeleteNode(parameterNode);
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
                            tree.DeleteNode(idNode);
                            tree.DeleteNode(argumentNode);
                            tree.DeleteNode(parameterNode);
                            tree.DeleteNode(returnTypeNode);
                            tree.DeleteNode(blockNode);
                            return true;
                        }
                    }
                }
            }
        }
    }

    tree.DeleteNode(idNode);
    tree.DeleteNode(argumentNode);
    tree.DeleteNode(parameterNode);
    tree.DeleteNode(returnTypeNode);
    tree.DeleteNode(blockNode);
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
                tree.DeleteNode(bandNode);
                tree.DeleteNode(mutNode);
                tree.DeleteNode(idNode);
                tree.DeleteNode(typeNode);
                return true;
            }
        }
    }

    _currentToken = saveToken;
    tree.DeleteNode(bandNode);
    tree.DeleteNode(mutNode);
    tree.DeleteNode(idNode);
    tree.DeleteNode(typeNode);
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

    if ((*_currentToken)->GetType() == BREAK || (*_currentToken)->GetType() == RETURN) {
        _currentToken++;
        if (_currentToken < _tokens.end() && (*_currentToken)->GetType() == SEMICOLON) {
            return true;
        }
    }

    _currentToken = saveToken;
    return false;
}
