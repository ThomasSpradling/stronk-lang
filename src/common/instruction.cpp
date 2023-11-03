#include "common/instruction.h"

bool operator==(const std::shared_ptr<Instr> &instr1, const std::shared_ptr<Instr> &instr2) {
    if (instr1->code_ == instr2->code_ &&
        instr1->line_ == instr2->line_ &&
        instr1->pos_ == instr2->pos_) {

        switch (instr1->code_) {
            case OpCode::LABEL: {
                auto labelInstr1 = std::static_pointer_cast<LabelInstr>(instr1);
                auto labelInstr2 = std::static_pointer_cast<LabelInstr>(instr2);
                return labelInstr1->label_ == labelInstr2->label_;
            }
            case OpCode::CALL: {
                auto callInstr1 = std::static_pointer_cast<CallInstr>(instr1);
                auto callInstr2 = std::static_pointer_cast<CallInstr>(instr2);
                return callInstr1->dest_ == callInstr2->dest_ &&
                       callInstr1->func_ == callInstr2->func_ &&
                       callInstr1->args_ == callInstr2->args_;
            }
            case OpCode::PHI: {
                auto phiInstr1 = std::static_pointer_cast<PhiInstr>(instr1);
                auto phiInstr2 = std::static_pointer_cast<PhiInstr>(instr2);
                return phiInstr1->dest_ == phiInstr2->dest_ &&
                       phiInstr1->labels_ == phiInstr2->labels_ &&
                       phiInstr1->args_ == phiInstr2->args_;
            }
            case OpCode::CONST: {
                auto constInstr1 = std::static_pointer_cast<ConstInstr>(instr1);
                auto constInstr2 = std::static_pointer_cast<ConstInstr>(instr2);
                return constInstr1->dest_ == constInstr2->dest_ &&
                       constInstr1->index_ == constInstr2->index_;
            }
            case OpCode::NEGATE:
            case OpCode::ADD:
            case OpCode::SUB:
            case OpCode::MULT:
            case OpCode::DIV:
                return true;
            default:
                return true;
        }
    }
    return false;
}