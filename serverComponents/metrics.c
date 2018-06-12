#include "metrics.h"

struct Metrics {
    uint32_t bytesRead;
    uint32_t bytesSent;
    uint32_t postAmount;
    uint32_t headAmount;
    uint32_t getAmount;
    uint32_t activeClients;
    uint32_t historicClients;
};

static struct Metrics metrics = {0}; 

uint32_t getBytesRead() {
    return metrics.bytesRead;
}

uint32_t getBytesSent() {
    return metrics.bytesSent;
}

uint32_t getAmountOfPost() {
    return metrics.postAmount;
}

uint32_t getAmountOfHead() {
    return metrics.headAmount;
}

uint32_t getAmountOfGet() {
    return metrics.getAmount;
}

uint32_t getActiveClients() {
    return metrics.activeClients;
}

uint32_t getHistoricClients() {
    return metrics.historicClients;
}

void addBytesRead(int n) {
    metrics.bytesRead += n;
}

void addBytesSent(int n) {
    metrics.bytesSent += n;
}

void addGetRequest() {
    metrics.getAmount += 1;
}

void addPostRequest() {
    metrics.postAmount += 1;
}

void addHeadRequest() {
    metrics.headAmount += 1;
}

void addClient() {
    metrics.activeClients += 1;
    metrics.historicClients += 1;
}

void removeClient() {
    if (metrics.activeClients == 0) {
        return;
    }
    metrics.activeClients -= 1;
}
