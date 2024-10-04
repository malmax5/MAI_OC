#include "../../includes/exceptions/exceptions.hpp"

CreateException::CreateException(const std::string& message) : message(message) {}
const char* CreateException::what() const noexcept {
    return message.c_str();
}

WriteReadPipeException::WriteReadPipeException(const std::string& message) : message(message) {}
const char* WriteReadPipeException::what() const noexcept {
    return message.c_str();
}