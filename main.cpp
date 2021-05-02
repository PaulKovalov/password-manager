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

bool has_access(string username, string password) {
    // get the storage file
    fs::path storage_path = Env::get_home_dir() + "/.pm/" + Hasher::hash_sha256(username + "_storage") + ".pst";
    // read hashed password written to the storage and check if user can access it
    ifstream fin(storage_path);
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
        string username = Env::get_user();
        if (has_access(username, storage_password)) {
            cout << endl << "site: ";
            cin >> site;
            set_std_echo(false);
            cout << "password: ";
            cin >> password;
            cout << endl << "confirm password: ";
            cin >> confirm_password;
            set_std_echo(true);
            if (password == confirm_password) {
                fs::path storage_path = Env::get_home_dir() + "/.pm/" + Hasher::hash_sha256(username + "_storage") + ".pst";
                // open file in append mode
                ofstream of(storage_path, ios_base::app);
                string key = Hasher::hash_sha1(storage_password);
                AES aes;
                of << aes.aes_encode(site, key) << endl << aes.aes_encode(password, key) << endl;
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
    } else if (command == "-i") {
        // storage initialization
        set_std_echo(false);
        string password, username;
        cout << "storage password: ";
        cin >> password;
        set_std_echo(true);
        username = Env::get_user();
        // create a file for passwords
        fs::path root_dir = Env::get_home_dir() + "/.pm";
        if (!fs::exists(root_dir)) {
            fs::create_directory(root_dir);
            fs::path filename =  Hasher::hash_sha256(username + "_storage") + ".pst";
            fs::path full_path = root_dir / filename;
            
            if (!fs::exists(full_path)) {
                ofstream of(full_path);
                of << Hasher::hash_sha256(password) << endl;
                cout << endl << "storage initialized for user " << username << endl;
                return 0;
            } else {
                ofstream of(full_path);
                cout << "storage already exists, overwrite? (y/n): ";
                string overwrite;
                cin >> overwrite;
                if (overwrite == "y") {
                    of << Hasher::hash_sha256(password) << endl;
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
