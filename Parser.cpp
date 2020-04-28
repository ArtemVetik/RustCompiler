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

    _currentToken = saveToken;
    if ((*_currentToken)->GetType() == PLUS || (*_currentToken)->GetType() == MINUS ||
        (*_currentToken)->GetType() == EXCL) {
        _currentToken++;
        return MinTerminal();
    }

    _currentToken = saveToken;
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
    if (LetDecl()) {
        return _currentToken == _tokens.end();
    }
    return false;
}

bool Parser::LetDecl() {
    if (_currentToken >= _tokens.end())
        return false;

    if ((*_currentToken)->GetType() == LET) {
        _currentToken++;
        if (Pat()){
            Init();
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

    auto saveToken = _currentToken;

    if ((*_currentToken)->GetType() == MUT)
        _currentToken++;

    if (IsID()) {
        if ((*_currentToken)->GetType() == COLON) {
            _currentToken++;
            return Type();
        }
        return true;
    }

    _currentToken = saveToken;
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
        return Expr();
    }
    return false;
}

bool Parser::Expr() {
    return IsLiteral() || GroupInit();
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

    if (IsLiteral()) {
        while(_currentToken < _tokens.end()) {
            if ((*_currentToken)->GetType() == COM) {
                _currentToken++;
            }
            else
                break;

            if (_currentToken < _tokens.end() && IsLiteral())
                continue;

            else
                return false;
        }
        return true;
    }

    return false;
}
