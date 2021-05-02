#pragma once

#include <string>
#include <cstdio>
#include <cstring>
#include <openssl/aes.h>
#include <sstream>
#include <iostream>
#include <vector>

#define AES_KEYLENGTH 256

using namespace std;

class AES {
    string uc_to_hex(unsigned char* data, int length);
    pair<unsigned char*, int> hex_to_uc(string data);

   public:
    string aes_encode(string message, string key);
    string aes_decode(string message, string key);
};
