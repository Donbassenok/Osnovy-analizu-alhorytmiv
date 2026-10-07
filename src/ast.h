#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <memory>

struct Condition {
    virtual ~Condition() = default;
    virtual void Print(int indent = 0) const = 0;
};

struct ComparisonCondition : Condition {
    std::string column;
    std::string op;
    std::string value;

    void Print(int indent = 0) const override {
        std::string spaces(indent, ' ');
        std::cout << spaces << "[Comparison] " << column << " " << op << " " << value << "\n";
    }
};

struct LogicalCondition : Condition {
    std::shared_ptr<Condition> left;
    std::string op;
    std::shared_ptr<Condition> right;

    void Print(int indent = 0) const override {
        std::string spaces(indent, ' ');
        std::cout << spaces << "[Logical " << op << "]\n";
        std::cout << spaces << "  Left:\n";
        left->Print(indent + 4);
        std::cout << spaces << "  Right:\n";
        right->Print(indent + 4);
    }
};

struct Command {
    virtual ~Command() = default;
    virtual void Print() const = 0;
};

struct ColumnDef {
    std::string name;
    bool is_indexed;
};

struct CreateCommand : Command {
    std::string table_name;
    std::vector<ColumnDef> columns;

    void Print() const override {
        std::cout << "Parsed Command: CREATE TABLE\n";
        std::cout << "Table Name: " << table_name << "\nColumns:\n";
        for (const auto& col : columns) {
            std::cout << " - " << col.name << (col.is_indexed ? " [INDEXED]" : "") << "\n";
        }
    }
};

struct InsertCommand : Command {
    std::string table_name;
    std::vector<std::string> values;

    void Print() const override {
        std::cout << "Parsed Command: INSERT\n";
        std::cout << "Table Name: " << table_name << "\nValues: ";
        for (const auto& val : values) std::cout << val << " ";
        std::cout << "\n";
    }
};

struct SelectCommand : Command {
    std::string table_name;
    std::shared_ptr<Condition> where_condition;

    void Print() const override {
        std::cout << "Parsed Command: SELECT\n";
        std::cout << "From Table: " << table_name << "\n";
        if (where_condition) {
            std::cout << "Where:\n";
            where_condition->Print(2);
        } else {
            std::cout << "Where: (All records)\n";
        }
    }
};