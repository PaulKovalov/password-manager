#include "aes.hpp"

string AES::uc_to_hex(unsigned char* data, int length) {
    stringstream ss;
    for (int i = 0; i < length; ++i) {
        ss << hex << (int)data[i] << " ";
    }
    return ss.str();
}

pair<unsigned char*, int> AES::hex_to_uc(string data) {
    istringstream hex_chars_stream(data);
    vector<unsigned char> bytes;
    unsigned int c;
    while (hex_chars_stream >> hex >> c) {
        bytes.push_back(c);
    }
    unsigned char* uc = new unsigned char[bytes.size()];
    memcpy(uc, bytes.data(), bytes.size());
    return {uc, bytes.size()};
}

string AES::aes_encode(string message, string key) {
    int input_length = message.size(), key_length = key.length();
    int encoded_msg_length = ((input_length / AES_BLOCK_SIZE) + 1) * AES_BLOCK_SIZE;

    unsigned char aes_input[input_length];
    unsigned char aes_key[AES_KEYLENGTH];
    unsigned char encoded[encoded_msg_length];
    unsigned char iv[AES_BLOCK_SIZE];

    memcpy(aes_input, message.c_str(), input_length);
    memcpy(aes_key, key.c_str(), key_length);
    
    memset(iv, 0, AES_BLOCK_SIZE);
    
    AES_KEY enc_key;
    AES_set_encrypt_key(aes_key, AES_KEYLENGTH, &enc_key);
    AES_cbc_encrypt(aes_input, encoded, input_length, &enc_key, iv, AES_ENCRYPT);

    return uc_to_hex(encoded, encoded_msg_length);
}

string AES::aes_decode(string message, string key) {
    pair<unsigned char*, int> input_message_pair = hex_to_uc(message);
    int input_length = input_message_pair.second;

    unsigned char* aes_input = input_message_pair.first;
    unsigned char aes_key[AES_KEYLENGTH];
    unsigned char decoded[input_length];
    unsigned char iv[AES_BLOCK_SIZE];
    
    memcpy(aes_key, key.c_str(), key.length());
    
    memset(iv, 0, AES_BLOCK_SIZE);
    
    AES_KEY dec_key;
    AES_set_decrypt_key(aes_key, AES_KEYLENGTH, &dec_key);
    AES_cbc_encrypt(aes_input, decoded, input_length, &dec_key, iv, AES_DECRYPT);
    
    stringstream dec_stream;
    for (int i = 0; i < input_length; ++i) {
        dec_stream << decoded[i];
    }
    delete[] aes_input;
    return dec_stream.str();
}
