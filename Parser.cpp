#include "Parser.h"

Parser::Parser(const std::vector<Token *> &tokens) {
    _tokens = tokens;
    _currentToken = _tokens.begin();
}

bool Parser::BoolExpr() {
    if (Add())
    {
        while (IsCompOperation())
        {
            if (!Add())
                return false;
        }

        return true;
    }
    return false;
}

bool Parser::Add() {
    if (_currentToken >= _tokens.end())
        return false;

    if (Mult()) {
        while((_currentToken < _tokens.end()) &&
              ((*_currentToken)->GetType() == PLUS  || (*_currentToken)->GetType() == MINUS || (*_currentToken)->GetType() == LOR))
        {
            _currentToken++;
            if (!Mult())
                return false;
        }
        return true;
    }

    return false;
}

bool Parser::Mult() {
    if (_currentToken >= _tokens.end())
        return false;

    if (MinTerminal()){
        while((_currentToken < _tokens.end()) &&
              ((*_currentToken)->GetType() == DIV || (*_currentToken)->GetType() == MULT || (*_currentToken)->GetType() == LAND))
        {
            _currentToken++;
            if (!MinTerminal())
                return false;
        }
        return true;
    }

    return false;
}

bool Parser::MinTerminal() {
    if (_currentToken >= _tokens.end())
        return false;
    auto saveToken = _currentToken;

    if (IsLiteral()) {
        return true;
    }

    if ((*_currentToken)->GetType() == PLUS || (*_currentToken)->GetType() == MINUS ||
        (*_currentToken)->GetType() == EXCL) {
        _currentToken++;
        return MinTerminal();
    }

    //_currentToken = saveToken;
    if ((*_currentToken)->GetType() == LFBR) {
        _currentToken++;
        if (BoolExpr()){
            if (_currentToken < _tokens.end() && (*_currentToken)->GetType() == RGBR) {
                _currentToken++;
                return true;
            }
        }
    }

    return false;
}

bool Parser::IsLiteral() {
    if (_currentToken >= _tokens.end())
        return false;

    return IsString() || IsChar() || IsBool() || IsNum() || IsID();
}

bool Parser::IsString() {
    if ((*_currentToken)->GetType() == STRING) {
        _currentToken++;
        return true;
    }

    return false;
}

bool Parser::IsNum() {
    if  ((*_currentToken)->GetType() == INTNUM || (*_currentToken)->GetType() == RNUM) {
        _currentToken++;
        return true;
    }

    return false;
}

bool Parser::IsChar() {
    return false;
}

bool Parser::IsBool() {
    if ((*_currentToken)->GetValue() == "true" || (*_currentToken)->GetValue() == "false") {
        _currentToken++;
        return true;
    }

    return false;

}

bool Parser::IsID() {
    if ((*_currentToken)->GetType() == ID) {
        _currentToken++;
        return true;
    }

    return false;
}

bool Parser::IsCompOperation() {
    if (_currentToken >= _tokens.end())
        return false;

    if ((*_currentToken)->GetType() == MORE || (*_currentToken)->GetType() == LESS || (*_currentToken)->GetType() == ASMR || (*_currentToken)->GetType() == ASLS ||
        (*_currentToken)->GetType() == NASSIG || (*_currentToken)->GetType() == EQUAL)
    {
        _currentToken++;
        return true;
    }

    return false;
}


bool Parser::Analyze() {
    if (Block()) {
        return _currentToken == _tokens.end();
    }
    return false;
}

bool Parser::LetDecl() {
    if (_currentToken >= _tokens.end())
        return false;

    auto saveToken = _currentToken;

    if ((*_currentToken)->GetType() == LET) {
        _currentToken++;
        if (Pat()){
            if (!Init()){
                _currentToken = saveToken;
            }
            if (_currentToken < _tokens.end() && (*_currentToken)->GetType() == SEMICOLON){
                _currentToken++;
                return true;
            }
        }
    }

    return false;
}

bool Parser::Pat() {
    if (_currentToken >= _tokens.end())
        return false;

    //auto saveToken = _currentToken;

    if ((*_currentToken)->GetType() == MUT)
        _currentToken++;

    if (IsID()) {
        if ((*_currentToken)->GetType() == COLON) {
            _currentToken++;
            return Type();
        }
        return true;
    }

    //_currentToken = saveToken;
    return GroupLet();
}

bool Parser::GroupLet() {
    if (_currentToken >= _tokens.end())
        return false;

    if ((*_currentToken)->GetType() == LFBR) {
        _currentToken++;
        if (VarList()){
            if (_currentToken < _tokens.end() && (*_currentToken)->GetType() == RGBR) {
                _currentToken++;
                return true;
            }
        }
    }

    return false;
}

bool Parser::VarList() {
    if (_currentToken >= _tokens.end())
        return false;

    if ((*_currentToken)->GetType() == MUT)
        _currentToken++;

    if (IsID()){
        while(_currentToken < _tokens.end()){
            if ((*_currentToken)->GetType() == COM)
                _currentToken++;
            else
                break;

            if ((*_currentToken)->GetType() == MUT)
                _currentToken++;
            if (_currentToken < _tokens.end() && (*_currentToken)->GetType() == ID)
                _currentToken++;
            else
                return false;
        }
        return true;
    }
    return false;
}

bool Parser::Init() {
    if (_currentToken >= _tokens.end())
        return false;

    if ((*_currentToken)->GetType() == ASSIG) {
        _currentToken++;
        auto saveToken = _currentToken;
        if (Expr())
            return true;
        _currentToken = saveToken;
        if (GroupInit())
            return true;
        _currentToken = saveToken;
    }
    return false;
}

bool Parser::Expr() {
    return BoolExpr();
}

bool Parser::Type() {
    if ((*_currentToken)->GetType() == INTEGER || (*_currentToken)->GetType() == REAL || (*_currentToken)->GetType() == UINT) {
        _currentToken++;
        return true;
    }

    return false;
}

bool Parser::GroupInit() {
    if (_currentToken >= _tokens.end())
        return false;

    if ((*_currentToken)->GetType() == LFBR) {
        _currentToken++;
        if (LitList()){
            if (_currentToken < _tokens.end() && (*_currentToken)->GetType() == RGBR) {
                _currentToken++;
                return true;
            }
        }
    }

    return false;
}

bool Parser::LitList() {
    if (_currentToken >= _tokens.end())
        return false;

    if (Expr()) {
        while(_currentToken < _tokens.end()) {
            if ((*_currentToken)->GetType() == COM) {
                _currentToken++;
            }
            else
                break;

            if (_currentToken < _tokens.end() && Expr())
                continue;
            else
                return false;
        }
        return true;
    }

    return false;
}

bool Parser::ElseTail() {
    if (_currentToken >= _tokens.end())
        return false;

    if ((*_currentToken)->GetType() == ELSE) {
        _currentToken++;
        if (IfExpr()) return true;
        else if ((*_currentToken)->GetType() == LBLBR) {
            _currentToken++;
            if (Block()) {
                if ((*_currentToken)->GetType() == RBLBR) {
                    _currentToken++;
                    return true;
                }
            }
        }
    }

    return false;
}

bool Parser::IfExpr() {
    if (_currentToken >= _tokens.end())
        return false;

    if ((*_currentToken)->GetType() == IF) {
        _currentToken++;
        if (BoolExpr()) {
            if ((*_currentToken)->GetType() == LBLBR) {
                _currentToken++;
                if (Block()) {
                    if (_currentToken < _tokens.end() && (*_currentToken)->GetType() == RBLBR) {
                        _currentToken++;
                        auto saveToken = _currentToken;
                        if (!ElseTail()) {
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

bool Parser::Block() {
    if (!BlockChecker())
        return false;

    auto saveToken = _currentToken;
    if (!Block())
        _currentToken = saveToken;

    return true;
}

bool Parser::BlockChecker() {
    if (_currentToken >= _tokens.end())
        return false;

    auto saveToken = _currentToken;

    if (!LetDecl()){
        _currentToken = saveToken;
        if (!IfExpr()){
            _currentToken = saveToken;
            if (!Println()){
                _currentToken = saveToken;
                return false;
            }
        }
    }

    return  true;
}

bool Parser::Println() {
    if (_currentToken >= _tokens.end())
        return false;

    if ((*_currentToken)->GetType() == PRINTLN || (*_currentToken)->GetType() == PRINT) {
        _currentToken++;
        if (_currentToken < _tokens.end() && (*_currentToken)->GetType() == EXCL) {
            _currentToken++;
            if (_currentToken < _tokens.end() && (*_currentToken)->GetType() == LFBR) {
                _currentToken++;
                if (IsString()) {
                    auto saveToken = _currentToken;
                    if (!ExprList()) {
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

bool Parser::ExprList() {
    while (_currentToken < _tokens.end()){
        auto saveToken = _currentToken;
        if ((*_currentToken)->GetType() == COM){
            _currentToken++;
            if (!Expr()){
                _currentToken = saveToken;
                return false;
            }
        }
        else
            return true;
    }

    return false;
}
