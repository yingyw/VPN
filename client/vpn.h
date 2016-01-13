#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string>

#define BEGIN_HANDSHAKE_WORD 0xbebebebe
#define RSA_KEY_LENGTH 1024
#define RSA_E 65537
#define BEGIN_SERVER_RESPONSE_WORD 0x12345678
#define SERVER_PROXY_WORD 0x23456564
#define OPEN_TYPE 1
#define CLOSE_TYPE 2
#define SEND_TYPE 3

enum VPNError {
	OK,
	NO_SERVER,
	SERVER_ERROR
};

class VPN {
private:
    int vpnServSockfd;
    std::string vpnServAddr;
    int vpnServPort;

    int listPort;

    RSA *rsa;
    unsigned char aesKey[64];
    unsigned int aesLength;
public:
	bool forcePort;

    VPN(std::string vpnServAddr, int vpnServPort, int listPort);
    VPNError connect();
    VPNError listen();
};
