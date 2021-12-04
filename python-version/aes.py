import base64
from Crypto import Random
from Crypto.Cipher import AES
from hasher import Hasher


# provides two methods for encrypting and decrypting string
class AESCipher(object):

    def __init__(self, key: str):
        # take SHA-256 hash of key, so that key is 32 bits long, which is perfect for AES
        self.key = Hasher.sha256_bytes(key)

    def encrypt(self, raw):
        raw = self._pad(raw)
        iv = Random.new().read(AES.block_size)
        cipher = AES.new(self.key, AES.MODE_CBC, iv)
        return base64.b64encode(iv + cipher.encrypt(raw.encode())).decode('utf-8')

    def decrypt(self, enc):
        # encoded string is base64 encoded, converted to human readable format
        enc = base64.b64decode(enc.encode('utf-8'))
        iv = enc[:AES.block_size]
        cipher = AES.new(self.key, AES.MODE_CBC, iv)
        return self._unpad(cipher.decrypt(enc[AES.block_size:])).decode('utf-8')

    # aligns data
    def _pad(self, s):
        return s + (AES.block_size - len(s) % AES.block_size) * chr(AES.block_size - len(s) % AES.block_size)

    def _unpad(self, s):
        return s[:-ord(s[len(s) - 1:])]
