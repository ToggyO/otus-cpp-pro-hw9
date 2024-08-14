//#include <atomic>
//#include <CLI11.hpp>
//#include <csignal>
//#include <iostream>
//#include <thread>

//#include "bulk.h"
//#include "bulk_istream_reader.h"
//#include "console_output.h"
//#include "file_output.h"
//
//#include "task.h" // TODO: выделить header coro_lib

#include <async.h>

int main(int, char *[]) {
    std::size_t bulk = 5;
    auto h = async::connect(bulk);
    auto h2 = async::connect(bulk);
    async::receive(h, "1", 1);
    async::receive(h2, "1\n", 2);
    async::receive(h, "\n2\n3\n4\n5\n6\n{\na\n", 15);
    async::receive(h, "b\nc\nd\n}\n89\n", 11);
    async::disconnect(h);
    async::disconnect(h2);

    return 0;
}

//bool getline_async(std::istream& is, std::string& str, char delim = '\n') {
//
//    static std::string lineSoFar;
//    char inChar;
//    std::streamsize charsRead = 0;
//    bool lineRead = false;
//    str = "";
//
//    do
//    {
//        charsRead = is.readsome(&inChar, 1);
//        if (charsRead == 1) {
//            // if the delimiter is read then return the string so far
//            if (inChar == delim) {
//                str = lineSoFar;
//                lineSoFar = "";
//                lineRead = true;
//            } else {  // otherwise add it to the string so far
//                lineSoFar.append(1, inChar);
//            }
//        }
//    } while (charsRead != 0 && !lineRead);
//
//    return lineRead;
//}
//
//coro::Task producer(std::queue<std::string>& commands, std::atomic_bool& stop)
//{
////    std::cout << "producer" << std::endl;
//    std::string cmd;
//    while (!stop) // TODO: шареная атомик переменная для двух корутин - стоппер
//    {
//        if (getline_async(std::cin, cmd))
//        {
////            std::cout << cmd << std::endl;
//            commands.push(std::move(cmd));
//            continue;
//        }
//
//        co_await coro::SuspendTask();
//    }
//
//    std::cout << "producer stopped" << std::endl;
//}
//
//coro::Task consumer(std::queue<std::string>& commands, std::atomic_bool& stop)
//{
//
////    std::ofstream output("/home/otogushakov/Projects/plusplus/otus-pro/hw/otus-cpp-pro-hw9/ololo.txt", std::ios::in);
//    std::ofstream output("/home/otogushakov/Projects/plusplus/otus-pro/hw/otus-cpp-pro-hw9/ololo.txt");
//    if (!output.is_open())
//    {
//        throw std::runtime_error("SOOOKA"); // TODO: check
//    }
//
//    while (!stop)
//    {
//        while (!commands.empty())
//        {
//            //        std::cout << "consumer" << std::endl;
////            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
//            auto cmd = commands.front();
//            commands.pop();
//            output << cmd << std::endl;
//            co_await coro::SuspendTask(); // TODO: duplicate
//        }
//        co_await coro::SuspendTask(); // TODO: duplicate
//    }
//
//    output.close();
//    std::cout << "consumer stopped" << std::endl;
//}
//
//std::atomic_bool done = false;
//void sig_handler(int signum) { done = true; }
//
//int main(int argc, char* argv[])
//{
//    signal(SIGINT, sig_handler);
//    signal(SIGTERM, sig_handler);
//
//    std::ios_base::sync_with_stdio(false); // TODO: в корутину
////    auto orchestrator = std::make_shared<coro::Orchestrator>();
//    auto orchestrator = coro::Orchestrator::create_ptr();
//
//    std::queue<std::string> commands;
//
//    orchestrator->enqueue(producer(commands, done));
//    orchestrator->enqueue(consumer(commands, done));
//
//    orchestrator->run();

//    unsigned int block_size = 0;
//    bool console_output = true;
//    bool file_output = true;
//
//    CLI::App app;
//    app.validate_positionals();
//
//    app.add_option("block-size", block_size, "Static command block maximum size")
//        ->required()
//        ->check(CLI::Range((unsigned int)1, std::numeric_limits<unsigned int>::max()));
//    app.add_option("--console-out", console_output, "Enable printing to standard output");
//    app.add_option("--file-out", file_output, "Enable printing to file");
//
//    CLI11_PARSE(app, argc, argv);
//
//    std::shared_ptr<IIstreamReader> reader_ptr = std::make_shared<BulkIstreamReader>(BulkIstreamReader(std::cin));
//    std::shared_ptr<Bulk> bulk_ptr = std::make_shared<Bulk>(Bulk(block_size, reader_ptr));
//
//    std::shared_ptr<IObserver> console;
//    if (console_output)
//    {
//        console = ConsoleOutput::create(bulk_ptr);
//    }
//
//    std::shared_ptr<IObserver> file;
//    if (file_output)
//    {
//        file = FileOutput::create(bulk_ptr, Bulk::output_prefix);
//    }
//
//    bulk_ptr->run();

//    return 0;
//}