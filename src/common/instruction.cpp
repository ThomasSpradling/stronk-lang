#include "common/instruction.h"

bool operator==(const std::shared_ptr<Instr>& instr1, const std::shared_ptr<Instr>& instr2) {
    if (instr1 && instr2) {
        return (*instr1 == instr2);
    }
    return false; // Handle nullptr case
}
