#ifndef ENCRYPTOR_H
#define ENCRYPTOR_H

#include <string>

std::string encrypt(std::string phrase, const std::string &password);
std::string decrypt(std::string phrase, const std::string &password);

#endif
