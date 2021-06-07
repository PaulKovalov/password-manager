#include <iostream>
#include <termios.h>
#include <unistd.h>
#include <pwd.h>
#include <filesystem>
#include <fstream>
#include <cstring>

#include "aes.hpp"
#include "hasher.hpp"
#include "env.hpp"

using namespace std;
namespace fs = std::filesystem;

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

string get_storage_path(string user) {
    return Env::get_home_dir() + "/.pm/" + Hasher::hash_sha256(user + "_storage") + ".pst";
}

bool has_access(string username, string password) {
    // read hashed password written to the storage and check if user can access it
    ifstream fin(get_storage_path(username));
    if (fin.is_open()) {
        string hashed_pwd;
        fin >> hashed_pwd;
        fin.close();
        return hashed_pwd == Hasher::hash_sha256(password);
    } else {
        fin.close();
        cout << "no storage file found, aborting" << endl;
        return false;
    }
}

string ask_password(string prompt) {
    string password;
    set_std_echo(false);
    cout << prompt;
    cin >> password;
    set_std_echo(true);
    return password;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        cout << "Usage: pm <command> <args>" << endl;
        return 0;
    }
    string command = string(argv[1]);
    
    if (command == "-a") {
        // -a stands for "add" a site
        string storage_password = ask_password("storage password: ");
        string username = Env::get_user();
        if (has_access(username, storage_password)) {
            string site;
            cout << endl << "site: ";
            cin >> site;
            string password = ask_password("password: ");
            string confirm_password = ask_password("\nconfirm password: ");
            if (password == confirm_password) {
                // open file in append mode
                ofstream of(get_storage_path(username), ios_base::app);
                AES aes;
                of << aes.aes_encode(site, storage_password) << endl << aes.aes_encode(password, storage_password) << endl;
                of.close();
                cout << endl << "done" << endl;
            } else {
                cout << "passwords don't match" << endl;
                return 0;
            }
        } else {
            cout << "access denied" << endl;
            return 0;
        }
    } else if (command == "-r") {
        // -r stands for "read" a site
        string storage_password = ask_password("storage password: ");
        string username = Env::get_user();
        if (has_access(username, storage_password)) {
            string site;
            // if site was provided as a third argument, use it, otherwise ask
            if (argc == 3) {
                site = string(argv[2]);
            } else {
                cout << endl << "site: ";
                cin >> site;
            }
            // encode site's name in order to find it in the file
            AES aes;
            site = aes.aes_encode(site, storage_password);
            ifstream fin(get_storage_path(username));
            if (fin.is_open()) {
                string line;
                fin >> line; // skip first line with hashed password;
                while (getline(fin, line)) {
                    if (line == site) {
                        // found site, read password
                        getline(fin, line);
                        cout << endl << aes.aes_decode(line, storage_password) << endl;
                        fin.close();
                        return 0;
                    }
                }
                fin.close();
                cout << endl << "couldn't find password" << endl;
            } else {
                cout << endl << "failed to open storage file" << endl;
                fin.close();
                return 0;
            }
        } else {
            cout << "access denied" << endl;
            return 0;
        }
    } else if (command == "-i") {
        // storage initialization
        string storage_password = ask_password("storage password: ");
        string username = Env::get_user();
        // create a file for passwords
        fs::path root_dir = Env::get_home_dir() + "/.pm";
        if (!fs::exists(root_dir)) {
            fs::create_directory(root_dir);
            fs::path filename =  Hasher::hash_sha256(username + "_storage") + ".pst";
            fs::path full_path = root_dir / filename;
            if (!fs::exists(full_path)) {
                ofstream of(full_path);
                of << Hasher::hash_sha256(storage_password) << endl;
                cout << endl << "storage initialized for user " << username << endl;
                return 0;
            } else {
                ofstream of(full_path);
                cout << "storage already exists, overwrite? (y/n): ";
                string overwrite;
                cin >> overwrite;
                if (overwrite == "y") {
                    of << Hasher::hash_sha256(storage_password) << endl;
                } else {
                    cout << "aborting" << endl;
                    return 0;
                }
            }
        } else {
            cout << "couldn't find a free directory to use as a storage, aborting" << endl;
            return 0;
        }
    } else if (command == "-d") {
        // delete word from storage
        string storage_password = ask_password("storage password: ");
        string username = Env::get_user();
    } else if (command == "-h") {
        // display help
    } else if (command == "-rs") {
        // remove storage
    }
    return 0;
}
