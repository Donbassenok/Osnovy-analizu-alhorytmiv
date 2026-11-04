#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include "ast.h"

using Row = std::vector<std::string>;

struct Table {
    std::string name;
    std::vector<ColumnDef> columns;
    std::vector<Row> rows;

    int GetColumnIndex(const std::string& col_name) const;
};

class Database {
private:
    std::unordered_map<std::string, Table> tables;

    bool EvaluateCondition(const std::shared_ptr<Condition>& cond, const Table& table, const Row& row) const;

    void PrintTableASCII(const Table& table, const std::vector<Row>& result_rows) const;

public:
    void ExecuteCreate(const CreateCommand& cmd);
    void ExecuteInsert(const InsertCommand& cmd);
    void ExecuteSelect(const SelectCommand& cmd);
};