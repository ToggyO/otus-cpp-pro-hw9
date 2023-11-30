#pragma once

#include <memory>
#include <filesystem>
#include <fstream>

#include "observable.interface.h"
#include "observer.interface.h"

using namespace std::chrono;

/**
 * @brief Represents file output functionality.
 *
 * Implementation of @link IObserver @endlink.
 * Uses default move/copy ctors/operators.
 */
class FileOutput : public std::enable_shared_from_this<FileOutput>, public IObserver
{
    static constexpr std::string_view m_extension = ".log";

public:
    /**
     * @brief Creates shared smart pointer of @link FileOutput::create @endlink.
     *
     * @param observable constant reference to shared smart pointer of @link IObservable @endlink.
     *
     * @param filename_prefix file name prefix.
     *
     * @return Shared smart pointer of file output handler.
     */
    static std::shared_ptr<FileOutput> create(const std::shared_ptr<IObservable> &observable, const std::string_view filename_prefix)
    {
        auto ptr = std::make_shared<FileOutput>(FileOutput());
        ptr->subscribe_on(observable);
        ptr->m_filename_prefix = std::string{filename_prefix};
        return ptr;
    }

    /**
     * @brief Creates subscription on provided @link IObservable @endlink.
     *
     * @param observable constant reference to shared smart pointer of @link IObservable @endlink.
     */
    void subscribe_on(const std::shared_ptr<IObservable> &observable)
    {
        observable->subscribe(shared_from_this());
    }

    /**
     * @copydoc IObserver::update()
     *
     * Prints message to file.
     */
    void update(const std::string_view message) override
    {
        std::stringstream filename;

        filename << m_filename_prefix;
        filename << time_point_cast<milliseconds>(steady_clock::now()).time_since_epoch().count() << m_extension;

        auto file_path = std::filesystem::current_path() / filename.str();
        std::ofstream file(file_path, std::ofstream::out);

        file << message;
        file.close();
    }

private:
    FileOutput() = default;

    std::string m_filename_prefix;
};