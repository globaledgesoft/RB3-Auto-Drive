#include "client.hpp"

/************************************************************************
* name : getInAddr
* function: setting thee address
************************************************************************/
void *getInAddr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET)
    {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

/************************************************************************
* name : getFd
* function: Establish a connection with server and retuens a socket
************************************************************************/
int getFd()
{
    int sockfd;
    struct addrinfo hints, *servinfo, *p;
    std::string host_ip = LOOPBACK_ADDR;
    int rv;
    char s[INET6_ADDRSTRLEN];

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    if ((rv = getaddrinfo(host_ip.c_str(), PORT, &hints, &servinfo)) != 0)
    {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    for (p = servinfo; p != NULL; p = p->ai_next)
    {
        if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1)
        {
            perror("client: socket");
            continue;
        }

        if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1)
        {
            close(sockfd);
            perror("client: connect (will try again)");
            continue;
        }

        break;
    }

    if (p == NULL)
    {
        fprintf(stderr, "client: failed to connect\n");
        return 2;
    }

    inet_ntop(p->ai_family, getInAddr((struct sockaddr *)p->ai_addr),
            s, sizeof s);
    printf("client: connecting to %s\n", s);

    freeaddrinfo(servinfo);

    return sockfd;
}

/************************************************************************
* name : predictDlr
* function: returns the predictions from dlr models 
************************************************************************/
std::vector<float> predictDlr(int sockfd, std::vector<float> ipBuf)
{
    std::vector<float> opBuf (DLR_SEG_H*DLR_SEG_W, 0.0);
    int recvbytes = 0;
    int sendbytes = 0;
    int total_send = 0;
    int total_recv = 0;

    while(total_send < (DLR_SEG_H * DLR_SEG_W * DLR_SEG_C * sizeof(float))) {
        if ((sendbytes = send(sockfd,(uint8_t*)ipBuf.data() + total_send, NO_BYTES, 0)) == -1) {
            perror("send");
            exit(1);
        }
        total_send+=sendbytes;
    }

    while(total_recv < (DLR_SEG_H * DLR_SEG_W * sizeof(float))) {
        if ((recvbytes = recv(sockfd, (uint8_t*)opBuf.data() + total_recv, NO_BYTES, 0)) == -1)
        {
            perror("recv");
            exit(1);
        }
        if((total_recv == 0) && (opBuf[0] == -1.0)) {
            return opBuf;
        }
        total_recv+=recvbytes;
    }
    return opBuf;
}
