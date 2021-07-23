#include <cxxopts.hpp>
#include <base64.h>
#include <fstream>
#include <iostream>
#include <string>
#include <memory>
#include <stdexcept>
#include <cstdint>
class writer {
public:
    virtual ~writer() { }
    virtual void write_line(const std::string& line, const std::string& encoded) = 0;
    virtual void write_error(const std::string& message) = 0;
};
class file_writer : public writer {
public:
    file_writer(const std::string& path, bool print_encoded) {
        this->m_file = std::ofstream(path, std::ios::binary);
        this->m_print_encoded = print_encoded;
    }
    virtual ~file_writer() override {
        this->m_file.close();
    }
    virtual void write_line(const std::string& line, const std::string& encoded) override {
        if (this->m_print_encoded) {
            this->m_file << encoded << ": ";
        }
        this->m_file << line << std::endl;
    }
    virtual void write_error(const std::string& message) override {
        this->m_file << "[error] " << message << std::endl;
    }
private:
    std::ofstream m_file;
    bool m_print_encoded;
};
class stdout_writer : public writer {
public:
    stdout_writer(bool print_encoded) {
        this->m_print_encoded = print_encoded;
    }
    virtual void write_line(const std::string& line, const std::string& encoded) override {
        if (this->m_print_encoded) {
            std::cout << encoded << ": ";
        }
        std::cout << line << std::endl;
    }
    virtual void write_error(const std::string& message) override {
        std::cerr << "[error] " << message << std::endl;
    }
private:
    bool m_print_encoded;
};
int32_t main(int32_t argc, const char** argv) {
    cxxopts::Options options(BINARY_NAME);
    options.allow_unrecognised_options(); // to allow base64 input
    options.add_options()
        ("o,to-file", "Writes outputs to a file.", cxxopts::value<std::string>())
        ("f,from-file", "Reads inputs from a file.", cxxopts::value<std::string>())
        ("s,from-stdin", "Reads from standard input. Overrides -f.", cxxopts::value<bool>()->default_value("false"))
        ("l,remove-linebreaks", "Removes linebreaks from the passed data.", cxxopts::value<bool>()->default_value("false"))
        ("p,print-encoded", "Prints the encoded data before the decoded text.", cxxopts::value<bool>()->default_value("false"))
        ("h,help", "Displays this dialog.", cxxopts::value<bool>()->default_value("false"));
    auto result = options.parse(argc, argv);
    if (result["h"].as<bool>()) {
        std::cout << options.help() << std::endl;
        return EXIT_FAILURE;
    }
    std::vector<std::string> inputs;
    if (result["s"].as<bool>()) {
        std::string line;
        while (std::getline(std::cin, line)) {
            inputs.push_back(line);
        }
    } else if (result["f"].count() > 0) {
        std::ifstream input_file(result["f"].as<std::string>());
        std::string line;
        while (std::getline(input_file, line)) {
            inputs.push_back(line);
        }
        input_file.close();
    } else {
        inputs = result.unmatched();
    }
    if (inputs.size() > 0) {
        std::shared_ptr<writer> writer_instance;
        bool print_encoded = result["p"].as<bool>();
        if (result["o"].count() > 0) {
            writer_instance = std::make_shared<file_writer>(result["o"].as<std::string>(), print_encoded);
        } else {
            writer_instance = std::make_shared<stdout_writer>(print_encoded);
        }
        if (!writer_instance) {
            throw std::runtime_error("writer_instance was null!");
        }
        bool remove_linebreaks = result["l"].as<bool>();
        int32_t return_code = EXIT_SUCCESS;
        for (const auto& input : inputs) {
            try {
                auto decode_result = base64_decode(input, remove_linebreaks);
                writer_instance->write_line(decode_result, input);
            } catch (std::runtime_error exc) {
                writer_instance->write_error(exc.what());
                return_code = EXIT_FAILURE;
            }
        }
        return return_code;
    } else {
        std::cerr << "No data was passed!" << std::endl;
        return EXIT_FAILURE;
    }
}