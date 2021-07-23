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
    virtual void write_line(const std::string& line, const std::string& original) = 0;
};
class file_writer : public writer {
public:
    file_writer(const std::string& path, bool print_original) {
        this->m_file = std::ofstream(path);
        this->m_print_original = print_original;
    }
    virtual void write_line(const std::string& line, const std::string& original) override {
        if (this->m_print_original) {
            this->m_file << original << ": ";
        }
        this->m_file << line << std::endl;
    }
private:
    std::ofstream m_file;
    bool m_print_original;
};
class stdout_writer : public writer {
public:
    stdout_writer(bool print_original) {
        this->m_print_original = print_original;
    }
    virtual void write_line(const std::string& line, const std::string& original) override {
        if (this->m_print_original) {
            std::cout << original << ": ";
        }
        std::cout << line << std::endl;
    }
private:
    bool m_print_original;
};
int32_t main(int32_t argc, const char** argv) {
    cxxopts::Options options(BINARY_NAME);
    options.allow_unrecognised_options();
    options.add_options()
        ("o,to-file", "Writes outputs to a file.", cxxopts::value<std::string>())
        ("f,from-file", "Reads inputs from a file.", cxxopts::value<std::string>())
        ("u,url", "Specifies that the passed data is a URL.", cxxopts::value<bool>()->default_value("false"))
        ("p,print-original", "Prints the input as well as the output for each entry.", cxxopts::value<bool>()->default_value("false"))
        ("h,help", "Displays this dialog.", cxxopts::value<bool>()->default_value("false"));
    auto result = options.parse(argc, argv);
    if (result["h"].as<bool>()) {
        std::cout << options.help() << std::endl;
        return EXIT_FAILURE;
    }
    std::vector<std::string> inputs;
    if (result["f"].count() > 0) {
        std::ifstream input_file(result["f"].as<std::string>(), std::ios::binary);
        std::string line;
        while (std::getline(input_file, line)) {
            inputs.push_back(line);
        }
        input_file.close();
    } else {
        inputs = result.unmatched();
    }
    if (inputs.size() >  0) {
        std::shared_ptr<writer> writer_instance;
        bool print_original = result["p"].as<bool>();
        if (result["o"].count() > 0) {
            writer_instance = std::make_shared<file_writer>(result["o"].as<std::string>(), print_original);
        } else {
            writer_instance = std::make_shared<stdout_writer>(print_original);
        }
        if (!writer_instance) {
            throw std::runtime_error("writer_instance was null!");
        }
        bool url = result["u"].as<bool>();
        for (const auto& input : inputs) {
            auto encode_result = base64_encode(input, url);
            writer_instance->write_line(encode_result, input);
        }
        return EXIT_SUCCESS;
    } else {
        std::cerr << "No data was passed!" << std::endl;
        return EXIT_FAILURE;
    }
}