#pragma once
#include <vector>
#include <string>
#include <memory>
#include "ast.h"

std::string ToUpper(std::string s);

std::vector<std::string> Tokenize(const std::string& input);

class Parser {
private:
    std::vector<std::string> tokens;
    size_t pos;

    std::string Peek() const;
    std::string Advance();
    void Consume(const std::string& expected);
    bool Match(const std::string& expected);
    
    std::shared_ptr<Condition> ParseCondition();

public:
    Parser(const std::vector<std::string>& t);
    
    std::shared_ptr<Command> Parse();
};