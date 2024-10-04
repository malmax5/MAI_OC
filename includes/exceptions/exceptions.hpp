#ifndef EXCEPTIONS_HPP
#define EXCEPTIONS_HPP

#include <exception>
#include <string>

class CreateException : public std::exception {
public:
    CreateException(const std::string& message);

    const char* what() const noexcept override;

private:
    std::string message;
};

class WriteReadPipeException : public std::exception {
public:
    WriteReadPipeException(const std::string& message);

    const char* what() const noexcept override;

private:
    std::string message;
};

#endif