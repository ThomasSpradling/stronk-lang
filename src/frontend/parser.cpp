#include <iostream>
#include "frontend/parser.h"

namespace stronk {

// ========================
// Public Methods
// ========================

// Adds a sinle token to the token list.
void Parser::AddToken(std::shared_ptr<Token> token) {
    tokens_.push_back(std::move(token));
}

// Parses the token list and returns corresponding bytecode.
void Parser::Parse() {
    current_ = tokens_.begin();
    previous_ = tokens_.end();

    while (Peek()->type_ != TokenType::TOKEN_EOF) {
        ParseDeclaration();
    }
}

auto Parser::GetBytecode() -> Bytecode {
    cg_.DissasembleCode();
    return cg_.GetCode();
}

// ========================
// Utility Methods
// ========================


// Grabs next non-error token. Returns false if
// an error occurs and true otherwise.
void Parser::StepForward() {
    previous_ = current_;

    for (;;) {
        current_++;
        if ((*current_)->type_ != TokenType::ERROR) {
            break;
        }

        auto error_token = dynamic_cast<ValueToken<std::string> *>(current_->get());
        if (error_token == nullptr) {
            ErrorAt(*current_, "Unexpected token.");
        } else {
            ErrorAt(*current_, error_token->value_);
        }
    }
}

// Steps forward if the current token matches the passed
// in token type. If it doesn, creates an error with message
// `message`.
void Parser::StepIfMatch(TokenType type, std::string_view message) {
    if ((*current_)->type_ == type) {
        StepForward();
        return;
    }

    ErrorAt(*current_, message);
}

// Gets current token.
auto Parser::Peek() const -> Token * {
    return current_->get();
}

void Parser::Match(TokenType type, std::string_view message) {
    if (Peek()->type_ != type) {
        Error(message);
        return;
    }
}

void Parser::ErrorAt(std::shared_ptr<Token> &token, std::string_view message) {
    if (is_panic_mode_) {
        return;
    }
    is_panic_mode_ = true;
    std::cerr << "[line " << token->line_ << "] Error";

    if (token->type_ == TokenType::TOKEN_EOF) {
        std::cerr << " at end";
    } else if (token->type_ != TokenType::ERROR) {
        std::cerr << " at '";
        token->ToString();
        std::cerr << "'";
    }

    std::cerr << ": " << message << "\n";
    error_occurred_ = true;
}

void Parser::Error(std::string_view message) {
    return Parser::ErrorAt(*previous_, message);
}

// Peeks at current token and extracts value.
template <class T>
auto Parser::ExtractValue(std::string_view message) -> std::optional<T> {
    auto token = dynamic_cast<ValueToken<T> *>(Peek());
    if (token == nullptr) {
        Error(message);
        return {};
    }
    return token->value_;
}

// ========================
// Type Methods
// ========================


// Attempts to convert `source` to type2 if posisble.
auto Parser::ConvertType(Address source, PrimitiveType type2) -> Address {
    if (symbol_table_.find(source) == symbol_table_.end()) {
        Error("Cannot convert type from undefined to <type2>.");
        return source;
    }

    PrimitiveType type1 = symbol_table_[source];

    if (type1 == type2) {
        return source;
    }

    Address dest;
    switch (type2) {
        case PrimitiveType::BOOL:
            Error("Cannot convert <type1> to bool.");
            break;
        case PrimitiveType::INT:
            if (type1 == PrimitiveType::REAL) {
                dest = num_gen_.GenerateTemp();
                EmitInstruction(dest, OpCode::F2I, source);
            } else {
                Error("Cannot convert <type1> to bool.");
            }
            break;
        case PrimitiveType::CHAR:
            Error("Cannot convert <type1> to bool.");
            break;
        case PrimitiveType::REAL:
            if (type1 == PrimitiveType::INT) {
                dest = num_gen_.GenerateTemp();
                EmitInstruction(dest, OpCode::I2F, source);
            } else {
                Error("Cannot convert <type1> to bool.");
            }
            break;
    }
    return dest;
}

auto Parser::GetType(Address source) -> std::optional<PrimitiveType> {
    if (symbol_table_.find(source) == symbol_table_.end()) {
        Error("Cannot get type of undefined.");
        return std::nullopt;
    }
    return symbol_table_[source];
}

// ========================
// Symbol Table Access Methods
// ========================


void Parser::AddToTable(Address dest, PrimitiveType type) {
    if (symbol_table_.find(dest) != symbol_table_.end()) {
        Error("Cannot redeclare.");
    }
    symbol_table_[dest] = type;
}

void Parser::UpdateTable(Address dest, PrimitiveType type) {
    if (symbol_table_.find(dest) == symbol_table_.end()) {
        Error("Cannot find.");
    }
    symbol_table_[dest] = type;
}

// ========================
// Instruction Utility Methods
// ========================


template <typename... Args>
void Parser::EmitInstruction(Address &dest, OpCode op, Args... args) {
    int line = previous_->get()->line_;
    int position = previous_->get()->position_;
    std::vector<Address> args_vec = {args...};
    cg_.AddInstruction(std::make_shared<PureInstr>(op, dest, args_vec, line, position));
}

template <typename... Args>
void Parser::EmitInstruction(OpCode op, Args... args) {
    int line = previous_->get()->line_;
    int position = previous_->get()->position_;
    std::vector<Address> arg_vec = {args...};
    std::vector<Address> label_vec;
    cg_.AddInstruction(std::make_shared<ImpureInstr>(op, arg_vec, label_vec, line, position));
}

void Parser::EmitBr(Address cond, Label label1, Label label2) {
    int line = previous_->get()->line_;
    int position = previous_->get()->position_;
    std::vector<Address> arg_vec = { cond };
    std::vector<Address> label_vec = { label1, label2 };
    cg_.AddInstruction(std::make_shared<ImpureInstr>(OpCode::BR, arg_vec, label_vec, line, position));
}

void Parser::EmitLabel(Label label) {
    int line = previous_->get()->line_;
    int position = previous_->get()->position_;
    cg_.AddInstruction(std::make_shared<LabelInstr>(label, line, position));
}

void Parser::EmitJmp(Label label) {
    int line = previous_->get()->line_;
    int position = previous_->get()->position_;
    std::vector<Address> arg_vec;
    std::vector<Address> label_vec = { label };
    cg_.AddInstruction(std::make_shared<ImpureInstr>(OpCode::JMP, arg_vec, label_vec, line, position));
}

auto Parser::EmitConstInstruction(const ConstantPool::ConstantValue &val, PrimitiveType type) -> Address {
    Address dest = num_gen_.GenerateTemp();
    int line = previous_->get()->line_;
    int position = previous_->get()->position_;
    cg_.AddConstantInstruction(dest, val, line, position);
    AddToTable(dest, type);
    return dest;
}

auto Parser::EmitConstInstruction(Address &dest, const ConstantPool::ConstantValue &val) -> Address {
    int line = previous_->get()->line_;
    int position = previous_->get()->position_;
    cg_.AddConstantInstruction(dest, val, line, position);
    return dest;
}

// ========================
// Parser Methods
// ========================


// Grammar: declaration -> TYPE var_declaration | statement
void Parser::ParseDeclaration() {
    Address var;
    switch (Peek()->type_) {
        case TokenType::PRIMITIVE:
            var = ParseVarDeclaration();
            break;
        default:
            ParseStatement();
            break;
    }
}

// Grammar: var_declaration -> PRIMITIVE IDENTIFIER ( "=" expression )? ";"
auto Parser::ParseVarDeclaration() -> Address {
    Match(TokenType::PRIMITIVE, "Expected typename.");
    PrimitiveType var_type = ExtractValue<PrimitiveType>().value();
    StepForward();

    Match(TokenType::IDENTIFIER, "Expected identifier.");
    std::string dest = ExtractValue<std::string>().value();
    StepForward();

    // Add variable to symbol table.
    if (symbol_table_.find(dest) != symbol_table_.end()) {
        Error("Cannot redeclare variables.");
    }
    symbol_table_[dest] = var_type;

    Address expression;
    ConstantPool::ConstantValue default_val;
    switch (Peek()->type_) {
        case TokenType::EQUAL:
            StepForward();
            expression = ParseExpression();

            StepIfMatch(TokenType::SEMICOLON, "Expected ';'.");

            EmitInstruction(dest, OpCode::ID, expression);
            return dest;
        case TokenType::SEMICOLON:
            switch (var_type) {
                case PrimitiveType::BOOL:
                    default_val = false;
                    break;
                case PrimitiveType::INT:
                    default_val = 0;
                    break;
                case PrimitiveType::REAL:
                    default_val = (float) 0;
                    break;
                case PrimitiveType::CHAR:
                    default_val = (char) 0;
                    break;
            }

            EmitConstInstruction(dest, default_val);
            StepForward();
            return dest;
        default:
            Error("Expected ';' or '='.");
    }
    return dest;
}

// Grammar: statement -> expression_statement | for_statement | if_statement | while_statement | block
void Parser::ParseStatement() {
    switch (Peek()->type_) {
        case TokenType::FOR:
            StepForward();
            ParseForStatement();
            break;
        case TokenType::IF:
            StepForward();
            ParseIfStatement();
            break;
        case TokenType::WHILE:
            StepForward();
            ParseWhileStatement();
            break;
        case TokenType::LEFT_BRACE:
            StepForward();
            ParseBlock();
            break;
        case TokenType::PRINT:
            StepForward();
            ParsePrintStatement();
            break;
        default:
            ParseExprStatement();
            break;
    }
}

void Parser::ParseForStatement() {}

// Grammar: "if" "(" expression ")" statement ( "else" statement )?
void Parser::ParseIfStatement() {
    if (previous_->get()->type_ != TokenType::IF) {
        throw std::invalid_argument("Incorrect usage of ParseIfStatement.");
    }

    StepIfMatch(TokenType::LEFT_PAREN, "Expected '('.");

    std::string if_num = std::to_string(control_flow_gen_.GenerateNumber());
    Label true_branch = ".if_" + if_num + ".true";
    Label false_branch = ".if_" + if_num + ".false";
    Label exit_branch = ".if_" + if_num + ".exit";

    Address cond = ParseExpression();

    if (symbol_table_[cond] != PrimitiveType::BOOL) {
        Error("If statements must take booleans.");
    }

    EmitBr(cond, true_branch, false_branch);
    EmitLabel(true_branch);

    StepIfMatch(TokenType::RIGHT_PAREN, "Expected ')'.");

    ParseStatement();
    
    if (Peek()->type_ != TokenType::ELSE) {
        // Cut off early and use false branch as the exit label
        EmitLabel(false_branch);
        return;
    }

    StepForward();

    EmitJmp(exit_branch);

    EmitLabel(false_branch);
    ParseStatement();
    EmitLabel(exit_branch);
}

// Grammar: "while" "(" expression ")" statement
void Parser::ParseWhileStatement() {
    if (previous_->get()->type_ != TokenType::WHILE) {
        throw std::invalid_argument("Incorrect usage of ParseWhileStatement.");
    }

    StepIfMatch(TokenType::LEFT_PAREN, "Expected '('.");

    std::string while_num = std::to_string(control_flow_gen_.GenerateNumber());
    Label condition_label = ".while_" + while_num + ".cond";
    Label true_label = ".while_" + while_num + ".true";
    Label exit_label = ".while_" + while_num + ".exit";

    EmitLabel(condition_label);

    Address cond = ParseExpression();

    if (symbol_table_[cond] != PrimitiveType::BOOL) {
        Error("While loops must take booleans.");
    }

    StepIfMatch(TokenType::RIGHT_PAREN, "Expected ')'.");

    EmitBr(cond, true_label, exit_label);

    EmitLabel(true_label);
    ParseStatement();
    EmitJmp(condition_label);

    EmitLabel(exit_label);
}

// Grammar: print_statement -> "print" expression ";"
void Parser::ParsePrintStatement() {
    if (previous_->get()->type_ != TokenType::PRINT) {
        throw std::invalid_argument("Incorrect usage of ParsePrintStatement.");
    }

    Address dest = ParseExpression();

    StepIfMatch(TokenType::SEMICOLON, "Expected ';'.");
    EmitInstruction(OpCode::PRINT, dest);
}

// Grammar: block_statement -> "{" declaration* "}"
void Parser::ParseBlock() {
    if (previous_->get()->type_ != TokenType::LEFT_BRACE) {
        throw std::invalid_argument("Incorrect usage of ParseBlock.");
    }

    while ( Peek()->type_ != TokenType::RIGHT_BRACE && Peek()->type_ != TokenType::TOKEN_EOF ) {
        ParseDeclaration();
    }

    StepIfMatch(TokenType::RIGHT_BRACE, "Expected '}'.");
}

// Grammar: expr_statement -> expression ";"
void Parser::ParseExprStatement() {
    ParseExpression();

    StepIfMatch(TokenType::SEMICOLON, "Expected ';'.");
}

// Grammar: expression -> assignment
auto Parser::ParseExpression() -> Address {
    return ParseAssignment();
}

// Grammar: assignment -> IDENTIFER "=" assignment | logic_or
auto Parser::ParseAssignment() -> Address {
    if ((current_ + 1)->get()->type_ != TokenType::EQUAL) {
        return ParseLogicOr();
    }

    auto dest = ExtractValue<std::string>().value();
    StepForward();

    StepIfMatch(TokenType::EQUAL, "Expected '='.");

    Address assignment = ParseAssignment();

    if (symbol_table_.find(dest) == symbol_table_.end()) {
        Error("Cannot assign to undefined variable.");
    }
    Address converted = ConvertType(assignment, symbol_table_[dest]);

    EmitInstruction(dest, OpCode::ID, converted);
    return dest;
}

// Grammar: logic_or -> logic_and ( "or" logic_and )*
auto Parser::ParseLogicOr() -> Address {
    Address dest = ParseLogicAnd();
    for(;;) {
        Token &tok = **current_;
        Address a;
        Address b;
        
        Address converted_a;
        Address converted_b;

        switch (tok.type_) {
            case TokenType::OR:
                StepForward();
                a = dest;
                b = ParseLogicAnd();
                converted_a = ConvertType(a, PrimitiveType::BOOL);
                converted_b = ConvertType(b, PrimitiveType::BOOL);
            
                dest = num_gen_.GenerateTemp();
                AddToTable(dest, PrimitiveType::BOOL);
            
                EmitInstruction(dest, OpCode::OR, converted_a, converted_b);
                break;
            default: return dest;
        }
    }
}

// Grammar: logic_and -> equality ( "and" equality )*
auto Parser::ParseLogicAnd() -> Address {
    Address dest = ParseEquality();
    for(;;) {
        Token &tok = **current_;
        Address a;
        Address b;
        Address converted_a;
        Address converted_b;

        switch (tok.type_) {
            case TokenType::AND:
                StepForward();
                a = dest;
                b = ParseEquality();
                converted_a = ConvertType(a, PrimitiveType::BOOL);
                converted_b = ConvertType(b, PrimitiveType::BOOL);
            
                dest = num_gen_.GenerateTemp();
                AddToTable(dest, PrimitiveType::BOOL);
            
                EmitInstruction(dest, OpCode::AND, converted_a, converted_b);
                break;
            default: return dest;
        }
    }
    return dest;
}

// Grammar: equality -> comparison ( ( "!=" | "==" ) comparison )*
auto Parser::ParseEquality() -> Address {
    Address dest = ParseComparison();
    for(;;) {
        Token &tok = **current_;
        Address a;
        Address b;

        switch (tok.type_) {
            case TokenType::EQUAL_EQUAL:
                StepForward();
                a = dest;
                b = ParseComparison();
                if (GetType(a).value() != GetType(b).value()) {
                    Error("Checking equality is only possible on equal types.");
                }
            
                dest = num_gen_.GenerateTemp();
                AddToTable(dest, PrimitiveType::BOOL);
            
                EmitInstruction(dest, OpCode::EQ, a, b);
                break;
            case TokenType::BANG_EQUAL:
                StepForward();
                a = dest;
                b = ParseComparison();
                if (GetType(a).value() != GetType(b).value()) {
                    Error("Checking equality is only possible on equal types.");
                }
            
                dest = num_gen_.GenerateTemp();
                AddToTable(dest, PrimitiveType::BOOL);
            
                EmitInstruction(dest, OpCode::NEQ, a, b);
                break;
            default: return dest;
        }
    }
    return dest;
}

// Grammar: comparison -> term ( (">" | ">=" | "<" | "<=" ) term )*
auto Parser::ParseComparison() -> Address {
    Address dest = ParseTerm();
    for(;;) {
        Token &tok = **current_;
        Address a;
        Address b;

        switch (tok.type_) {
            case TokenType::GREATER:
                StepForward();
                a = dest;
                b = ParseTerm();
                if (GetType(a).value() != GetType(b).value()) {
                    Error("Checking equality is only possible on equal types.");
                }
                if (GetType(a).value() != PrimitiveType::INT && GetType(a).value() != PrimitiveType::REAL) {
                    Error("Comparison is only possible between integers or floats.");
                }
            
                dest = num_gen_.GenerateTemp();
                AddToTable(dest, PrimitiveType::BOOL);
            
                EmitInstruction(dest, OpCode::GT, a, b);
                break;
            case TokenType::GREATER_EQUAL:
                StepForward();
                a = dest;
                b = ParseTerm();
                if (GetType(a).value() != GetType(b).value()) {
                    Error("Checking equality is only possible on equal types.");
                }
                if (GetType(a).value() != PrimitiveType::INT && GetType(a).value() != PrimitiveType::REAL) {
                    Error("Comparison is only possible between integers or floats.");
                }
            
                dest = num_gen_.GenerateTemp();
                AddToTable(dest, PrimitiveType::BOOL);
            
                EmitInstruction(dest, OpCode::GEQ, a, b);
                break;
            case TokenType::LESS:
                StepForward();
                a = dest;
                b = ParseTerm();
                if (GetType(a).value() != GetType(b).value()) {
                    Error("Checking equality is only possible on equal types.");
                }
                if (GetType(a).value() != PrimitiveType::INT && GetType(a).value() != PrimitiveType::REAL) {
                    Error("Comparison is only possible between integers or floats.");
                }
            
                dest = num_gen_.GenerateTemp();
                AddToTable(dest, PrimitiveType::BOOL);
            
                EmitInstruction(dest, OpCode::LT, a, b);
                break;
            case TokenType::LESS_EQUAL:
                StepForward();
                a = dest;
                b = ParseTerm();
                if (GetType(a).value() != GetType(b).value()) {
                    Error("Checking equality is only possible on equal types.");
                }
                if (GetType(a).value() != PrimitiveType::INT && GetType(a).value() != PrimitiveType::REAL) {
                    Error("Comparison is only possible between integers or floats.");
                }
            
                dest = num_gen_.GenerateTemp();
                AddToTable(dest, PrimitiveType::BOOL);
            
                EmitInstruction(dest, OpCode::LEQ, a, b);
                break;
            default: return dest;
        }
    }
    return dest;
}

// Grammar: term -> factor ( (+ | -) factor )*
auto Parser::ParseTerm() -> Address {
    Address dest = ParseFactor();
    for (;;) {
        Token &tok = **current_;
        Address a;
        Address b;
        Address converted_a;
        Address converted_b;

        switch (tok.type_) {
            case TokenType::PLUS:
                StepForward();
                a = dest;
                b = ParseFactor();
                if (GetType(a).value() != PrimitiveType::INT && GetType(a).value() != PrimitiveType::REAL) {
                    Error("Addition is only possible between integers or floats.");
                }

                // Convert to float if one of the two are floats.
                if (GetType(a).value() != GetType(b).value()) {
                    converted_a = ConvertType(a, PrimitiveType::REAL);
                    converted_b = ConvertType(b, PrimitiveType::REAL);

                    dest = num_gen_.GenerateTemp();
                    AddToTable(dest, PrimitiveType::REAL);

                    EmitInstruction(dest, OpCode::FADD, converted_a, converted_b);
                } else {
                    dest = num_gen_.GenerateTemp();
                    AddToTable(dest, PrimitiveType::INT);
                    EmitInstruction(dest, OpCode::ADD, a, b);
                }
                break;
            case TokenType::MINUS:
                StepForward();
                a = dest;
                b = ParseFactor();
                if (GetType(a).value() != PrimitiveType::INT && GetType(a).value() != PrimitiveType::REAL) {
                    Error("Subtraction is only possible between integers or floats.");
                }

                // Convert to float if one of the two are floats.
                if (GetType(a).value() != GetType(b).value()) {
                    converted_a = ConvertType(a, PrimitiveType::REAL);
                    converted_b = ConvertType(b, PrimitiveType::REAL);

                    dest = num_gen_.GenerateTemp();
                    AddToTable(dest, PrimitiveType::REAL);

                    EmitInstruction(dest, OpCode::FSUB, converted_a, converted_b);
                } else {
                    dest = num_gen_.GenerateTemp();
                    AddToTable(dest, PrimitiveType::INT);
                    EmitInstruction(dest, OpCode::SUB, a, b);
                }
                break;
            default: return dest;
        }
    }
    return dest;
}

// Grammar: factor -> unary ( (* | /) unary )*
auto Parser::ParseFactor() -> Address {
    Address dest = ParseUnary();
    for (;;) {
        Token &tok = **current_;
        Address a;
        Address b;
        Address converted_a;
        Address converted_b;

        switch (tok.type_) {
            case TokenType::STAR:
                StepForward();
                a = dest;
                b = ParseUnary();
                if (GetType(a).value() != PrimitiveType::INT && GetType(a).value() != PrimitiveType::REAL) {
                    Error("Multiplication is only possible between integers or floats.");
                }

                // Convert to float if one of the two are floats.
                if (GetType(a).value() != GetType(b).value()) {
                    converted_a = ConvertType(a, PrimitiveType::REAL);
                    converted_b = ConvertType(b, PrimitiveType::REAL);

                    dest = num_gen_.GenerateTemp();
                    AddToTable(dest, PrimitiveType::REAL);

                    EmitInstruction(dest, OpCode::FMULT, converted_a, converted_b);
                } else {
                    dest = num_gen_.GenerateTemp();
                    AddToTable(dest, PrimitiveType::INT);
                    EmitInstruction(dest, OpCode::MULT, a, b);
                }
                break;
            case TokenType::SLASH:
                StepForward();
                a = dest;
                b = ParseUnary();
                if (GetType(a).value() != PrimitiveType::INT && GetType(a).value() != PrimitiveType::REAL) {
                    Error("Division is only possible between integers or floats.");
                }

                // Convert to float if one of the two are floats.
                if (GetType(a).value() != GetType(b).value()) {
                    converted_a = ConvertType(a, PrimitiveType::REAL);
                    converted_b = ConvertType(b, PrimitiveType::REAL);

                    dest = num_gen_.GenerateTemp();
                    AddToTable(dest, PrimitiveType::REAL);

                    EmitInstruction(dest, OpCode::FDIV, converted_a, converted_b);
                } else {
                    dest = num_gen_.GenerateTemp();
                    AddToTable(dest, PrimitiveType::INT);
                    EmitInstruction(dest, OpCode::DIV, a, b);
                }
                break;
            default:
                return dest;
        }
    }
    return dest;
}

// Grammar: unary -> - unary | primary
auto Parser::ParseUnary() -> Address {
    TokenType op = current_->get()->type_;

    if (op == TokenType::MINUS) {
        StepForward();

        Address a = ParseUnary();

        if (GetType(a).value() != PrimitiveType::INT && GetType(a).value() != PrimitiveType::REAL) {
            Error("Negation is only possible on integers or floats.");
        }
        
        Address temp = EmitConstInstruction(0, GetType(a).value());
        Address dest = num_gen_.GenerateTemp();
        AddToTable(dest, GetType(a).value());
        
        EmitInstruction(dest, OpCode::SUB, temp, a);
        return dest;
    }
    return ParsePrimary();
}

// Grammar:
//      primary -> TRUE | FALSE | INT | REAL | IDENTIFIER
//              | "(" expression ")"
auto Parser::ParsePrimary() -> Address {
    TokenType a = current_->get()->type_;

    Address dest;
    ValueToken<std::string> *b;

    ValueToken<float> *value_float;
    ValueToken<int> *value_int;

    switch (a) {
        case TokenType::TRUE:
            StepForward();
            return EmitConstInstruction(true, PrimitiveType::BOOL);
        case TokenType::FALSE:
            StepForward();
            return EmitConstInstruction(false, PrimitiveType::BOOL);
        case TokenType::REAL:
            StepForward();
            value_float = dynamic_cast<ValueToken<float> *>(previous_->get());
            if (value_float == nullptr) {
                ErrorAt(*previous_, "Expected float.");
            } else {
                return EmitConstInstruction(value_float->value_, PrimitiveType::REAL);
            }
            break;
        case TokenType::INT:
            StepForward();
            value_int = dynamic_cast<ValueToken<int> *>(previous_->get());
            if (value_int == nullptr) {
                 ErrorAt(*previous_, "Expected integer.");
            } else {
                return EmitConstInstruction(value_int->value_, PrimitiveType::INT);
            }
            break;
        case TokenType::QUOTE:
            StepForward();
            dest = ParseString();
            break;
        case TokenType::IDENTIFIER:
            StepForward();
            b = dynamic_cast<ValueToken<std::string> *>(previous_->get());
            if (b == nullptr) {
                ErrorAt(*previous_, "Unexpected token.");
            } else {
                dest = b->value_;
            }
            break;
        case TokenType::LEFT_PAREN:
            StepForward();
            dest = ParseExpression();
            if (current_->get()->type_ != TokenType::RIGHT_PAREN) {
                ErrorAt(*previous_, "Expected end of parentheses.");
            }
            StepForward();
            break;
        default:
            return dest;
    }
    return dest;
}

// Grammar:
// string -> ( TEXT | "${" expression "}" )* QUOTE
auto Parser::ParseString() -> Address {
    // TODO: Add support back for strings.
    Address res;
    return res;

    // Address dest;
    // if (current_->get()->type_ == TokenType::QUOTE) {
    //     return EmitConstInstruction("");
    // }
    // for(;;) {
    //     Token &tok = **current_;
    //     Address a;
    //     Address b;

    //     ValueToken<std::string> *value;

    //     switch (tok.type_) {
    //         case TokenType::TEXT:
    //             StepForward();
    //             value = dynamic_cast<ValueToken<std::string> *>(previous_->get());
    //             if (value == nullptr) {
    //                 ErrorAt(*previous_, "Expected string.");
    //             } else if (dest.empty()) {
    //                 dest = EmitConstInstruction(value->value_);
    //             } else {
    //                 a = dest;
    //                 b = EmitConstInstruction(value->value_);

    //                 dest = num_gen_.GenerateTemp();
    //                 EmitInstruction(dest, OpCode::CONCAT, a, b);
    //             }
    //             break;
    //         case TokenType::DOLLAR_BRACE:
    //             StepForward();
    //             if (dest.empty()) {
    //                 a = ParseExpression();
    //                 dest = num_gen_.GenerateTemp();
    //                 EmitInstruction(dest, OpCode::TO_STRING, a);
    //             } else {
    //                 a = dest;
    //                 b = ParseExpression();
    //                 dest = num_gen_.GenerateTemp();
    //                 EmitInstruction(dest, OpCode::TO_STRING, b);

    //                 b = dest;

    //                 dest = num_gen_.GenerateTemp();
    //                 EmitInstruction(dest, OpCode::CONCAT, a, b);
    //             }
    //             if (current_->get()->type_ != TokenType::RIGHT_BRACE) {
    //                 ErrorAt(*current_, "Expected right brace '}' after interpolation.");
    //             }
    //             StepForward();
    //             break;
    //         case TokenType::QUOTE:
    //             StepForward();
    //             [[fallthrough]];
    //         default: return dest;
    //     }
    // }
}

} // namespace "stronk"

#undef PARSE_CASE