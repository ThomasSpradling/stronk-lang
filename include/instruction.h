#ifndef _STRONK_INSTRUCTION_H
#define _STRONK_INSTRUCTION_H

#include <string>

using Address = std::string;
using Label = std::string;

enum class OpCode {
    ADD,
    LABEL,
    CALL,
    PHI,
    CONST
};

struct Instr {
    OpCode code_;

    explicit Instr(OpCode code) : code_(code) {}

    virtual auto ToString() const -> std::string {
        switch (code_) {
            case OpCode::ADD: return "ADD";
            default: return "UNKNOWN_INSTR";
        };
    };
};

// These are instructions that indicate labelings.
struct LabelInstr : public Instr {
    Label label_;

    explicit LabelInstr(Label &label) : Instr(OpCode::LABEL), label_(label) {}

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

    ImpureInstr(OpCode code, std::vector<Address> &args, std::vector<Label> &labels) : Instr(code), labels_(labels), args_(args) {}

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

    CallInstr(Address &dest, std::vector<Address> &args, int func) : Instr(OpCode::CALL), args_(args), func_(func), dest_(dest) {}

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

    PureInstr(OpCode code, Address &dest, std::vector<Address> &args) : Instr(code), dest_(dest), args_(args) {}
    
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

    PhiInstr(Address &dest, std::vector<Address> &args, std::vector<Label> &labels) : Instr(OpCode::PHI), dest_(dest), labels_(labels), args_(args) {}

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
    int val_;

    ConstInstr(Address &dest, int val) : Instr(OpCode::CONST), dest_(dest), val_(val) {}

    auto ToString() const -> std::string override {
        return dest_ + " = " + Instr::ToString() + " " + std::to_string(val_);
    }
};

#endif // _STRONK_INSTRUCTION_H