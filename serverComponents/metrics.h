#ifndef _METRICS_H_
#define _METRICS_H_

#include <stdint.h>

/* @depreacted */
uint32_t getBytesRead();

/* @depreacted */
uint32_t getBytesSent();

uint32_t getAmountOfPost();

uint32_t getAmountOfHead();

uint32_t getAmountOfGet();

uint32_t getActiveClients();

uint32_t getHistoricClients();

/* @depreacted */
void addBytesRead(int n);

/* @depreacted */
void addBytesSent(int n);

void addGetRequest();

void addPostRequest();

void addHeadRequest();

void addClient();

void removeClient();

#endif
