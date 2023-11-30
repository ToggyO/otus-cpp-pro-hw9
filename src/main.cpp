#include <CLI11.hpp>
#include <iostream>

#include "bulk.h"
#include "bulk_istream_reader.h"
#include "console_output.h"
#include "file_output.h"

int main(int argc, char* argv[])
{
    unsigned int block_size = 0;
    bool console_output = true;
    bool file_output = true;

    CLI::App app;
    app.validate_positionals();

    app.add_option("block-size", block_size, "Static command block maximum size")
        ->required()
        ->check(CLI::Range((unsigned int)1, std::numeric_limits<unsigned int>::max()));
    app.add_option("--console-out", console_output, "Enable printing to standard output");
    app.add_option("--file-out", file_output, "Enable printing to file");

    CLI11_PARSE(app, argc, argv);

    std::shared_ptr<IIstreamReader> reader_ptr = std::make_shared<BulkIstreamReader>(BulkIstreamReader(std::cin));
    std::shared_ptr<Bulk> bulk_ptr = std::make_shared<Bulk>(Bulk(block_size, reader_ptr));

    std::shared_ptr<IObserver> console;
    if (console_output)
    {
        console = ConsoleOutput::create(bulk_ptr);
    }

    std::shared_ptr<IObserver> file;
    if (file_output)
    {
        file = FileOutput::create(bulk_ptr, Bulk::output_prefix);
    }

    bulk_ptr->run();

    return 0;
}