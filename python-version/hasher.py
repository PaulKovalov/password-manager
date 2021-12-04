from hashlib import sha256, sha1


# provides static methods for generating cryptographic hashes of a string
class Hasher:
    __default_encoding = 'utf-8'

    # returns SHA-256 hash in a HEX format
    @staticmethod
    def sha256(_str) -> str:
        return sha256(_str.encode(Hasher.__default_encoding)).hexdigest()

    # returns SHA-256 hash in a format of bytes
    @staticmethod
    def sha256_bytes(_str):
        return sha256(_str.encode(Hasher.__default_encoding)).digest()