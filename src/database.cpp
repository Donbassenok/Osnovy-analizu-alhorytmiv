#include "database.h"
#include <iostream>
#include <stdexcept>
#include <iomanip>

int Table::GetColumnIndex(const std::string& col_name) const {
    for (size_t i = 0; i < columns.size(); ++i) {
        if (columns[i].name == col_name) return static_cast<int>(i);
    }
    return -1; 
}

void Database::ExecuteCreate(const CreateCommand& cmd) {
    if (tables.find(cmd.table_name) != tables.end()) {
        throw std::runtime_error("Table already exists: " + cmd.table_name);
    }

    Table new_table;
    new_table.name = cmd.table_name;
    new_table.columns = cmd.columns;
    
    tables[cmd.table_name] = new_table;
    std::cout << "Table " << cmd.table_name << " has been created.\n";
}

void Database::ExecuteInsert(const InsertCommand& cmd) {
    auto it = tables.find(cmd.table_name);
    if (it == tables.end()) {
        throw std::runtime_error("Table does not exist: " + cmd.table_name);
    }

    Table& table = it->second;

    if (cmd.values.size() != table.columns.size()) {
        throw std::runtime_error("Insert failed: expected " + 
                                 std::to_string(table.columns.size()) + 
                                 " values, got " + std::to_string(cmd.values.size()));
    }

    table.rows.push_back(cmd.values);
    std::cout << "1 row has been inserted into " << cmd.table_name << ".\n";
}

bool Database::EvaluateCondition(const std::shared_ptr<Condition>& cond, const Table& table, const Row& row) const {
    if (!cond) return true;

    if (auto comp = std::dynamic_pointer_cast<ComparisonCondition>(cond)) {
        int col_idx = table.GetColumnIndex(comp->column);
        if (col_idx == -1) {
            throw std::runtime_error("Column not found in WHERE clause: " + comp->column);
        }
        
        const std::string& cell_value = row[col_idx];
        
        if (comp->op == "=") return cell_value == comp->value;
        if (comp->op == "<") return cell_value < comp->value;
        
        return false;
    } 
    else if (auto log = std::dynamic_pointer_cast<LogicalCondition>(cond)) {
        bool left_val = EvaluateCondition(log->left, table, row);
        bool right_val = EvaluateCondition(log->right, table, row);
        
        if (log->op == "AND") return left_val && right_val;
        if (log->op == "OR") return left_val || right_val;
    }
    
    return false;
}

void Database::ExecuteSelect(const SelectCommand& cmd) {
    auto it = tables.find(cmd.table_name);
    if (it == tables.end()) {
        throw std::runtime_error("Table does not exist: " + cmd.table_name);
    }

    const Table& table = it->second;
    std::vector<Row> result_rows;

    for (const auto& row : table.rows) {
        if (EvaluateCondition(cmd.where_condition, table, row)) {
            result_rows.push_back(row);
        }
    }

    PrintTableASCII(table, result_rows);
}

void Database::PrintTableASCII(const Table& table, const std::vector<Row>& result_rows) const {
    if (table.columns.empty()) return;

    std::vector<size_t> col_widths(table.columns.size(), 0);
    for (size_t i = 0; i < table.columns.size(); ++i) {
        col_widths[i] = table.columns[i].name.length();
    }
    for (const auto& row : result_rows) {
        for (size_t i = 0; i < row.size(); ++i) {
            if (row[i].length() > col_widths[i]) {
                col_widths[i] = row[i].length();
            }
        }
    }

    auto print_separator = [&]() {
        std::cout << "+";
        for (size_t w : col_widths) std::cout << std::string(w + 2, '-') << "+";
        std::cout << "\n";
    };

    print_separator();
    
    std::cout << "|";
    for (size_t i = 0; i < table.columns.size(); ++i) {
        std::cout << " " << std::setw(col_widths[i]) << std::left << table.columns[i].name << " |";
    }
    std::cout << "\n";
    
    print_separator();

    for (const auto& row : result_rows) {
        std::cout << "|";
        for (size_t i = 0; i < row.size(); ++i) {
            std::cout << " " << std::setw(col_widths[i]) << std::left << row[i] << " |";
        }
        std::cout << "\n";
    }
    
    print_separator();
}