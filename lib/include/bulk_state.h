#pragma once

#include <cstdint>

/**
 * @brief Represents bulk command processing state.
 */
enum class BulkState : uint8_t
{
    /** @brief Initial state. */
    Empty,

    /** @brief Static command block, restricted by maximum block size. */
    StaticBlock,

    /** @brief Dynamic command block starts. */
    DynamicBlockStart,

    /** @brief Dynamic command in progress. */
    DynamicBlockProcessing,

    /** @brief Nested dynamic command block starts. */
    NestedBlockStart,

    /** @brief Nested dynamic command block ends. */
    NestedBlockEnd,

    /** @brief Dynamic command block ends. */
    DynamicBlockEnd,

    /** @brief Input ends. */
    EndOfFile
};