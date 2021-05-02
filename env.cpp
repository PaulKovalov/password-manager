#include "env.hpp"

string Env::get_home_dir() {
    const char *homedir;
    if ((homedir = getenv("HOME")) == NULL) {
        homedir = getpwuid(getuid())->pw_dir;
    }
    return string(homedir);
}

string Env::get_user() {
    struct passwd *pw = getpwuid(geteuid());
    if (pw) {
        return string(pw->pw_name);
    } else {
        string username;
        cout << endl << "username: ";
        cin >> username;
        return username;
    }
}
