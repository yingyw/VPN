#include <iostream>
#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <string>
#include "vpn.h"

using namespace std;

int main(int argc, char** argv)
{
    string servAddr = "173.250.189.190";
    int servPort = 8080;
    int listPort = 1887;

    int argInd = 1;
    bool forcePort = false;
    while (argInd < argc) {
        if (argv[argInd] == string("-h")) {
            cout << "Welcome to vpn. This program will securely route tcp traffic received on the  listening" << endl
                << "port through the vpn server." << endl
                << "Usage: " << endl
                << "    " << argv[0] << " [-h] [-s <server address>] [-p <server port>] [-l <listen port>] [-f]" << endl
                << endl
                << "Options:" << endl
                << "    " << "-h                  : display this help message" << endl
                << "    " << "-s <server address> : the address of the vpn server. Default: " << servAddr << endl
                << "    " << "-p <serevr port>    : the port of the vpn server. Default: " << servPort << endl
                << "    " << "-l <listen port>    : the port to listen for tcp traffic. Default: " << listPort << endl
                << "    " << "-f                  : forces outgoing port to be 80. Default: " << listPort << endl
                << endl;
            return 0;
        }
        if (argv[argInd] == string("-s")) {
            argInd++;
            if (argInd < argc)
                servAddr = argv[argInd];
        }
        if (argv[argInd] == string("-p")) {
            argInd++;
            if (argInd < argc)
                servPort = stoi(argv[argInd]);
        }
        if (argv[argInd] == string("-l")) {
            argInd++;
            if (argInd < argc)
                listPort = stoi(argv[argInd]);
        }
        if (argv[argInd] == string("-f")) {
            forcePort = true;
        }

        argInd++;
    }

    VPN vpnCli(servAddr, servPort, listPort);

    vpnCli.forcePort = forcePort;

    VPNError status;

    status = vpnCli.connect();
    if (status != OK) {
        cout << "Could not connect to server. Check that it exists." << endl;
        return 0;
    }
    status = vpnCli.listen();
    cout << "Done listening." << endl;

    // unsigned char b[10];
    // (*(int*)(b + 1)) = 30;
    // std::cout << (*(int*)b) << std::endl;
    // for (int i = 0; i < 10; i++) {
    //     std::cout << ((int)b[i]) << " ";
    // }
    // VPN vpnCli("69.91.165.41", 8080);

    // const int kBits = RSA_KEY_LENGTH;
    // const int kExp = RSA_E;

    // int keylen;
    // char *pem_key;

    // RSA *rsa = RSA_generate_key(kBits, kExp, 0, 0);

    // /* To get the C-string PEM form: */
    // BIO *bio = BIO_new(BIO_s_mem());
    // PEM_write_bio_RSAPrivateKey(bio, rsa, NULL, NULL, 0, NULL, NULL);

    // keylen = BIO_pending(bio);
    // pem_key = (char*)calloc(keylen+1, 1); /* Null-terminate */
    // BIO_read(bio, pem_key, keylen);

    // string pubKey = pem_key;

    // BIO_free_all(bio);
    // RSA_free(rsa);
    // free(pem_key);

    // cout << pubKey << endl;
    return 0;
}