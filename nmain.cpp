#include <iostream>
#include <openssl/aes.h>
#include <openssl/sha.h>
#include <sstream>
#include <iomanip>
#include <termios.h>
#include <unistd.h>
#include <pwd.h>
#include <filesystem>
#include <fstream>
#include <cstring>

using namespace std;
namespace fs = std::filesystem;

string hash_sha1(string str)
{
    unsigned char res[20];
    SHA1((unsigned char *)str.c_str(), str.size(), res);
    stringstream ss;
    for (int k = 0; k < 20; k++)
    {
        ss << hex << setw(2) << setfill('0') << (int)res[k];
    }
    return ss.str();
}

string hash_sha256(string str) {
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, str.c_str(), str.size());
    SHA256_Final(hash, &sha256);
    stringstream ss;
    for(int i = 0; i < SHA256_DIGEST_LENGTH; i++)
    {
        ss << hex << setw(2) << setfill('0') << (int)hash[i];
    }
    return ss.str();
}

void set_std_echo(bool enable) {
    // works only with linux
    struct termios tty;
    tcgetattr(STDIN_FILENO, &tty);
    if(!enable) {
        tty.c_lflag &= ~ECHO;
    } else {
        tty.c_lflag |= ECHO;
    }
    tcsetattr(STDIN_FILENO, TCSANOW, &tty);
}

string get_user() {
    // here maybe get this from env variable $USER, who knows
    string username;
    cout << endl << "username: ";
    cin >> username;
    return username;
}

string get_home_dir() {
    const char *homedir;
    if ((homedir = getenv("HOME")) == NULL) {
        homedir = getpwuid(getuid())->pw_dir;
    }
    return string(homedir);
}

bool has_access(string username, string password) {
    // get the storage file
    fs::path storage_path = get_home_dir() + "/.pm/" + hash_sha256(username + "_storage") + ".pst";
    // read hashed password written to the storage and check if user can access it
    ifstream fin(storage_path);
    if (fin.is_open()) {
        string hashed_pwd;
        fin >> hashed_pwd;
        fin.close();
        return hashed_pwd == hash_sha256(password);
    } else {
        fin.close();
        cout << "no storage file found, aborting" << endl;
        return false;
    }
}

string aes_encrypted(string& password, string& data)
{
	// Set iv to character 0 by default here
	unsigned char iv[AES_BLOCK_SIZE] = { '0','0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0' };
	AES_KEY aes_key;
	if (AES_set_encrypt_key((const unsigned char*)password.c_str(), password.length() * 8, &aes_key) < 0) {
		return "";
	}
	string encoded;
	string data_bak = data;
	unsigned int data_length = data_bak.length();

	int padding = 0;
	if (data_bak.length() % (AES_BLOCK_SIZE) > 0) {
		padding = AES_BLOCK_SIZE - data_bak.length() % (AES_BLOCK_SIZE);
	}
	data_length += padding;
	while (padding > 0) {
		data_bak += '\0';
		padding--;
	}

	for (unsigned int i = 0; i < data_length / (AES_BLOCK_SIZE); i++) {
		string str16 = data_bak.substr(i*AES_BLOCK_SIZE, AES_BLOCK_SIZE);
		unsigned char out[AES_BLOCK_SIZE];
		memset(out, 0, AES_BLOCK_SIZE);
		AES_cbc_encrypt((const unsigned char*)str16.c_str(), out, AES_BLOCK_SIZE, &aes_key, iv, AES_ENCRYPT);
		encoded += string((const char*)out, AES_BLOCK_SIZE);
	}
	return encoded;
}


int main(int argc, char* argv[]) {
    if (argc < 2) {
        cout << "Usage: pm <command> <args>" << endl;
        return 0;
    }
    string command = string(argv[1]);
    
    if (command == "-a") {
        // -a stands for "add" a site
        // disable echoing to console
        string site, password, confirm_password, storage_password;
        set_std_echo(false);
        cout << "storage password: ";
        cin >> storage_password;
        set_std_echo(true);
        string username = get_user();
        if (has_access(username, storage_password)) {
            cout << "site: ";
            cin >> site;
            set_std_echo(false);
            cout << "password: ";
            cin >> password;
            cout << endl << "confirm password: ";
            cin >> confirm_password;
            set_std_echo(true);
            if (password == confirm_password) {
                fs::path storage_path = get_home_dir() + "/.pm/" + hash_sha256(username + "_storage") + ".pst";
                // open file in append mode
                ofstream of(storage_path, ios_base::app);
                string key = hash_sha1(storage_password);
                of << aes_encrypted(key, site) << " " << aes_encrypted(key, password) << endl;
                of.close();
                cout << endl << "done" << endl;
            } else {
                cout << "password doesn't match" << endl;
                return 0;
            }
        } else {
            cout << "access denied" << endl;
            return 0;
        }

    } else if (command == "-r") {
        // -r stands for "read" a site
    } else if (command == "-i") {
        // storage initialization
        set_std_echo(false);
        string password, username;
        cout << "storage password: ";
        cin >> password;
        set_std_echo(true);
        username = get_user();
        // create a file for passwords
        fs::path root_dir = get_home_dir() + "/.pm";
        if (!fs::exists(root_dir)) {
            fs::create_directory(root_dir);
            fs::path filename =  hash_sha256(username + "_storage") + ".pst";
            fs::path full_path = root_dir / filename;
            
            if (!fs::exists(full_path)) {
                ofstream of(full_path);
                of << hash_sha256(password) << endl;
                cout << "storage initialized" << endl;
                return 0;
            } else {
                ofstream of(full_path);
                cout << "storage already exists, overwrite? (y/n): ";
                string overwrite;
                cin >> overwrite;
                if (overwrite == "y") {
                    of << hash_sha256(password) << endl;
                } else {
                    cout << "aborting" << endl;
                    return 0;
                }
            }
        } else {
            cout << "couldn't find a free directory to use as a storage, aborting" << endl;
            return 0;
        }
        
    } else if (command == "-h") {
        // display help
    }
    return 0;
}
