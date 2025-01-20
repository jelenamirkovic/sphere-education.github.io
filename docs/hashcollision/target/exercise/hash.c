#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "hash.h"

#define SET_SIZE 4096

struct hlist_t {
    char *key;
    struct hlist_t *next;
};

struct hlist_t *buckets[SET_SIZE];

unsigned int hash(unsigned char* str, unsigned int len)
{
   unsigned int hash = 0;
   unsigned int i    = 0;

   for(i = 0; i < len; str++, i++)
   {
      hash = (*str) + (hash << 6) + (hash << 16) - hash;
   }

   return hash;
}

void hset_init(void)
{
    unsigned int i;
    for (i = 0; i < SET_SIZE; i++) {
        buckets[i] = NULL;
    }
}

void hset_add(unsigned char *key)
{
    unsigned int h, i;
    unsigned int key_len;
    struct hlist_t *hlist_node, *hlist_node_new;

    key_len = strlen(key);
    h = hash(key, key_len);
    i = h % SET_SIZE;

    hlist_node = buckets[i];
    while (hlist_node != NULL) {
        /* Entry alread exists */
        if (strncmp(hlist_node->key, key, key_len) == 0) {
            return;
        }
        hlist_node = hlist_node->next;
    }


    hlist_node_new = malloc(sizeof (struct hlist_t));
    hlist_node_new->key = malloc(sizeof (unsigned char) * key_len + 1);
    strncpy(hlist_node_new->key, key, key_len + 1);
    hlist_node_new->next = buckets[i];
    buckets[i] = hlist_node_new;
}

unsigned int hset_contains(unsigned char *key)
{
    unsigned int i, h;
    unsigned int key_len;
    struct hlist_t *hlist_node;
    key_len = strlen(key);
    h = hash(key, key_len);
    i = h % SET_SIZE;

    hlist_node = buckets[i];
    while (hlist_node != NULL) {
        if (strncmp(hlist_node->key, key, key_len) == 0) {
            return 1;
        }
        hlist_node = hlist_node->next;
    }
    return 0;
}

void hset_free(void)
{
    unsigned int i;
    struct hlist_t *hlist_node, *hlist_node_next;
    for (i = 0; i < SET_SIZE; i++) {
        hlist_node = buckets[i];
        while (hlist_node != NULL) {
            hlist_node_next = hlist_node->next;
            free(hlist_node->key);
            free(hlist_node);
            hlist_node = hlist_node_next;
        }
    }
}

void hset_dump(void)
{
    unsigned int i;
    struct hlist_t *hlist_node, *hlist_node_next;
    for (i = 0; i < SET_SIZE; i++) {
        hlist_node = buckets[i];
        if (hlist_node != NULL) {
            printf("%d:", i);
            while (hlist_node != NULL) {
                printf(" %s --> ", hlist_node->key);
                hlist_node = hlist_node->next;
            }
            printf("NULL\n");
        }
    }
}
