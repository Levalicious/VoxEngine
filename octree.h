#pragma once
#include "common.h"

/* TODO: Reimplement Octree based on https://hastebin.com/oyanepocos.h in this format */

/* TODO: Find out how to stream it to the GPU */

const static uint32_t LEAF = 1u << 31u;

typedef struct {
    uint32_t children[8];
    uint32_t color;
    uint32_t is_leaf_depth;
} Node;

bool is_leaf(Node* node) {
    return (node->is_leaf_depth & LEAF) != 0;
}

typedef struct {
    Node* nodes;
    uint32_t len;
    uint32_t size;
    uint32_t lost;
    uint32_t diam;
} Octree;

Node* initNodes() {
    Node* out = malloc(sizeof(Node));
    for (int i = 0; i < 8; i++) {
        out->children[i] = 0;
    }
    out->color = 0x0000ff;
    out->is_leaf_depth = 1u << 31u;

    return out;
}

Octree* newOctree(uint32_t diam) {
    Octree* out = malloc(sizeof(Octree));
    out->diam = diam;
    out->nodes = initNodes();
    out->len = 1;
    out->size = 1;
    out->lost = 0;
    return out;
}

void reallocTree(Octree* tree) {
    tree->size *= 2;
    tree->nodes = realloc(tree->nodes, sizeof(Node) * tree->size);
}

void addChildren(Octree* tree, uint32_t pos, uint32_t value) {
    if (tree->len + 8 >= tree->size) reallocTree(tree);

    //tree->nodes[pos]
}