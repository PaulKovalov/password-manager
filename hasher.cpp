#include "hasher.hpp"

string Hasher::hash_sha1(string str) {
    unsigned char res[20];
    SHA1((unsigned char *)str.c_str(), str.size(), res);
    stringstream ss;
    for (int k = 0; k < 20; k++) {
        ss << hex << setw(2) << setfill('0') << (int)res[k];
    }
    return ss.str();
}

string Hasher::hash_sha256(string str) {
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, str.c_str(), str.size());
    SHA256_Final(hash, &sha256);
    stringstream ss;
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
        ss << hex << setw(2) << setfill('0') << (int)hash[i];
    }
    return ss.str();
}
