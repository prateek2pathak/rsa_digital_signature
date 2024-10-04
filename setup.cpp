#include <iostream>
#include <fstream>
#include <cryptopp/integer.h>
#include <cryptopp/osrng.h>
#include <cryptopp/nbtheory.h>  

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

Integer CustomGCD(const Integer& a, const Integer& b) {
    Integer x = a;
    Integer y = b;

    while (y != 0) {
        Integer temp = y;
        y = x % y;
        x = temp;
    }

    return x;
}

bool is_prime(const Integer &n, int iterations = 10) {
    AutoSeededRandomPool rng;
    if (n <= 1) return false;
    if (n <= 3) return true;
    if (n % 2 == 0 || n % 3 == 0) return false;

    Integer d = n - 1;
    int r = 0;
    while (d % 2 == 0) {
        d /= 2;
        r++;
    }

    for (int i = 0; i < iterations; i++) {
        Integer a = 2 + Integer(rng, n.BitCount() - 1) % (n - 3);
        Integer x = CustomModularExponentiation(a, d, n);

        if (x == 1 || x == n - 1) continue;

        bool continue_outer_loop = false;
        for (int j = 0; j < r - 1; j++) {
            x = CustomModularExponentiation(x, 2, n);
            if (x == n - 1) {
                continue_outer_loop = true;
                break;
            }
        }

        if (!continue_outer_loop) return false;
    }

    return true;
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
    AutoSeededRandomPool rng;
    Integer p, q;
    do {
        p.Randomize(rng, 1024);  
    } while (!is_prime(p));     
    do {
        q.Randomize(rng, 1024);
    } while (!is_prime(q) || p == q); 
    Integer n = p * q;
    Integer phi_n = (p - 1) * (q - 1);
    Integer d;
    do {
        d.Randomize(rng, 1024);  
    } while (CustomGCD(d, phi_n) != Integer::One());  
    Integer e = d.InverseMod(phi_n);
    WriteToFile("publickey.bin", e);
    WriteToFile("publickey_n.bin", n);  
    WriteToFile("privatekey.bin", d);
    p = 0;
    q = 0;
    phi_n = 0;

    std::cout << "Setup Completed!!!!" << std::endl;

    return 0;
}
