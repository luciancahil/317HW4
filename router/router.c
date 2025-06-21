#include <assert.h>
#include <stdint.h>
#include <stdio.h>

#include "ip.h"
#include "config.h"
#include "util.h"
#include "log.h"

/*
 * Add a forwarding entry to your forwarding table.  The network address is
 * given by the network and netlength arguments.  Datagrams destined for
 * this network are to be sent out on the indicated interface.
 */
void addForwardEntry(uint32_t network, int netlength, int interface) {
    // TODO: Implement this
}

/*
 * Add an interface to your router. 
 */
void addInterface(int interface) {
    printf("Interface: %d\n", interface);
    // TODO: Implement this
}

void run() {
    int port = getDefaultPort();
    int fd = udp_open(port);
    struct packet *pkt = malloc(12);
    printf("%d\n", port);

    int *interface = malloc(sizeof(int));
    // TODO: Implement this

    while(1){
        readpkt(fd, pkt, interface);

        ipheader *hdr = (ipheader *)&pkt->data;
        hdr->ttl = hdr->ttl - 1;
        if(hdr->ttl <= 0) {
            continue;
        }
        hdr->checksum = 0;
        unsigned short checksum = ipchecksum(hdr, sizeof(ipheader));
        hdr->checksum = checksum;

        ntohHdr(hdr);
        
        int length = hdr->length;
        htonHdr(hdr);


        
        printf("Cksum: %x\n", checksum);

        sendpkt(fd, 7, pkt);
    }
    printf("Hello after\n");
}


int main(int argc, char **argv) {
    logConfig("router", "packet,error,failure");
    printf("Yo!\n");
    char *configFileName = "router.config";
    if (argc > 1) {
	configFileName = argv[1];
    }
    configLoad(configFileName, addForwardEntry, addInterface);
    run();
    return 0;
}
