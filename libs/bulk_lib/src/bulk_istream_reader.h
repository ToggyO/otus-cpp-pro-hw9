#pragma once

#include <istream>

#include "bulk_state.h"
#include "istream_reader.interface.h"

/**
 * @brief Input stream reader.
 */
class BulkIstreamReader : public IIstreamReader
{
    static constexpr std::string_view k_curly_brace_open = "{";
    static constexpr std::string_view k_curly_brace_closed = "}";

public:
    /**
     * @brief Creates instance of @link BulkIstreamReader::BulkIstreamReader @endlink.
     *
     * @param in reference to input stream.
     */
    explicit BulkIstreamReader(std::istream& in)
        : m_in{in},
        m_state{BulkState::Empty},
        m_current_line{},
        m_block_depth{0} {}

    /**
    * @copydoc IIstreamReader::read_next_line()
    *
    * @brief Read next line of input.
    */
    void read_next_line() override
    {
        if (!std::getline(m_in, m_current_line))
        {
            m_state = BulkState::EndOfFile;
            return;
        }

        if (m_current_line == k_curly_brace_open)
        {
            if (m_block_depth++ == 0)
            {
                m_state = BulkState::DynamicBlockStart;
            }
            else
            {
                m_state = BulkState::NestedBlockStart;
            }
            return;
        }

        if (m_current_line == k_curly_brace_closed)
        {
            if (--m_block_depth == 0)
            {
                m_state = BulkState::DynamicBlockEnd;
            }
            else
            {
                m_state = BulkState::NestedBlockEnd;
            }
            return;
        }

        if (m_block_depth > 0)
        {
            m_state = BulkState::DynamicBlockProcessing;
            return;
        }

        m_state = BulkState::StaticBlock;
    }

    /**
    * @copydoc IIstreamReader::get_current_line()
    *
    * @brief Retrieve current read line.
    */
    void get_current_line(std::string &storage) const override { storage = std::string{m_current_line}; }

    /**
    * @copydoc IIstreamReader::get_state()
    *
    * @brief Retrieve current read state.
    */
    [[nodiscard]]
    BulkState get_state() const override { return m_state; }

private:
    BulkState m_state;

    std::istream& m_in;

    std::string m_current_line;

    size_t m_block_depth;
};