#include "Parser.h"

Parser::Parser(const std::vector<Token *> &tokens) {
    _tokens = tokens;
    _currentToken = _tokens.begin();
}

bool Parser::BoolExpr(Token* const &token) {
    if (Add(*_currentToken))
    {
        while (IsCompOperation(*_currentToken))
        {
            if (!Add(*_currentToken))
                return false;
        }

        return true;
    }

    return false;
}

bool Parser::Add(Token* const &token) {
    if (_currentToken >= _tokens.end())
        return false;

    if (Mult(*_currentToken)) {
        while((_currentToken < _tokens.end()) &&
        ((*_currentToken)->GetType() == PLUS  || (*_currentToken)->GetType() == MINUS || (*_currentToken)->GetType() == LOR))
        {
            _currentToken++;
            if (!Mult(*_currentToken))
                return false;
        }
        return true;
    }

    return false;
}

bool Parser::Mult(Token* const &token) {
    if (_currentToken >= _tokens.end())
        return false;

    if (MinTerminal(*_currentToken)){
        while((_currentToken < _tokens.end()) &&
                ((*_currentToken)->GetType() == DIV || (*_currentToken)->GetType() == MULT || (*_currentToken)->GetType() == LAND))
        {
            _currentToken++;
            if (!MinTerminal(*_currentToken))
                return false;
        }
        return true;
    }

    return false;
}

bool Parser::MinTerminal(Token* const &token) {
    if (_currentToken >= _tokens.end())
        return false;
    auto saveToken = _currentToken;

    if (IsLiteral(*(_currentToken))) {
        return true;
    }

    _currentToken = saveToken;
    if ((*_currentToken)->GetType() == PLUS || (*_currentToken)->GetType() == MINUS ||
            (*_currentToken)->GetType() == EXCL) {
        _currentToken++;
        if (MinTerminal(*_currentToken)) {
            return true;
        }
        else return false;
    }

    _currentToken = saveToken;
    if ((*_currentToken)->GetType() == LFBR) {
        _currentToken++;
        if (BoolExpr(*_currentToken)){
            if (_currentToken < _tokens.end() && (*_currentToken)->GetType() == RGBR) {
                _currentToken++;
                return true;
            }
        }
    }

    return false;
}

bool Parser::IsLiteral(Token* const &token) {
    if (_currentToken >= _tokens.end())
        return false;

    return IsString(*_currentToken) || IsChar(*_currentToken) || IsBool(*_currentToken) || IsNum(*_currentToken) || IsID(*_currentToken);
}

bool Parser::IsString(Token* const &token) {
     if (token->GetType() == STRING) {
         _currentToken++;
         return true;
     }

    return false;
}

bool Parser::IsNum(Token* const &token) {
    if  (token->GetType() == INTNUM || token->GetType() == RNUM) {
        _currentToken++;
        return true;
    }

    return false;
}

bool Parser::IsChar(Token* const &token) {
    return false;
}

bool Parser::IsBool(Token* const &token) {
    if (token->GetValue() == "true" || token->GetValue() == "false") {
        _currentToken++;
        return true;
    }

    return false;

}

bool Parser::IsID(Token* const &token) {
    if (token->GetType() == ID) {
        _currentToken++;
        return true;
    }

    return false;
}

bool Parser::IsCompOperation(Token* const &token) {
    if (_currentToken >= _tokens.end())
        return false;

    if (token->GetType() == MORE || token->GetType() == LESS || token->GetType() == ASMR || token->GetType() == ASLS ||
                                                                token->GetType() == NASSIG || token->GetType() == EQUAL)
    {
        _currentToken++;
        return true;
    }

    return false;
}




bool Parser::Analyze() {
    if (LetDecl(*_currentToken)) {
        return _currentToken == _tokens.end();
    }
    return false;
}

bool Parser::LetDecl(Token *const &token) {
    if (_currentToken >= _tokens.end())
        return false;

    if ((*_currentToken)->GetType() == LET){
        _currentToken++;
        if (Pat(*_currentToken)){
            Init(*_currentToken);
            if (_currentToken < _tokens.end() && (*_currentToken)->GetType() == SEMICOLON){
                _currentToken++;
                return true;
            }
        }
    }

    return false;
}

bool Parser::Pat(Token *const &token) {
    if (_currentToken >= _tokens.end())
        return false;

    auto saveToken = _currentToken;

    if ((*_currentToken)->GetType() == MUT)
        _currentToken++;

    if (IsID(*_currentToken)) {
        if ((*_currentToken)->GetType() == COLON) {
            _currentToken++;
            return Type(*_currentToken);
        }
        return true;
    }

    _currentToken = saveToken;
    return GroupLet(*_currentToken);
}

bool Parser::GroupLet(Token *const &token) {
    if (_currentToken >= _tokens.end())
        return false;

    if (token->GetType() == LFBR) {
        _currentToken++;
        if (VarList(*_currentToken)){
            if (_currentToken < _tokens.end() && (*_currentToken)->GetType() == RGBR) {
                _currentToken++;
                return true;
            }
        }
    }

    return false;
}

bool Parser::VarList(Token *const &token) {
    if (_currentToken >= _tokens.end())
        return false;

    if ((*_currentToken)->GetType() == MUT)
        _currentToken++;
    if (IsID(*_currentToken)){
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

bool Parser::Init(Token *const &token) {
    if (_currentToken >= _tokens.end())
        return false;

    if (token->GetType() == ASSIG){
        _currentToken++;
        return Expr(*_currentToken);
    }
    return false;
}

bool Parser::Expr(Token *const &token) {
    return IsLiteral(token);
}

bool Parser::Type(Token *const &token) {
    if (token->GetType() == INTEGER || token->GetType() == REAL || token->GetType() == UINT){
        _currentToken++;
        return true;
    }

    return false;
}
