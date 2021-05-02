#pragma once
#include <pwd.h>
#include <unistd.h>
#include <string>
#include <iostream>

using namespace std;

class Env {
   public:
    static string get_home_dir();
    static string get_user();
};
