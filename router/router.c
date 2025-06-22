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
    int netlength;
} forwardingRow;



int interface_size = 0;

int interface_list_max_size = 100;

int *interface_list;


int forwarding_size = 0;

int forwarding_list_max_size = 100;

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
    newRow->netlength = netlength;


    forwarding_list[forwarding_size] = newRow;
    forwarding_size++;


    printf("last network mask: %x\n", forwarding_list[forwarding_size - 1]->mask);
}

void sortForwardTable() {
    struct forwardingRow *temp;

    for (int i = 0; i < forwarding_size - 1; i++) {
        int largest_index = i;
        int largest_mask = forwarding_list[i]->mask;

        for (int j = i + 1; j < forwarding_size; j++) {
            if (forwarding_list[j]->mask > largest_mask) {
                largest_mask = forwarding_list[j]->mask;
                largest_index = j;
            }
        }

        if (largest_index != i) {
            temp = forwarding_list[i];
            forwarding_list[i] = forwarding_list[largest_index];
            forwarding_list[largest_index] = temp;
        }
    }
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


int pickInterface(uint32_t network) {
    for (int i = 0; i < forwarding_size; i++) {
        struct forwardingRow  *row = forwarding_list[i];
        uint32_t mask = row->mask;

        if((network & mask) == (row->network & mask)) {
            return row->interface;
        }
    }

    // we shouldn't  ever be here, since ther should always be a 0.0.0.0/0 option
    return 0;
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

        int newInterface = pickInterface(hdr->dstipaddr);

        printf("Next interface: %d", newInterface);
        if (newInterface == *interface) {
            continue;
        }

        
        printf("size: %d\n", sizeof(packet));

        sendpkt(fd, newInterface, pkt);
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

    sortForwardTable();

    printf("Size: %d\n", forwarding_size);
    for (int i = 0; i < forwarding_size; i++) {
        printf("Network + Mask: %x, %d\n", forwarding_list[i]->network, forwarding_list[i]->netlength);
    }
    
    
    run();
    return 0;
}



/*

        ntohHdr(hdr);
        
        int length = hdr->length;
        htonHdr(hdr);

*/