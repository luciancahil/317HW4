#include <assert.h>
#include <stdint.h>
#include <stdio.h>

#include "ip.h"
#include "config.h"
#include "util.h"
#include "log.h"


typedef struct forwardingRow {
    uint32_t network;
    uint32_t mask;
    int interface;
} forwardingRow;



int interface_size = 0;

int interface_list_max_size = 12;

int *interface_list;


int forwarding_size = 0;

int forwarding_list_max_size = 12;

struct forwardingRow **forwarding_list;

/*
 * Add a forwarding entry to your forwarding table.  The network address is
 * given by the network and netlength arguments.  Datagrams destined for
 * this network are to be sent out on the indicated interface.
 */



uint32_t getMask(int mask_size) {
    if(mask_size == 0){
        return 0;
    }

    return 0xFFFFFFFF << (32 - mask_size);
}

 void addForwardEntry(uint32_t network, int netlength, int interface) {
    // TODO: Implement this

    struct forwardingRow *newRow = (struct forwardingRow *)malloc(sizeof(forwardingRow));

    newRow->network = network;
    newRow->mask = getMask(netlength);
    newRow->interface = interface;



    forwarding_list[forwarding_size] = newRow;
    forwarding_size++;


    printf("last network mask: %x\n", forwarding_list[forwarding_size - 1]->mask);
}

/*
 * Add an interface to your router. 
 */
void addInterface(int interface) {
    printf("Interface: %d\n", interface);

    interface_list[interface_size] = interface;

    interface_size += 1;
    printf("Num interfaces: %d\n", interface_size);
    // TODO: Implement this
}

void run() {
    int port = getDefaultPort();
    int fd = udp_open(port);
    struct packet *pkt = malloc(sizeof(packet));
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


        
        printf("size: %d\n", sizeof(packet));

        sendpkt(fd, 7, pkt);
    }
}


int main(int argc, char **argv) {
    logConfig("router", "packet,error,failure");
    printf("Yo!\n");
    interface_list = malloc(12 * sizeof(int));
    forwarding_list = malloc(12 * sizeof(forwardingRow*));

    char *configFileName = "router.config";
    if (argc > 1) {
	configFileName = argv[1];
    }
    configLoad(configFileName, addForwardEntry, addInterface);
    run();
    return 0;
}
