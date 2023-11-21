#ifndef _STRONK_INSTRUCTION_H
#define _STRONK_INSTRUCTION_H

#include <string>
#include <vector>
#include <memory>

namespace stronk {
    
using Address = std::string;
using Label = std::string;

enum class OpCode {
    //// Arithmetic ////

    /** ADD x y
     * Args: x (int), y (int).
     * Result: Adds the two numbers and returns the result.
    */
    ADD,

    /** SUB x y
     * Args: x (int), y (int).
     * Result: Subtracts the two numbers and returns a number.
    */
    SUB,

    /** MULT x y
     * Args: x (int), y (int).
     * Result: Multplies the two numbers and returns a number.
    */
    MULT,

    /** DIV x y
     * Args: x (int), y (int).
     * Result: Divides the two numbers and returns a number.
    */
    DIV,

    //// Float arithmetic ////

    /** FADD x y
     * Args: x (float), y (float).
     * Result: Adds the two numbers and returns the result.
    */
    FADD,

    /** FSUB x y
     * Args: x (float), y (float).
     * Result: Subtracts the two numbers and returns a number.
    */
    FSUB,

    /** FMULT x y
     * Args: x (float), y (float).
     * Result: Multplies the two numbers and returns a number.
    */
    FMULT,

    /** FDIV x y
     * Args: x (float), y (float).
     * Result: Divides the two numbers and returns a number.
    */
    FDIV,

    //// Comparison ////
    
    /** EQ x y
     * Args: x (int), y (int).
     * Result: Returns true iff the two numbers are equal.
    */
    EQ,

    /** GT x y
     * Args: x (int), y (int).
     * Result: Returns true iff x is greater than y.
    */
    GT,

    /** LT x y
     * Args: x (int), y (int).
     * Result: Returns true iff x is less than y.
    */
    LT,

    /** GEQ x y
     * Args: x (int), y (int).
     * Result: Returns true iff x is greater than or equal to y.
    */
    GEQ,

    /** LEQ x y
     * Args: x (int), y (int).
     * Result: Returns true iff x is less than or equal to y.
    */
    LEQ,

    /** NEQ x y
     * Arg: x (int), y (int).
     * Result: Returns true iff x is not equal to y.
    */
    NEQ,

    //// Float Comparisons ////
    
    /** FEQ x y
     * Args: x (float), y (float).
     * Result: Returns true iff the two numbers are equal.
    */
    FEQ,

    /** FGT x y
     * Args: x (float), y (float).
     * Result: Returns true iff x is greater than y.
    */
    FGT,

    /** FLT x y
     * Args: x (float), y (float).
     * Result: Returns true iff x is less than y.
    */
    FLT,

    /** FGEQ x y
     * Args: x (float), y (float).
     * Result: Returns true iff x is greater than or equal to y.
    */
    FGEQ,

    /** FLEQ x y
     * Args: x (float), y (float).
     * Result: Returns true iff x is less than or equal to y.
    */
    FLEQ,

    /** FNEQ x y
     * Arg: x (float), y (float).
     * Result: Returns true iff x is not equal to y.
    */
    FNEQ,

    //// Type Conversion //

    /** F2I x
     * Arg: x (float)
     * Result: Places integer conversion into <dest>.
    */
   F2I,

    /** I2F x
     * Arg: x (float)
     * Result: Places integer conversion into <dest>.
    */
   I2F,

    //// Logic ////

    /** NOT x
     * Arg: x (int).
     * Result: Returns ~x.
    */
    NOT,

    /** AND x y
     * Arg: x (int), y (int).
     * Result: Returns x & y.
    */
    AND,

    /** OR x y
     * Arg: x (int), y (int).
     * Result: Returns x | y.
    */
    OR,

    /** XOR x y
     * Arg: x (int), y (int).
     * Result: Returns x ^ y.
    */
    XOR,

    //// Type Conversion ////
    /** I2F x
     * Arg: x (int).
     * Result: Returns float.
     */

    //// Control Flow ////

    /** LABEL L
     * Label: L
     * Result: A label to reference a jumpable address.
    */
    LABEL,

    /** JMP L
     * Label: L
     * Result: Jumps to the label L.
    */
    JMP,

    /** BR x, L1, L2
     * Arg: x (boolean)
     * Labels: L1, L2
     * Result: If x is true, jumps to L1. Otherwise, jumps to L2.
    */
    BR,

    /** TODO: CALL */
    CALL,

    /** TODO: RET */
    RET,

    //// Misc ////
    
    /** ID x
     * Arg: x (any value)
     * Result: Returns the passed in value.
    */
    ID,

    /** PRINT a1, a2, ...
     * Args: a1 (any value), a2 (any value), ...
     * Result: Prints value to console with a newline.
    */
    PRINT, // TODO: Only for testing purposes. Remove.

    //// Static Single Assignment (SSA) ////

    /** PHI L1, L2, ..., LN, x1, x2, ..., xN
     * Args: xi (any value)
     * Labels: Li
     * Result: Let j be the second most recently executed label. This
     * instruction will return the xj.
    */
    PHI,

    //// Constants ////

    /** CONST v
     * Args: v (value index)
     * Result: Gets a value from constant pool and places it into <dest>.
    */
    CONST
};

struct Instr {
    OpCode code_;
    int line_;
    int pos_;

    Instr(OpCode code, int line, int pos) : code_(code), line_(line), pos_(pos) {}
    virtual ~Instr() = default;

    virtual auto operator==(const Instr &other) const -> bool {
        return code_ == other.code_;
    }

    virtual auto ToString() const -> std::string {
        switch (code_) {
            case OpCode::ADD: return "ADD";
            case OpCode::SUB: return "SUB";
            case OpCode::MULT: return "MULT";
            case OpCode::DIV: return "DIV";
            case OpCode::FADD: return "fADD";
            case OpCode::FSUB: return "fSUB";
            case OpCode::FMULT: return "fMULT";
            case OpCode::FDIV: return "fDIV";

            case OpCode::F2I: return "FLOAT -> INT";
            case OpCode::I2F: return "INT -> FLOAT";

            case OpCode::EQ: return "EQ";
            case OpCode::GT: return "GT";
            case OpCode::LT: return "LT";
            case OpCode::GEQ: return "GEQ";
            case OpCode::LEQ: return "LEQ";
            case OpCode::NEQ: return "NEQ";
            case OpCode::FEQ: return "fEQ";
            case OpCode::FGT: return "fGT";
            case OpCode::FLT: return "fLT";
            case OpCode::FGEQ: return "fGEQ";
            case OpCode::FLEQ: return "fLEQ";
            case OpCode::FNEQ: return "fNEQ";

            case OpCode::NOT: return "NOT";
            case OpCode::AND: return "AND";
            case OpCode::OR: return "OR";
            case OpCode::XOR: return "XOR";

            case OpCode::LABEL: return "LABEL";
            case OpCode::JMP: return "JMP";
            case OpCode::BR: return "BR";
            case OpCode::CALL: return "CALL";
            case OpCode::RET: return "RET";

            case OpCode::ID: return "ID";
            case OpCode::PRINT: return "PRINT";

            case OpCode::PHI: return "PHI";

            case OpCode::CONST: return "CONST";

            default: return "UNKNOWN_INSTR";
        };
    };
};

// These are instructions that indicate labelings.
struct LabelInstr : public Instr {
    Label label_;

    explicit LabelInstr(Label &label, int line, int pos) : Instr(OpCode::LABEL, line, pos), label_(label) {}

    auto operator==(const Instr &other) const -> bool override {
        if (auto instr = dynamic_cast<const LabelInstr *>(&other)) {
            return (code_ == other.code_) && (label_ == instr->label_);
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

    virtual auto operator==(const Instr &other) const -> bool override {
        if (auto instr = dynamic_cast<const ImpureInstr *>(&other)) {
            return (code_ == other.code_) && (labels_ == instr->labels_) && (args_ == instr->args_);
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

    auto operator==(const Instr &other) const -> bool override {
        if (auto instr = dynamic_cast<const CallInstr *>(&other)) {
            return (code_ == other.code_) && (dest_ == instr->dest_) && (func_ == instr->func_) && (args_ == instr->args_);
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
    
    auto operator==(const Instr &other) const -> bool override {
        if (auto instr = dynamic_cast<const PureInstr *>(&other)) {
            return (code_ == other.code_) &&
                   (dest_ == instr->dest_) && (args_ == instr->args_);
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

    auto operator==(const Instr &other) const -> bool override {
        if (auto instr = dynamic_cast<const PhiInstr *>(&other)) {
            return (code_ == other.code_) &&
                   (dest_ == instr->dest_) && (labels_ == instr->labels_) && (args_ == instr->args_);
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

    auto operator==(const Instr &other) const -> bool override {
        if (auto instr = dynamic_cast<const ConstInstr *>(&other)) {
            return (code_ == other.code_) &&
                   (dest_ == instr->dest_) && (index_ == instr->index_);
        }
        return false;
    }

    auto ToString() const -> std::string override {
        return dest_ + " = " + Instr::ToString() + " " + std::to_string(index_);
    }
};

} // namespace "stronk"

#endif // _STRONK_INSTRUCTION_H