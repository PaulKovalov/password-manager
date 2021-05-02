#pragma once

#include <openssl/sha.h>

#include <iomanip>
#include <string>

using namespace std;

class Hasher {
   public:
    static string hash_sha1(string str);
    static string hash_sha256(string str);
};
