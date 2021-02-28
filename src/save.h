#ifndef _SAVE_H
#define _SAVE_H

#include "blockchain/blockchain_public.h"

int save(const char *filename, Node *head_node);
int load(char *filename);

#endif // _SAVE_H
