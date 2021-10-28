import sys
import getpass
import os.path
import pyperclip

from pathlib import Path
from hasher import Hasher
from aes import AESCipher


def get_storage_path(username: str) -> str:
    return os.path.join(os.environ['HOME'],
                        '.pmp',
                        Hasher.sha256(username + '_storage') + '.pst')


def has_access(username: str, password: str) -> bool:
    storage_path = get_storage_path(username)
    if Path(storage_path).exists() and Path(storage_path).is_file():
        with open(storage_path, 'r') as storage:
            storage_password = storage.readline().rstrip()
            return storage_password == Hasher.sha256(password)
    else:
        print('no storage file found, aborting')
        return False


def append_newline(s: str) -> str:
    return s + '\n'


def create_storage_file(absolute_storage_path: str, storage_password: str):
    Path(absolute_storage_path).touch()
    with open(absolute_storage_path, 'a') as storage_file:
        storage_file.write(append_newline(Hasher.sha256(storage_password)))
        print('done')


def get_ctx() -> (str, str, str):
    storage_password = getpass.getpass('storage password: ')
    username = getpass.getuser()
    if has_access(username, storage_password):
        if len(sys.argv) >= 3 and sys.argv[2][0] != '-':
            site = sys.argv[2]
        else:
            site = input('site: ')
        return username, storage_password, site
    else:
        return None, None, None


def main():
    num_of_args = len(sys.argv)
    if num_of_args < 2:
        print('Usage: main.py <command> <args>')
        return
    command = sys.argv[1]
    if command == '-a':
        username, storage_password, site = get_ctx()
        if storage_password is not None:
            site_password = input('site password: ')
            confirm_site_password = input('confirm site password: ')
            if site_password == confirm_site_password:
                storage_path = get_storage_path(username)
                with open(storage_path, 'a+') as storage_file:
                    cipher = AESCipher(storage_password)
                    storage_file.write(append_newline(cipher.encrypt(site)))
                    storage_file.write(append_newline(cipher.encrypt(site_password)))
                    print('done')
            else:
                print('passwords don\'t match')
        else:
            print('access denied')
    elif command == '-i':
        # storage initialization
        storage_password = getpass.getpass('storage password: ')
        username = getpass.getuser()
        root_dir = os.path.join(os.environ['HOME'], '.pmp')
        if not Path(root_dir).exists():
            Path(root_dir).mkdir()
        storage_filename = Hasher.sha256(username + '_storage') + '.pst'
        absolute_storage_path = os.path.join(root_dir, storage_filename)
        if Path(absolute_storage_path).exists():
            print('storage already exists, are you sure you want to overwrite it? (y/N)', end=' ')
            overwrite = input()
            if overwrite == 'y':
                create_storage_file(absolute_storage_path, storage_password)
            else:
                print('aborting')
        else:
            create_storage_file(absolute_storage_path, storage_password)
            print('storage initialized for user ' + username)
    elif command == '-r':
        username, storage_password, site = get_ctx()
        if storage_password is not None:
            # encode site's name to find the corresponding password in the storage file
            cipher = AESCipher(storage_password)
            absolute_storage_path = get_storage_path(username)
            with open(absolute_storage_path, 'r') as storage_file:
                # skip the first line with storage password
                storage_file.readline()
                while True:
                    encrypted_site = storage_file.readline().rstrip()
                    encrypted_password = storage_file.readline().rstrip()
                    if not encrypted_site or not encrypted_password:
                        print('couldn\'t find password')
                        break
                    if site == cipher.decrypt(encrypted_site):
                        decrypted_password = cipher.decrypt(encrypted_password)
                        if len(sys.argv) == 4 and sys.argv[3] == '-ptc':
                            print(decrypted_password)
                        else:
                            pyperclip.copy(decrypted_password)
                            print('copied to clipboard')
                        break
        else:
            print('access denied')
    elif command == '-d':
        username, storage_password, site = get_ctx()
        if storage_password is not None:
            cipher = AESCipher(storage_password)
            absolute_storage_path = get_storage_path(username)
            storage_file = open(absolute_storage_path, 'r+')
            storage_data = storage_file.readlines()
            storage_file.close()
            passwords_data = storage_data[1:]
            passwords_tuples = [passwords_data[i:i + 2] for i in range(0, len(passwords_data), 2)]
            new_passwords = [t for t in passwords_tuples if cipher.decrypt(t[0].rstrip()) != site]
            if len(new_passwords) != len(passwords_tuples):
                # write updated passwords to the file
                create_storage_file(absolute_storage_path, storage_password)
                with open(absolute_storage_path, 'a') as storage_file:
                    for password_tuple in new_passwords:
                        storage_file.write(password_tuple[0])
                        storage_file.write(password_tuple[1])
                print('password deleted')
            else:
                print('couldn\'t find password for the specified site')
        else:
            print('access denied')
    elif command == '-h':
        print("""+++++++++++++++++++++++++++++++++++++++++++++++
+ |¯¯| |\/|   /\   |\  |   /\   |¯¯| |¯¯ |¯\  +
+ |__| |  |  /__\  | \ |  /__\  | _  |-  |_/  +
+ |    |  | /    \ |  \| /    \ |__| |__ | \  +
+++++++++++++++++++++++++++++++++++++++++++++++
pmanager is a command line tool for securely storing and accessing passwords. 
pmanager keeps all passwords and their context encrypted using symmetrical encryption with master password
serving as the encryption key.
        
"python3 pm.py -i"                       - initialize password storage for the user defined in env variable $USER
"python3 pm.py -a <optional: site name>" - append password for the site name to the existing
                                           storage of the current user. If site name is not
                                           passed as a command line argument, you will be
                                           prompted to enter it
"python3 pm.py -r <optional: site name>" - read password for the site, and write it to the clipboard. 
                  <optional: -ptc>         If flag "ptc" is set, prints password to the stdout! The order                                        
                                           of the arguments is important
"python3 pm.py -d <optional: site name>  - delete password for the site from storage. 
                                           This is irreversible operation
"python3 pm.py -h"                       - print this help""")


if __name__ == '__main__':
    main()
