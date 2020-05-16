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
              ((*_currentToken)->GetType() == DIV || (*_currentToken)->GetType() == MULT ))
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
        if (!ArrayExpr(root)) // TODO node
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
        root =  new Node(new NodeData(**_currentToken, BinaryExpression));
        _currentToken++;
        return true;
    }

    return false;
}

bool Parser::Analyze() {
    Node* tmp = nullptr;
    if (!Block(tmp)){
        return false;
    }

    tmp->Traversal();
    std::cout << std::endl;

    return _currentToken == _tokens.end();
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

    IsID(root);
    if (!ArrayExpr(root)){
        _currentToken = saveToken;
        if (!BoolExpr(root)){
            _currentToken = saveToken;
            return false;
        }
    }

    return true;
}

bool Parser::Type(Node *&root) {
    if (_currentToken >= _tokens.end())
        return false;

    if ((*_currentToken)->GetType() == BAND){
        _currentToken++;
        if (_currentToken < _tokens.end() && (*_currentToken)->GetType() == MUT){
            _currentToken++;
        }
    }

    if (_currentToken >= _tokens.end())
        return false;
    if ((*_currentToken)->GetType() == INTEGER || (*_currentToken)->GetType() == REAL || (*_currentToken)->GetType() == UINT) {
        root = new Node(new NodeData(**_currentToken, None));
        _currentToken++;
        return true;
    }

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

    if ((*_currentToken)->GetType() == IF) {
        _currentToken++;
        if (BoolExpr(root)) {
            if ((*_currentToken)->GetType() == LBLBR) {
                _currentToken++;
                if (Block(root)) {
                    if (_currentToken < _tokens.end() && (*_currentToken)->GetType() == RBLBR) {
                        _currentToken++;
                        auto saveToken = _currentToken;
                        if (!ElseTail(root)) {
                            _currentToken = saveToken;
                        }
                        return true;
                    }
                }
            }
        }
    }

    return false;
}

bool Parser::Block(Node *&root) {

    Node* rule = nullptr;
    if (!BlockChecker(rule)) {
        tree.DeleteNode(rule);
        return false;
    }
    if (root == nullptr)
        root = new Node(new NodeData(Token("Root"),RuleType::Block));
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
        if (!VarInit(root)) {
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

    if ((*_currentToken)->GetType() == PRINTLN || (*_currentToken)->GetType() == PRINT) {
        _currentToken++;
        if (_currentToken < _tokens.end() && (*_currentToken)->GetType() == EXCL) {
            _currentToken++;
            if (_currentToken < _tokens.end() && (*_currentToken)->GetType() == LFBR) {
                _currentToken++;
                if (IsString(root)) {
                    auto saveToken = _currentToken;
                    if (!ExprList(root)) {
                        _currentToken = saveToken;
                    }
                    if (_currentToken < _tokens.end() && (*_currentToken)->GetType() == RGBR) {
                        _currentToken++;
                        if (_currentToken < _tokens.end() && (*_currentToken)->GetType() == SEMICOLON) {
                            _currentToken++;
                            return true;
                        }
                    }
                }
            }
        }
    }

    return false;
}

bool Parser::ExprList(Node *&root) {
    while (_currentToken < _tokens.end()){
        auto saveToken = _currentToken;
        if ((*_currentToken)->GetType() == COM){
            _currentToken++;
            if (!Expr(root)){
                _currentToken = saveToken;
                return false;
            }
        }
        else
            return true;
    }

    return false;
}

bool Parser::LetArrayDecl(Node *&root) {
    if (_currentToken >= _tokens.end())
        return false;

    if ((*_currentToken)->GetType() == LET) {
        _currentToken++;
        if ((*_currentToken)->GetType() == MUT)
            _currentToken++;
        if (IsID(root)) {
            if ((*_currentToken)->GetType() == COLON) {
                _currentToken++;
                if (ArrayType(root)) {
                    auto saveToken = _currentToken;
                    if (!Init(root))
                        _currentToken = saveToken;
                    if ((*_currentToken)->GetType() == SEMICOLON) {
                        _currentToken++;
                        return true;
                    }
                }
            }
        }
    }

    return false;
}

bool Parser::ArrayType(Node *&root) {
    if (_currentToken >= _tokens.end())
        return false;

    auto saveToken = _currentToken;
    if ((*_currentToken)->GetType() == SLBR) {
        _currentToken++;
        if (Type(root)) {
            if (_currentToken < _tokens.end() && (*_currentToken)->GetType() == SEMICOLON) {
               _currentToken++;
               if (!Expr(root)) {
                   _currentToken = saveToken;
                   return false;
               }
            }
            if (_currentToken < _tokens.end() && (*_currentToken)->GetType() == SRBR){
                _currentToken++;
                return true;
            }
        }
    }

    _currentToken = saveToken;
    return false;
}

bool Parser::ArrayExpr(Node *&root) {
    if (_currentToken >= _tokens.end())
        return false;

    auto saveToken = _currentToken;
    if ((*_currentToken)->GetType() == SLBR){
        _currentToken++;
        if (ArrayElems(root)){
            if (_currentToken < _tokens.end() && (*_currentToken)->GetType() == SRBR){
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

    auto saveToken = _currentToken;
    if (Expr(root)){
        while (_currentToken < _tokens.end()){
            auto saveToken2 = _currentToken;
            if ((*_currentToken)->GetType() == COM){
                _currentToken++;
                if (!Expr(root)) {
                    _currentToken = saveToken2;
                    return false;
                }
            }
            else
                break;
        }
        return true;
    }

    _currentToken = saveToken;
    return false;
}

bool Parser::VarInit(Node *&root) {
    if (_currentToken >= _tokens.end())
        return false;

    auto saveToken = _currentToken;
    if (IsID(root)){
        auto saveToken2 = _currentToken;
        if (!ArrayExpr(root))
            _currentToken = saveToken2;
        if (Init(root)){
            if (_currentToken < _tokens.end() && (*_currentToken)->GetType() == SEMICOLON) {
                _currentToken++;
                return true;
            }
        }
    }

    _currentToken = saveToken;
    return false;
}

bool Parser::WhileExpr(Node *&root) {
    if (_currentToken >= _tokens.end())
        return false;

    auto saveToken = _currentToken;
    if ((*_currentToken)->GetType() == WHILE){
        _currentToken++;
        if (BoolExpr(root)){
            if (_currentToken < _tokens.end() && (*_currentToken)->GetType() == LBLBR){
                _currentToken++;
                if (Block(root)){
                    if (_currentToken < _tokens.end() && (*_currentToken)->GetType() == RBLBR){
                        _currentToken++;
                        return true;
                    }
                }
            }
        }
    }

    _currentToken = saveToken;
    return false;
}

bool Parser::LoopExpr(Node *&root) {
    if (_currentToken >= _tokens.end())
        return false;

    auto saveToken = _currentToken;

    if ((*_currentToken)->GetType() == LOOP) {
        _currentToken++;
        if (_currentToken < _tokens.end() && (*_currentToken)->GetType() == LBLBR) {
            _currentToken++;
            if (Block(root)) {
                if (_currentToken < _tokens.end() && (*_currentToken)->GetType() == RBLBR) {
                    _currentToken++;
                    return true;
                }
            }
        }
    }

    _currentToken = saveToken;
    return false;
}

bool Parser::FunctionInvoke(Node *&root) {
    if (_currentToken >= _tokens.end())
        return false;

    auto saveToken1 = _currentToken;

    if (IsID(root)) {
        if (_currentToken < _tokens.end() && (*_currentToken)->GetType() == LFBR) {
            _currentToken++;
            if (FuncArgument(root)) {
                while (_currentToken < _tokens.end()) {
                    auto saveToken2 = _currentToken;
                    if (_currentToken < _tokens.end() && (*_currentToken)->GetType() == COM) {
                        _currentToken++;
                        if (FuncArgument(root)) {
                            continue;
                        }
                        else {
                            _currentToken = saveToken2;
                            return false;
                        }
                    }
                    else break;
                }
            }
            if (_currentToken < _tokens.end() && (*_currentToken)->GetType() == RGBR) {
                _currentToken++;
                return true;
            }
        }
    }

    _currentToken = saveToken1;
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

    if ((*_currentToken)->GetType() == BAND) {
        _currentToken++;
        if (_currentToken < _tokens.end() && (*_currentToken)->GetType() == MUT)
            _currentToken++;
        if (IsID(root)) {
            return true;
        }
    }

    _currentToken = saveToken;
    return false;
}

bool Parser::InternalFunctionInvoke(Node *&root) {
    if (_currentToken >= _tokens.end())
        return false;

    auto saveToken = _currentToken;

    if (IsID(root)) {
        if (_currentToken < _tokens.end() && (*_currentToken)->GetType() == DOT) {
            _currentToken++;
            if (FunctionInvoke(root))
                return true;
        }
    }

    _currentToken = saveToken;
    return false;
}

bool Parser::FunctionDefine(Node *&root) {
    if (_currentToken >= _tokens.end())
        return false;

    if ((*_currentToken)->GetType() == FUNCTION) {
        _currentToken++;
        if (IsID(root)) {
            if (_currentToken < _tokens.end() && (*_currentToken)->GetType() == LFBR) {
                _currentToken++;
                if (FunctionDefineArg(root)) {
                    while (_currentToken < _tokens.end()) {
                        if (_currentToken < _tokens.end() && (*_currentToken)->GetType() == COM) {
                            _currentToken++;
                            if (FunctionDefineArg(root)) {
                                continue;
                            }
                            else {
                                return false;
                            }
                        }
                        else break;
                    }
                }
                if (_currentToken < _tokens.end() && (*_currentToken)->GetType() == RGBR) {
                    _currentToken++;
                    FunctionReturn(root);
                    if (_currentToken < _tokens.end() && (*_currentToken)->GetType() == LBLBR) {
                        _currentToken++;
                        Block(root);
                        if (_currentToken < _tokens.end() && (*_currentToken)->GetType() == RBLBR) {
                            _currentToken++;
                            return true;
                        }
                    }
                }
            }
        }
    }

    return false;
}

bool Parser::FunctionDefineArg(Node *&root) {
    if (_currentToken >= _tokens.end())
        return false;

    auto saveToken = _currentToken;

    if ((*_currentToken)->GetType() == BAND){
        _currentToken++;
    }
    if (_currentToken < _tokens.end() && (*_currentToken)->GetType() == MUT){
        _currentToken++;
    }

    if (IsID(root)){
        if (_currentToken < _tokens.end() && (*_currentToken)->GetType() == COLON){
            _currentToken++;
            if (Type(root)){
                return true;
            }
            if (ArrayType(root)){
                return true;
            }
        }
    }

    _currentToken = saveToken;
    return false;
}

bool Parser::FunctionReturn(Node *&root) {
    if (_currentToken >= _tokens.end())
        return false;

    auto saveToken = _currentToken;

    if ((*_currentToken)->GetType() == ARROW){
        _currentToken++;
        if (Type(root)){
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

    if ((*_currentToken)->GetType() == BREAK || (*_currentToken)->GetType() == RETURN){
        _currentToken++;
        if (_currentToken < _tokens.end() && (*_currentToken)->GetType() == SEMICOLON){
            return true;
        }
    }

    _currentToken = saveToken;
    return false;
}
