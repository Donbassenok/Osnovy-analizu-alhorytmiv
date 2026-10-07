#include "parser.h"
#include <regex>
#include <stdexcept>
#include <cctype>
#include <algorithm>

std::string ToUpper(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c) { return std::toupper(c); });
    return s;
}

std::vector<std::string> Tokenize(const std::string& input) {
    std::vector<std::string> tokens;
    std::regex re(R"(\"[^\"]*\"|[a-zA-Z][a-zA-Z0-9_]*|[=<>();,])");
    auto begin = std::sregex_iterator(input.begin(), input.end(), re);
    auto end = std::sregex_iterator();
    
    for (auto i = begin; i != end; ++i) {
        tokens.push_back(i->str());
    }
    return tokens;
}

Parser::Parser(const std::vector<std::string>& t) : tokens(t), pos(0) {}

std::string Parser::Peek() const {
    if (pos >= tokens.size()) return "";
    return tokens[pos];
}

std::string Parser::Advance() {
    if (pos >= tokens.size()) return "";
    return tokens[pos++];
}

void Parser::Consume(const std::string& expected) {
    if (pos >= tokens.size() || ToUpper(tokens[pos]) != ToUpper(expected)) {
        throw std::runtime_error("Syntax error: expected '" + expected + "', got '" + Peek() + "'");
    }
    pos++;
}

bool Parser::Match(const std::string& expected) {
    if (pos < tokens.size() && ToUpper(tokens[pos]) == ToUpper(expected)) {
        pos++;
        return true;
    }
    return false;
}

std::shared_ptr<Condition> Parser::ParseCondition() {
    if (Match("(")) {
        auto left = ParseCondition();
        Consume(")");

        std::string op = ToUpper(Advance());
        if (op != "AND" && op != "OR") {
            throw std::runtime_error("Syntax error: expected AND or OR, got '" + op + "'");
        }

        Consume("(");
        auto right = ParseCondition();
        Consume(")");

        auto log_cond = std::make_shared<LogicalCondition>();
        log_cond->left = left;
        log_cond->op = op;
        log_cond->right = right;
        return log_cond;
    } else {
        auto comp = std::make_shared<ComparisonCondition>();
        
        comp->column = Advance();
        comp->op = Advance();
        if (comp->op != "=" && comp->op != "<") {
            throw std::runtime_error("Syntax error: invalid operator '" + comp->op + "'. Expected '=' or '<'");
        }
        
        comp->value = Advance();
        if (comp->value.empty() || comp->value.front() != '"') {
            throw std::runtime_error("Syntax error: value must be a string literal, got '" + comp->value + "'");
        }
        return comp;
    }
}

std::shared_ptr<Command> Parser::Parse() {
    if (tokens.empty()) return nullptr;

    std::string cmd_type = ToUpper(Advance());
    
    if (cmd_type == "CREATE") {
        auto cmd = std::make_shared<CreateCommand>();
        cmd->table_name = Advance();
        Consume("(");
        
        while (Peek() != ")") {
            ColumnDef col;
            col.name = Advance();
            col.is_indexed = Match("INDEXED");
            cmd->columns.push_back(col);
            
            if (!Match(",")) break;
        }
        Consume(")");
        return cmd;
    } 
    else if (cmd_type == "INSERT") {
        auto cmd = std::make_shared<InsertCommand>();
        Match("INTO"); 
        cmd->table_name = Advance();
        
        Consume("(");
        while (Peek() != ")") {
            std::string val = Advance();
            if (val.empty() || val.front() != '"') {
                throw std::runtime_error("Syntax error: insert values must be string literals.");
            }
            cmd->values.push_back(val);
            
            if (!Match(",")) break;
        }
        Consume(")");
        return cmd;
    } 
    else if (cmd_type == "SELECT") {
        auto cmd = std::make_shared<SelectCommand>();
        Consume("FROM");
        cmd->table_name = Advance();
        
        if (Match("WHERE")) {
            cmd->where_condition = ParseCondition();
        }
        return cmd;
    } 
    else {
        throw std::runtime_error("Unknown command: " + cmd_type);
    }
}