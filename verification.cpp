#include <iostream>
#include <fstream>
#include <string>
#include <cryptopp/integer.h>
#include <cryptopp/osrng.h>
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

int main() {
    Integer e = ReadFromFile("publickey.bin");
    Integer n = ReadFromFile("publickey_n.bin");
    Integer S = ReadFromFile("signature.bin");
    Integer D = ReadFromFile("msgHash1.bin");
    Integer D_prime = CustomModularExponentiation(S, e, n);
    WriteToFile("msgHash2.bin", D_prime);
    if (D == D_prime) {
        std::cout << "Signature is valid." << std::endl;
    } else {
        std::cout << "Signature is invalid." << std::endl;
    }
    return 0;
}