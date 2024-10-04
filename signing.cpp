#include <iostream>
#include <fstream>
#include <string>
#include <cryptopp/integer.h>
#include <cryptopp/osrng.h>
#define CRYPTOPP_ENABLE_NAMESPACE_WEAK 1
#include <cryptopp/md5.h>

using namespace CryptoPP;
using namespace std;
Integer CustomModularExponentiation(const Integer& base, const Integer& exponent, const Integer& modulus) {
    Integer result = 1;
    Integer base_mod = base % modulus;
    Integer exp = exponent;

    while (exp > 0) {
        if (exp.IsOdd()) {
            result = (result * base_mod) % modulus;
        }
        base_mod = (base_mod * base_mod) % modulus;
        exp >>= 1;
    }

    return result;
}
Integer ReadFromFile(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        throw std::runtime_error("Error!!!!");
    }

    file.seekg(0, std::ios::end);
    size_t size = file.tellg();
    file.seekg(0, std::ios::beg);

    std::vector<CryptoPP::byte> buffer(size);
    file.read((char*)buffer.data(), buffer.size());
    file.close();

    Integer value;
    value.Decode(buffer.data(), buffer.size());
    return value;
}
void WriteToFile(const std::string& filename, const Integer& value) {
    std::ofstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        throw std::runtime_error("Error!!!!");
    }

    size_t size = value.MinEncodedSize();
    std::vector<CryptoPP::byte> buffer(size);
    value.Encode(buffer.data(), size);
    file.write((char*)buffer.data(), buffer.size());
    file.close();
}
Integer HashMessage(const std::string& message) {
    Weak::MD5 hash;  
    CryptoPP::byte digest[Weak::MD5::DIGESTSIZE];  
    
    hash.Update((const CryptoPP::byte*)message.data(), message.size());
    hash.Final(digest);

    return Integer(digest, Weak::MD5::DIGESTSIZE);
}

int main() {
    Integer d = ReadFromFile("privatekey.bin");
    Integer n = ReadFromFile("publickey_n.bin");
    std::string message_file;
    std::cout << "Enter the message file path: ";
    std::cin >> message_file;
    std::ifstream file(message_file);
    if (!file.is_open()) {
        throw std::runtime_error("Error!!!!");
    }

    std::string message((std::istreambuf_iterator<char>(file)),
                        std::istreambuf_iterator<char>());
    file.close();
    Integer H_m = HashMessage(message);
    WriteToFile("msgHash1.bin", H_m);
    Integer S = CustomModularExponentiation(H_m, d, n);
    WriteToFile("signature.bin", S);

    std::cout << "Message signed and signature saved to 'signature.bin'." << std::endl;
    return 0;
}
