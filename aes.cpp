#include <string>
#include <cstdio>
#include <cstring>
#include <openssl/aes.h>
#include <sstream>
#include <iostream>
#include <vector>

using namespace std;

#define AES_KEYLENGTH 256

string cipher_AES(string key, string message);
string aes_encode(string message, string key);
string aes_decode(string message, string key);

int main()
{
    //  cipher_AES("29857efkjgahklfgq938dfghalkjhqweriuqwpoeriuqwpoeriuqwopeiruoq529348trty92", "Hello, how are you, you mad?");
    string message = "Hello, how are you, you mad?";
    string key = "29857efkjgahklfgq938dfghalkjhqweriuqwpoeriuqwpoeriuqwopeiruoq529348trty92";
    
    string encoded = aes_encode(message, key);
    cout << encoded << endl;
    string decoded = aes_decode(encoded, key);
    cout << decoded << endl;
    return 0;
}


string uc_to_hex(unsigned char* data, int length) {
    cout << "UC to hex input length: " << length << endl;
    stringstream ss;
    for (int i = 0; i < length; ++i) {
        ss << hex << (int)data[i];
    }
    return ss.str();
}

pair<unsigned char*, int> hex_to_uc(string data) {
    istringstream hex_chars_stream(data);
    vector<unsigned char> bytes;
    unsigned char c;
    while (hex_chars_stream >> hex >> c) {
        bytes.push_back(c);
    }
    unsigned char* uc = new unsigned char[bytes.size()];
    memcpy(uc, bytes.data(), bytes.size());
    return {uc, bytes.size()};
}

string aes_encode(string message, string key) {
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

string aes_decode(string message, string key) {
    pair<unsigned char*, int> input_message_pair = hex_to_uc(message);
    int input_length = input_message_pair.second;
    cout << "input length: " << input_length << endl;
    unsigned char* aes_input = input_message_pair.first;
    unsigned char aes_key[AES_KEYLENGTH];
    unsigned char decoded[input_length / AES_BLOCK_SIZE];
    unsigned char iv[AES_BLOCK_SIZE];
    
    memcpy(aes_key, key.c_str(), key.length());
    
    memset(iv, 0, AES_BLOCK_SIZE);
    
    AES_KEY dec_key;
    AES_set_decrypt_key(aes_key, AES_KEYLENGTH, &dec_key);
    AES_cbc_encrypt(aes_input, decoded, input_length, &dec_key, iv, AES_DECRYPT);
    for (int i = 0; i < sizeof(decoded); ++i) {
        cout << (int)decoded[i] << " ";
    }
    cout << endl;
    return "";
}

/* computes the ciphertext from plaintext and key using AES256-CBC algorithm */
string cipher_AES(string key, string message)
{
    size_t inputslength = message.length();
    unsigned char aes_input[inputslength];
    unsigned char aes_key[AES_KEYLENGTH];
    memset(aes_input, 0, inputslength/8);
    memset(aes_key, 0, AES_KEYLENGTH/8);
    strcpy((char*) aes_input, message.c_str());
    strcpy((char*) aes_key, key.c_str());

    /* init vector */
    unsigned char iv[AES_BLOCK_SIZE];
    memset(iv, 0x00, AES_BLOCK_SIZE);

    // buffers for encryption and decryption
    const size_t encslength = ((inputslength + AES_BLOCK_SIZE) / AES_BLOCK_SIZE) * AES_BLOCK_SIZE;
    unsigned char enc_out[encslength];
    unsigned char dec_out[inputslength];
    memset(enc_out, 0, sizeof(enc_out));
    memset(dec_out, 0, sizeof(dec_out));

    AES_KEY enc_key, dec_key;
    AES_set_encrypt_key(aes_key, AES_KEYLENGTH, &enc_key);
    AES_cbc_encrypt(aes_input, enc_out, inputslength, &enc_key, iv, AES_ENCRYPT);

    AES_set_decrypt_key(aes_key, AES_KEYLENGTH, &dec_key);
    memset(iv, 0x00, AES_BLOCK_SIZE);
    AES_cbc_encrypt(enc_out, dec_out, encslength, &dec_key, iv, AES_DECRYPT);

    
    // cout << "original:\t" << to_hex(aes_input, sizeof(aes_input)) << endl;
    // cout << "encrypt:\t" << to_hex(enc_out, sizeof(enc_out)) << endl;
    // cout << "decrypt:\t" << to_hex(dec_out, sizeof(dec_out)) << endl;

    stringstream ss;
    for(int i = 0; i < encslength; i++)
    {
        ss << enc_out[i];
    }
    return ss.str();
}
