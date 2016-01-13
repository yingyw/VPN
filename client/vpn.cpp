#include <string>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#include <iostream>
#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <openssl/err.h>
#include <openssl/bn.h>
#include <openssl/conf.h>
#include <string.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <linux/netfilter_ipv4.h>
#include <fcntl.h>

#include <ctime>
#include <chrono>

#include <vector>
#include <map>

#include <errno.h>

#include "vpn.h"

#define BUF_LEN 16384

// #define NO_ECRYPT

#define BENCHMARK

static void handleErrors() {ERR_print_errors_fp (stderr); std::cout << "error" << std::endl; }

static int encryptms(unsigned char *plaintext, int plaintext_len, unsigned char *key,
    const unsigned char *iv, unsigned char *ciphertext)
{
#ifdef NO_ECRYPT
    memcpy(ciphertext, plaintext, plaintext_len);
    return plaintext_len;
#else
    EVP_CIPHER_CTX *ctx;

    int len;

    int ciphertext_len;

    /* Create and initialise the context */
    if(!(ctx = EVP_CIPHER_CTX_new())) handleErrors();

    /* Initialise the encryption operation. IMPORTANT - ensure you use a key
    * and IV size appropriate for your cipher
    * In this example we are using 256 bit AES (i.e. a 256 bit key). The
    * IV size for *most* modes is the same as the block size. For AES this
    * is 128 bits */
    if(1 != EVP_EncryptInit_ex(ctx, EVP_aes_128_ecb(), NULL, key, iv))
    handleErrors();

    /* Provide the message to be encrypted, and obtain the encrypted output.
    * EVP_EncryptUpdate can be called multiple times if necessary
    */
    if(1 != EVP_EncryptUpdate(ctx, ciphertext, &len, plaintext, plaintext_len))
    handleErrors();
    ciphertext_len = len;

    /* Finalise the encryption. Further ciphertext bytes may be written at
    * this stage.
    */
    if(1 != EVP_EncryptFinal_ex(ctx, ciphertext + len, &len)) handleErrors();
    ciphertext_len += len;

    /* Clean up */
    EVP_CIPHER_CTX_free(ctx);

    return ciphertext_len;
#endif
}

static int decryptms(unsigned char *ciphertext, int ciphertext_len, unsigned char *key,
    const unsigned char *iv, unsigned char *plaintext)
{
#ifdef NO_ECRYPT
    memcpy(plaintext, ciphertext, ciphertext_len);
    return ciphertext_len;
#else
    EVP_CIPHER_CTX *ctx;

    int len;

    int plaintext_len;

    /* Create and initialise the context */
    if(!(ctx = EVP_CIPHER_CTX_new())) handleErrors();

    /* Initialise the decryption operation. IMPORTANT - ensure you use a key
    * and IV size appropriate for your cipher
    * In this example we are using 256 bit AES (i.e. a 256 bit key). The
    * IV size for *most* modes is the same as the block size. For AES this
    * is 128 bits */
    if(1 != EVP_DecryptInit_ex(ctx, EVP_aes_128_ecb(), NULL, key, iv))
    handleErrors();

    /* Provide the message to be decrypted, and obtain the plaintext output.
    * EVP_DecryptUpdate can be called multiple times if necessary
    */
    if(1 != EVP_DecryptUpdate(ctx, plaintext, &len, ciphertext, ciphertext_len))
    handleErrors();
    plaintext_len = len;

    /* Finalise the decryption. Further plaintext bytes may be written at
    * this stage.
    */
    if(1 != EVP_DecryptFinal_ex(ctx, plaintext + len, &len)) handleErrors();
    plaintext_len += len;

    /* Clean up */
    EVP_CIPHER_CTX_free(ctx);

    return plaintext_len;
#endif
}



VPN::VPN(std::string vpnServAddr, int vpnServPort, int listPort) {
    this->vpnServAddr = vpnServAddr;
    this->vpnServPort = vpnServPort;
    this->listPort = listPort;

    ERR_load_crypto_strings();
    OpenSSL_add_all_algorithms();
    OPENSSL_config(NULL);

    rsa = RSA_generate_key(RSA_KEY_LENGTH, RSA_E, 0, 0);

    // aesKeyLength = 16;
    // aesKey[0] = 0x01;
    // aesKey[1] = 0xdd;
    // aesKey[2] = 0x32;
    // aesKey[3] = 0x95;
    // aesKey[4] = 0x25;
    // aesKey[5] = 0x74;
    // aesKey[6] = 0x3b;
    // aesKey[7] = 0x4a;
    // aesKey[8] = 0xf2;
    // aesKey[9] = 0xae;
    // aesKey[10] = 0xac;
    // aesKey[11] = 0xaa;
    // aesKey[12] = 0x1c;
    // aesKey[13] = 0xa9;
    // aesKey[14] = 0x0c;
    // aesKey[15] = 0x39;
    // aesKey[16] = 0;

    // char* msg = "\0x12\12";
}

VPNError VPN::connect() {
    struct sockaddr_in serv_addr;
    struct hostent *server;

    vpnServSockfd = socket(AF_INET, SOCK_STREAM, 0);
    server = gethostbyname(vpnServAddr.c_str());
    memset((char *) &serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
    serv_addr.sin_port = htons(vpnServPort);

    if (::connect(vpnServSockfd, (struct sockaddr *)&serv_addr,sizeof(serv_addr)) < 0) {
        std::cout << "ERROR connecting to vpn server" << std::endl;
        return NO_SERVER;
    }

    unsigned char buffer[1360];
    int nLen = BN_num_bytes(rsa->n);
    int eLen = BN_num_bytes(rsa->e);
    int* bufPtr = (int*)buffer;
    *(bufPtr++) = nLen + eLen + 4 * 4;
    *(bufPtr++) = 0xBEBEBEBE;

    *(bufPtr++) = nLen; // modulus length
    BN_bn2bin(rsa->n, (unsigned char*)bufPtr);
    bufPtr = (int*)((char*)bufPtr + nLen);

    *(bufPtr++) = eLen; // exponent length
    BN_bn2bin(rsa->e, (unsigned char*)bufPtr);

    write(vpnServSockfd,buffer,BN_num_bytes(rsa->n) + BN_num_bytes(rsa->e) + 4 * 4);

    read(vpnServSockfd, buffer, 300);
    int shake = *(((int*)buffer) + 1);

    unsigned char * rsaIn = (unsigned char *)buffer + 8;
    int inpLen = *(int*)buffer;

    aesLength = RSA_private_decrypt((inpLen - 8), rsaIn, aesKey, rsa, RSA_PKCS1_PADDING);
    aesKey[aesLength] = 0;

    // std::cout << "Length: " << aesLength << " AES key: ";
    // for (int i = 0; i < aesLength; i++) {
    //     std::cout << std::hex << (int)aesKey[i] << " ";
    // }
    return OK;
}


VPNError VPN::listen() {
    unsigned char buf[BUF_LEN];
    unsigned char buf2[BUF_LEN];

    // Make server socket non-blocking
    fcntl(vpnServSockfd, F_SETFL, fcntl(vpnServSockfd, F_GETFL, 0) | O_NONBLOCK);

    auto t_start = std::chrono::high_resolution_clock::now();

    // Listen to port 1887
    int sockfd;
    int sock_family_;
    struct sockaddr_in serv_addr;
    sockfd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
    int optval = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof optval);
    if (sockfd < 0)
      printf("ERROR opening socket");
    memset((char *) &serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(listPort);
    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
      printf("ERROR on binding: %s", strerror(errno));
    ::listen(sockfd, 100/*HTTP_MAX_CONNECTIONS*/);


    std::cout << "listening..." << std::endl;

    std::map<int, int> clients;

    int waitTime = 1;

    while (true) {
        
        t_start = std::chrono::high_resolution_clock::now();
        // Sleep for a little
        usleep(waitTime);
        waitTime = waitTime < 16 ? waitTime * 2 : waitTime;


        // Add new connection if such exists
        struct sockaddr_in cli_addr;
        socklen_t clilen = sizeof(cli_addr);
        int newsockfd = accept4(sockfd, (struct sockaddr *) &cli_addr, &clilen, SOCK_NONBLOCK);
        //if (std::chrono::duration<double, std::milli>(std::chrono::high_resolution_clock::now()-t_start).count() > 10) printf("Accepting: %.f\n", std::chrono::duration<double, std::milli>(std::chrono::high_resolution_clock::now()-t_start).count());//--
        if (newsockfd >= 0) {
            // New socket received.
            clients[newsockfd] = newsockfd;
            // Tell server about it
            struct sockaddr_in destAddress;
            memset(&destAddress, 0, sizeof(destAddress));
            socklen_t destAddrLen = sizeof(destAddress);
            getsockopt(newsockfd, IPPROTO_IP, SO_ORIGINAL_DST, &destAddress, &destAddrLen);
            
            if (forcePort) {
                destAddress.sin_port = 0x5000;
                // destAddress.sin_addr.s_addr = 216 + (58 << 8) + (216 << 16) + (142 << 24);
            }


            *(unsigned int*)(buf + 4) = SERVER_PROXY_WORD;
            buf2[0] = OPEN_TYPE;
            *(int*)(buf2 + 1) = newsockfd;
            buf2[5] = (destAddress.sin_addr.s_addr >> 0);
            buf2[6] = (destAddress.sin_addr.s_addr >> 8);
            buf2[7] = (destAddress.sin_addr.s_addr >> 16);
            buf2[8] = (destAddress.sin_addr.s_addr >> 24);
            buf2[9] = destAddress.sin_port;
            buf2[10] = destAddress.sin_port >> 8;

            std::cout << "New connection. Destination: " << (int)buf2[5] << "." << (int)buf2[6] << "." << 
                (int)buf2[7] << "." <<  (int)buf2[8] << ":" <<  ((((int)buf2[9]) << 8) + buf2[10]) << std::endl;

            int ciphertext_len = encryptms(buf2, 11, aesKey, (const unsigned char*)"randomivstuffher", buf + 8);
            *(unsigned int*)buf = 8 + ciphertext_len;
            int wr = write(vpnServSockfd, buf, 8 + ciphertext_len);


            //if (std::chrono::duration<double, std::milli>(std::chrono::high_resolution_clock::now()-t_start).count() > 10) printf("Shaking: %.f\n", std::chrono::duration<double, std::milli>(std::chrono::high_resolution_clock::now()-t_start).count());//--
            // Reset waitTime
            waitTime = 1;
        } else if (errno !=  EAGAIN && errno != EWOULDBLOCK) {
            // Error getting new socket
            printf("Error accepting client.\n    %s\n", strerror(errno));
        }


        // Go through current sockets and handle them
        for (std::map<int, int>::iterator clPtr = clients.begin(); clPtr != clients.end(); clPtr++) {
            int cliId = clPtr->first;
            int cliSock = clPtr->second;
            // Check for new data from client
            int rd = read(cliSock, buf2 + 5, BUF_LEN - 5 - 32);
            //if (std::chrono::duration<double, std::milli>(std::chrono::high_resolution_clock::now()-t_start).count() > 10) printf("reading: %.f\n", std::chrono::duration<double, std::milli>(std::chrono::high_resolution_clock::now()-t_start).count());//--
            if (rd > 0) {
                // Data read from client. Send to server
                buf2[0] = SEND_TYPE;
                *(int*)(buf2 + 1) = cliId;
                int ciphertext_len = encryptms(buf2, rd + 5, aesKey, (const unsigned char*)"randomivstuffher", buf + 8);
                *(unsigned int*)buf = 8 + ciphertext_len;
                *(unsigned int*)(buf + 4) = SERVER_PROXY_WORD;
                write(vpnServSockfd, buf, ciphertext_len + 8);
                // Reset waitTime
                waitTime = 1;
            } else if ((rd < 0) && (errno !=  EAGAIN && errno != EWOULDBLOCK)) {
                // Drop connection
                printf("Error reading from client.\n    %s\n Dropping...\n", strerror(errno));
                buf2[0] = CLOSE_TYPE;
                *(int*)(buf2 + 1) = cliId;
                int ciphertext_len = encryptms(buf2, 5, aesKey, (const unsigned char*)"randomivstuffher", buf + 8);
                *(unsigned int*)buf = 8 + ciphertext_len;
                *(unsigned int*)(buf + 4) = SERVER_PROXY_WORD;
                write(vpnServSockfd, buf, ciphertext_len + 8);
                clients.erase(clPtr);
                // Reset waitTime
                waitTime = 1;
            }
        }

        // Try to read stuff from server
        int rd = read(vpnServSockfd, buf, 4);
        if (rd > 0) {
            // std::cout << "reading packet\n";
            // read the rest of length
            while (rd < 4) {
                // std::cout << "+" << std::endl;
                int bRead = read(vpnServSockfd, buf + rd, 4 - rd);
                if (bRead < 0) {
                    std::cout << "Error while reading length from server. Quitting." << std::endl;
                    return SERVER_ERROR;
                }
                rd += bRead;
            }
            int len = *(int*)buf;
            // std::cout << len << std::endl;
            if (len > BUF_LEN)
                std::cout << "Receiveing unusually long message from server of length " << len << std::endl;
            // read packet
            unsigned char* packetBuf = len > BUF_LEN ? (unsigned char*)malloc(len) : buf;
            if (packetBuf == NULL) {
                std::cout << "Can't get enough memory for receiving packet from server." << std::endl;
                return SERVER_ERROR;
            }
            while (rd < len) {
                int bRead = read(vpnServSockfd, buf + rd, len - rd);
                if (bRead < 0) {
                    if (errno !=  EAGAIN && errno != EWOULDBLOCK) {
                        std::cout << "Error while reading data from server. Quitting." << std::endl
                            << strerror(errno) << std::endl;
                        return SERVER_ERROR;
                    }
                } else {
                    rd += bRead;
                }
            }
            // decrypt
            // std::cout << "Encrypted : " << std::endl;
            // for (int i = 0; i < 128; i++) {
            //     std::cout << std::hex << (int)buf[i + 8];
            // }
            int decryptedtext_len = decryptms(buf + 8, rd - 8, aesKey, (const unsigned char*)"randomivstuffher", buf2);
            // std::cout << std::endl << "Decrypted " << std::endl;F822A294
            // for (int i = 0; i < 64; i++) {
            //     std::cout << std::hex << (int)buf[i];
            // }
            // std::cout << std::endl << std::dec << decryptedtext_len << std::endl;
            //if (std::chrono::duration<double, std::milli>(std::chrono::high_resolution_clock::now()-t_start).count() > 10) printf("ss: %.f\n", std::chrono::duration<double, std::milli>(std::chrono::high_resolution_clock::now()-t_start).count());//--
            // std::cout << std::dec << decryptedtext_len << std::endl;
            if (buf2[0] == CLOSE_TYPE) {
                close(clients[*(int*)(buf2 + 1)]);
                clients.erase(*(int*)(buf2 + 1));
            } else if (buf2[0] == SEND_TYPE) {
                write(clients[*(int*)(buf2 + 1)], buf2 + 5, decryptedtext_len - 5);
            }
            //if (std::chrono::duration<double, std::milli>(std::chrono::high_resolution_clock::now()-t_start).count() > 10) printf("sd: %.f\n", std::chrono::duration<double, std::milli>(std::chrono::high_resolution_clock::now()-t_start).count());//--

            if (len > BUF_LEN)
                free(packetBuf);
        }
        //if (std::chrono::duration<double, std::milli>(std::chrono::high_resolution_clock::now()-t_start).count() > 10) printf("sr: %.f\n", std::chrono::duration<double, std::milli>(std::chrono::high_resolution_clock::now()-t_start).count());//--
    }

    return OK;
};
