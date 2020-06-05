#ifndef RUSTCOMPILER_BLOCK_H
#define RUSTCOMPILER_BLOCK_H

#include <vector>
#include "Table.h"
#include "TableData.h"

template <typename IDType, typename ArrType>
struct ProgramBlock {
    std::vector<ProgramBlock> internalBlocks;
    ProgramBlock* upperBlock;
    Table<IDType> idTable;
    Table<ArrType> arrayTable;

    ProgramBlock() : upperBlock(nullptr) { }
    explicit ProgramBlock(ProgramBlock* const &previous) : upperBlock(previous) { }

    ProgramBlock& AddBlock() {
        internalBlocks.emplace_back(ProgramBlock(this));
        return internalBlocks.back();
    }

    static void DeleteBlock(ProgramBlock *& programBlock) {
        if (programBlock) {
            while (programBlock->upperBlock)
                programBlock = programBlock->upperBlock;

            delete programBlock;
            programBlock = nullptr;
        }
    }
};


#endif //RUSTCOMP
// ILER_BLOCK_H
