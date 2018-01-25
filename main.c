



#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <inttypes.h>

#define PKT_MAX_LENGTH  (4096)

int create_udp_socket(int port) {
    int sockFd = socket(AF_INET, SOCK_DGRAM, 0);

    if (sockFd < 0) {
        printf("failed to open socket.\n");
        return 1;
    }


    int sockopt = 1;
    setsockopt(sockFd, SOL_SOCKET, SO_REUSEADDR, (char *)&sockopt, sizeof(sockopt));

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family         = AF_INET;
    addr.sin_port           = htons(port);
    addr.sin_addr.s_addr    = htonl(INADDR_ANY);

    if(bind(sockFd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        printf("failed to bind socket\n");
        close(sockFd);
        return 1;
    }

    return sockFd;
}

int create_tcp_socket(int port) {
    return 0;
}

int send_udp_to(int sockFd, struct sockaddr_in *dest,
        const uint8_t *pkt, size_t pktLength) {
    // no connection


    return sendto(sockFd, pkt, pktLength, 0,
            (const struct sockaddr*)dest, 
            sizeof(struct sockaddr_in));

}

int connect_tcp_socket(const char *ip, int port) {
    return 0;
}

int main(int argc, char **argv) {

    int localport = atoi(argv[1]);
    int sockFd = create_udp_socket(localport);
    if (sockFd < 0) return -1;
    
    int loop = 1;

    while (loop) {
        char line[1024];
        printf(">> ");
        fgets(line, 1024, stdin);

        char *saved = NULL;
        char *seq = strtok_r(line, ";", &saved);

        while (seq) {
            while (*seq == ' ')  ++seq;     // skip space
            printf("seq: %s\n", seq);

            if (!memcmp(seq, "recv", 4)) {
                fd_set mask;
                FD_ZERO(&mask);
                FD_SET(sockFd, &mask);

                struct timeval tv;
                tv.tv_sec   = 10;
                tv.tv_usec  = 0;
                if (select(sockFd + 1, &mask, NULL, NULL, &tv) > 0) {
                    if (FD_ISSET(sockFd, &mask)) {
                        uint8_t pkt[PKT_MAX_LENGTH];
                        struct sockaddr_in sender;
                        socklen_t addrLen = sizeof(struct sockaddr_in);
                        ssize_t sz = recvfrom(sockFd, pkt, PKT_MAX_LENGTH, 0,
                                (struct sockaddr *)&sender, &addrLen);

                        uint8_t *ip = (uint8_t*)&sender.sin_addr;
                        printf("recv %s <= %d.%d.%d.%d:%d\n", (char*)pkt, 
                                ip[0], ip[1], ip[2], ip[3], 
                                sender.sin_port);

                    } else {
                        printf("something is wrong\n");
                    }
                }
            } else if (!memcmp(seq, "send", 4)) {
                struct sockaddr_in dest;
                uint8_t pkt[PKT_MAX_LENGTH];
                uint32_t port;
                uint8_t ip[4];

                sscanf(seq, "send %" SCNu8 ".%" SCNu8 ".%" SCNu8 ".%" SCNu8 ":%d %[^\n]",
                        &ip[0], &ip[1], &ip[2], &ip[3],
                        &port,
                        pkt);

                printf("send %s => %d.%d.%d.%d:%d\n", 
                        (const char *)pkt, ip[0], ip[1], ip[2], ip[3], port);

                dest.sin_family         = AF_INET;
                dest.sin_port           = htons(port);
                memcpy(&dest.sin_addr, ip, 4);

                send_udp_to(sockFd, &dest, pkt, strlen((const char *)pkt) + 1);

            } else if (!memcmp(seq, "quit", 4) ||
                    !memcmp(seq, "exit", 4)) {
                printf("exiting...\n");
                loop = 0;
                break;
            } else {
                printf("unknown command %s\n", seq);
                printf("Usage: \n");
                printf("\trecv              - recv from peer\n");
                printf("\tsend              - send to peer\n");
                printf("\texit|quit         - exit the program\n");
                break;
            }
            seq = strtok_r(NULL, ";", &saved);
        }
    }

    close(sockFd);


    return 0;
}
