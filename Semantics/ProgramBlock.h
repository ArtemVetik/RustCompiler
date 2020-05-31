#ifndef RUSTCOMPILER_BLOCK_H
#define RUSTCOMPILER_BLOCK_H

#include <vector>
#include "Table.h"
#include "TableData.h"


#include <iostream>
struct ProgramBlock {
    std::vector<ProgramBlock> internalBlocks;
    ProgramBlock* upperBlock;
    Table<ID_Data> idTable;
    Table<Array_Data> arrayTable;

    ProgramBlock() : upperBlock(nullptr) { }
    explicit ProgramBlock(ProgramBlock* const &previous) : upperBlock(previous) { }

    ProgramBlock& AddBlock() {
        internalBlocks.emplace_back(ProgramBlock(this));
        return internalBlocks.back();
    }
};


#endif //RUSTCOMPILER_BLOCK_H
