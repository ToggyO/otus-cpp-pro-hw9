#pragma once

#include <queue>
#include <sstream>

#include "line_by_line_channel.h"
#include "input_reader.interface.h"

// TODO: copy/move ctors
// TODO: add descr
class BulkQueueReader : public IInputReader
{
    static constexpr std::string_view k_curly_brace_open = "{"; // TODO: duplicate
    static constexpr std::string_view k_curly_brace_closed = "}";

public:
    /**
     * @brief Creates instance of @link BulkQueueReader::BulkQueueReader @endlink.
     */
    explicit BulkQueueReader(const std::shared_ptr<IChannel>& chan)
        : m_chan_ptr{chan},
        m_state{BulkState::Empty},
        m_current_line{},
        m_block_depth{0} {}

    /**
    * @copydoc IIstreamReader::read_next_line()
    *
    * @brief Read next line of input.
    */
    void read_next_line() final
    {
        if (m_chan_ptr->is_closed() && m_chan_ptr->is_empty())
        {
            m_state = BulkState::EndOfFile;
            // На cppreference пишут, что большенство реализаций этого метода работают за константное время
            // https://en.cppreference.com/w/cpp/string/basic_string/clear
            m_current_line.clear();
            return;
        }

        *m_chan_ptr >> m_current_line;
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
    void get_current_line(std::string &storage) const override { storage = std::string{m_current_line}; } // TODO: duplicate

    /**
    * @copydoc IIstreamReader::get_state()
    *
    * @brief Retrieve current read state.
    */
    [[nodiscard]]
    BulkState get_state() const override { return m_state; } // TODO: duplicate

private:
    BulkState m_state;

    std::shared_ptr<IChannel> m_chan_ptr;

    std::string m_current_line;

    size_t m_block_depth;
};