#ifndef WZQ_UTILS_H
#define WZQ_UTILS_H

#include <openssl/sha.h>
#include <string>

std::string crypto_hash(const std::string &s);
std::string random_string(int n);
int sha256(const char *readstr, unsigned char *hash);
void print_sha256(const std::string &hash);
void print_sha256(unsigned char *hash);

#endif