#ifndef _STRONK_INSTRUCTION_H
#define _STRONK_INSTRUCTION_H

#include <string>
#include <vector>
#include <memory>

using Address = std::string;
using Label = std::string;

enum class OpCode {
    NEGATE, ADD, SUB, MULT, DIV,
    GEQ, LEQ, GT, LT, EQ, NEQ,
    OR, AND, NOT,

    ID, 

    TO_STRING, CONCAT,
    LABEL,
    CALL,
    PHI,
    CONST,

    PRINT,
    JMP,
    BR,
};

struct Instr {
    OpCode code_;
    int line_;
    int pos_;

    explicit Instr(OpCode code, int line, int pos) : code_(code), line_(line), pos_(pos) {}
    virtual ~Instr() = default;

    virtual bool operator==(const std::shared_ptr<Instr>& other) const {
        return (code_ == other->code_);
    }

    virtual auto ToString() const -> std::string {
        switch (code_) {
            case OpCode::ADD: return "ADD";
            case OpCode::NEGATE: return "NEGATE";
            case OpCode::SUB: return "SUB";
            case OpCode::MULT: return "MULT";
            case OpCode::DIV: return "DIV";
            case OpCode::GEQ: return "GEQ";
            case OpCode::LEQ: return "LEQ";
            case OpCode::GT: return "GT";
            case OpCode::LT: return "LT";
            case OpCode::EQ: return "EQ";
            case OpCode::NEQ: return "NEQ";
            case OpCode::OR: return "OR";
            case OpCode::AND: return "AND";
            case OpCode::NOT: return "NOT";
            case OpCode::ID: return "ID";
            case OpCode::TO_STRING: return "TO_STRING";
            case OpCode::CONCAT: return "CONCAT";
            case OpCode::LABEL: return "LABEL";
            case OpCode::CALL: return "CALL";
            case OpCode::PHI: return "PHI";
            case OpCode::CONST: return "CONST";
            case OpCode::PRINT: return "PRINT";
            case OpCode::JMP: return "JMP";
            case OpCode::BR: return "BR";
            default: return "UNKNOWN_INSTR";
        };
    };
};

// These are instructions that indicate labelings.
struct LabelInstr : public Instr {
    Label label_;

    explicit LabelInstr(Label &label, int line, int pos) : Instr(OpCode::LABEL, line, pos), label_(label) {}

    bool operator==(const std::shared_ptr<Instr>& other) const override {
        if (auto other_label = std::dynamic_pointer_cast<LabelInstr>(other)) {
            return (code_ == other->code_) &&
                   (label_ == other_label->label_);
        }
        return false;
    }

    auto ToString() const -> std::string override {
        std::string res = Instr::ToString();
        res += " " + label_;
        return res;
    }
};

// Effect-based instructions that cannot have a destination.
struct ImpureInstr : public Instr {
    std::vector<Label> labels_;
    std::vector<Address> args_;

    ImpureInstr(OpCode code, std::vector<Address> &args, std::vector<Label> &labels, int line, int pos) : Instr(code, line, pos), labels_(labels), args_(args) {}

    bool operator==(const std::shared_ptr<Instr>& other) const override {
        if (auto other_impure = std::dynamic_pointer_cast<ImpureInstr>(other)) {
            return (code_ == other->code_) &&
                   (labels_ == other_impure->labels_) && (args_ == other_impure->args_);
        }
        return false;
    }

    auto ToString() const -> std::string override {
        std::string res = Instr::ToString() + " ";

        if (!labels_.empty()) {
            res += "[";
            for (const auto &label : labels_) {
                res += label + ", ";
            }
            res += "]";
        }
        res += " ";
        for (const auto &arg : args_) {
            res += arg + ", ";
        }

        return res;
    }
};

// Call instruction is specifically reserved for function calls.
struct CallInstr : public Instr {
    Address dest_;
    int func_;
    std::vector<Address> args_;

    CallInstr(Address &dest, std::vector<Address> &args, int func, int line, int pos) : Instr(OpCode::CALL, line, pos), dest_(dest), func_(func), args_(args) {}

    bool operator==(const std::shared_ptr<Instr>& other) const override {
        if (auto other_call = std::dynamic_pointer_cast<CallInstr>(other)) {
            return (code_ == other->code_) &&
                   (dest_ == other_call->dest_) && (func_ == other_call->func_) && (args_ == other_call->args_);
        }
        return false;
    }

    auto ToString() const -> std::string override {
        std::string res;

        if (!dest_.empty()) {
            res += dest_ + " = ";
        }

        res += Instr::ToString() + " " + std::to_string(func_) + "(";
        for (const auto &arg : args_) {
            res += arg + ", ";
        }
        res += ")";
        return res;
    }
};

// These are instructions that are pure functions of
// their arguments and have one destination.
struct PureInstr : public Instr {
    Address dest_;
    std::vector<Address> args_;

    PureInstr(OpCode code, Address &dest, std::vector<Address> &args, int line, int pos) : Instr(code, line, pos), dest_(dest), args_(args) {}
    
    bool operator==(const std::shared_ptr<Instr>& other) const override {
        if (auto other_pure = std::dynamic_pointer_cast<PureInstr>(other)) {
            return (code_ == other->code_) &&
                   (dest_ == other_pure->dest_) && (args_ == other_pure->args_);
        }
        return false;
    }

    auto ToString() const -> std::string override {
        std::string res = dest_ + " = " + Instr::ToString() + " ";
        for (const auto &arg : args_) {
            res += arg + ", ";
        }
        return res;
    }
};

// Phi decision function for static-single analysis.
struct PhiInstr : public Instr {
    Address dest_;
    std::vector<Label> labels_;
    std::vector<Address> args_;

    PhiInstr(Address &dest, std::vector<Address> &args, std::vector<Label> &labels, int line, int pos) : Instr(OpCode::PHI, line, pos), dest_(dest), labels_(labels), args_(args) {}

    bool operator==(const std::shared_ptr<Instr>& other) const override {
        if (auto other_phi = std::dynamic_pointer_cast<PhiInstr>(other)) {
            return (code_ == other->code_) &&
                   (dest_ == other_phi->dest_) && (labels_ == other_phi->labels_) && (args_ == other_phi->args_);
        }
        return false;
    }

    auto ToString() const -> std::string override {
        std::string res = dest_ + " = " + Instr::ToString() + " ";

        if (!labels_.empty()) {
            res += "[";
            for (const auto &label : labels_) {
                res += label + ", ";
            }
            res += "]";
        }
        res += " ";
        for (const auto &arg : args_) {
            res += arg + ", ";
        }

        return res;
    }
};

// Constant instructions associate values to variables.
struct ConstInstr : public Instr {
    Address dest_;
    int index_;

    ConstInstr(const Address &dest, int index, int line, int pos) : Instr(OpCode::CONST, line, pos), dest_(dest), index_(index) {}

    bool operator==(const std::shared_ptr<Instr>& other) const override {
        if (auto other_const = std::dynamic_pointer_cast<ConstInstr>(other)) {
            return (code_ == other->code_) &&
                   (dest_ == other_const->dest_) && (index_ == other_const->index_);
        }
        return false;
    }

    auto ToString() const -> std::string override {
        return dest_ + " = " + Instr::ToString() + " " + std::to_string(index_);
    }
};

bool operator==(const std::shared_ptr<Instr> &instr1, const std::shared_ptr<Instr> &instr2);

#endif // _STRONK_INSTRUCTION_H