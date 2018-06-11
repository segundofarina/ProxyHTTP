#ifndef _METRICS_H_
#define _METRICS_H_

#include <stdint.h>

uint32_t getBytesRead();

uint32_t getBytesSent();

uint32_t getAmountOfPost();

uint32_t getAmountOfHead();

uint32_t getAmountOfGet();

uint32_t getActiveClients();

uint32_t getHistoricClients();

void addBytesRead(int n);

void addBytesSent(int n);

void addGetRequest();

void addPostRequest();

void addHeadRequest();

void addClient();

void removeClient();

#endif
