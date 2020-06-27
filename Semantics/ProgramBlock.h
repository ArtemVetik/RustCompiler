#ifndef RUSTCOMPILER_BLOCK_H
#define RUSTCOMPILER_BLOCK_H

#include <vector>
#include "Table.h"
#include "TableData.h"

/// @brief программный блок
/// @details содержит таблицы переменных внутри текущего блока а также список вложенных блоков
template <typename IDType, typename ArrType>
struct ProgramBlock {
    std::vector<ProgramBlock<IDType, ArrType>> internalBlocks; /// @brief список вложенных блоков
    ProgramBlock* upperBlock; /// @brief предыдущий блок
    Table<IDType> idTable;
    Table<ArrType> arrayTable;

    ProgramBlock() : upperBlock(nullptr) { }
    explicit ProgramBlock(ProgramBlock* const &previous) : upperBlock(previous) { }

    /// @brief добавление вложенного блока
    /// @return возвращает ссылку на добавленный блок
    ProgramBlock& AddBlock() {
        internalBlocks.emplace_back(ProgramBlock(this));
        return internalBlocks.back();
    }

    /// @brief удаление блока и всех вложенных блоков
    static void DeleteBlock(ProgramBlock *& programBlock) {
        if (programBlock) {
            while (programBlock->upperBlock)
                programBlock = programBlock->upperBlock;

            delete programBlock;
            programBlock = nullptr;
        }
    }
};

#endif //RUSTCOMP// ILER_BLOCK_H
