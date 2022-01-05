#include "utils.h"
#include <cstdio>
#include <cstring>
#include <string>

using std::string;
//ref: https://blog.csdn.net/qq_42650346/article/details/118445356
//unsigned char *SHA256(const unsigned char  *d, size_t n, unsigned char *hash);
int sha256(const char *readstr, unsigned char *hash)
{
	if (SHA256((unsigned char*)readstr, strlen(readstr), hash) == NULL) {
		printf("sha256 erro\n");
		return -1;
	}	
	return 0;
}

void print_sha256(const std::string &hash)
{
    print_sha256((unsigned char *)hash.c_str());
}

void print_sha256(unsigned char *hash)
{
	for (int i = 0; i < 32; i++) {
		printf("%02x", hash[i]);
	}
	printf("\n");
}

std::string random_string(int n) {
    string s(n, ' ');
    for(int i = 0; i < n; i++) {
        s[i] = 'a' + rand() % 26;
    }
    return s;
}

std::string crypto_hash(const std::string &s) {
    unsigned char hash[33];
    memset(hash, 0, 33);
    if(sha256(s.c_str(), hash) == -1) {
        return "";
    }
    hash[32] = '\0';
    return string((char *)hash);
}