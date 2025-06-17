#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#ifdef _WIN32
#include <windows.h>
#else
#include <time.h>
#endif

#define ORDER       3
#define MAX_KEYS   (2*ORDER - 1)
#define MIN_KEYS    (ORDER - 1)

typedef struct BTreeNode {
    int is_leaf;                    // 1 se folha, 0 caso contrário
    int num_keys;                   // número atual de chaves
    char **keys;                    // array de chaves
    struct BTreeNode **children;    // ponteiros para filhos
} BTreeNode;

typedef struct {
    BTreeNode *root;                // raiz da árvore
    int node_count;                 // contador de nós totais
} BTree;


void delete_key(BTree *tree, BTreeNode *node, const char *key);

// Criação e liberação de nós e árvore
BTreeNode* create_node(int is_leaf) {
    BTreeNode *node = malloc(sizeof(BTreeNode));
    node->is_leaf   = is_leaf;
    node->num_keys  = 0;
    node->keys      = calloc(MAX_KEYS, sizeof(char*));
    node->children  = calloc(MAX_KEYS+1, sizeof(BTreeNode*));
    return node;
}

BTree* create_btree() {
    BTree *tree = malloc(sizeof(BTree));
    tree->root       = create_node(1);
    tree->node_count = 1;
    return tree;
}

void free_node(BTreeNode *node) {
    if (!node) return;
    for (int i = 0; i < node->num_keys; i++)
        free(node->keys[i]);
    free(node->keys);
    for (int i = 0; i <= node->num_keys; i++)
        free_node(node->children[i]);
    free(node->children);
    free(node);
}

void free_btree(BTree *tree) {
    free_node(tree->root);
    free(tree);
}


// Inserção
void split_child(BTree *tree, BTreeNode *parent, int index) {
    BTreeNode *child     = parent->children[index];
    BTreeNode *new_child = create_node(child->is_leaf);
    tree->node_count++;

    new_child->num_keys = ORDER - 1;
    for (int i = 0; i < ORDER-1; i++) {
        new_child->keys[i] = child->keys[i + ORDER];
        child->keys[i + ORDER] = NULL;
    }
    if (!child->is_leaf) {
        for (int i = 0; i < ORDER; i++) {
            new_child->children[i] = child->children[i + ORDER];
            child->children[i + ORDER] = NULL;
        }
    }
    child->num_keys = ORDER - 1;

    for (int i = parent->num_keys; i > index; i--) {
        parent->children[i+1] = parent->children[i];
        parent->keys[i]       = parent->keys[i-1];
    }
    parent->children[index+1] = new_child;
    parent->keys[index]       = child->keys[ORDER-1];
    child->keys[ORDER-1]      = NULL;
    parent->num_keys++;
}

void insert_non_full(BTree *tree, BTreeNode *node, const char *key) {
    int i = node->num_keys - 1;
    if (node->is_leaf) {
        while (i >= 0 && strcmp(key, node->keys[i]) < 0) {
            node->keys[i+1] = node->keys[i];
            i--;
        }
        node->keys[i+1] = strdup(key);
        node->num_keys++;
    } else {
        while (i >= 0 && strcmp(key, node->keys[i]) < 0) i--;
        i++;
        if (node->children[i]->num_keys == MAX_KEYS) {
            split_child(tree, node, i);
            if (strcmp(key, node->keys[i]) > 0) i++;
        }
        insert_non_full(tree, node->children[i], key);
    }
}

void insert_key(BTree *tree, const char *key) {
    BTreeNode *root = tree->root;
    if (root->num_keys == MAX_KEYS) {
        BTreeNode *new_root = create_node(0);
        new_root->children[0] = root;
        tree->root = new_root;
        tree->node_count++;
        split_child(tree, new_root, 0);
        insert_non_full(tree, new_root, key);
    } else {
        insert_non_full(tree, root, key);
    }
}

//Remoção
char* get_predecessor(BTreeNode *node) {
    while (!node->is_leaf)
        node = node->children[node->num_keys];
    return node->keys[node->num_keys - 1];
}

char* get_successor(BTreeNode *node) {
    while (!node->is_leaf)
        node = node->children[0];
    return node->keys[0];
}

void borrow_from_prev(BTreeNode *node, int idx) {
    BTreeNode *child   = node->children[idx];
    BTreeNode *sibling = node->children[idx-1];
    for (int i = child->num_keys -1; i >= 0; i--)
        child->keys[i+1] = child->keys[i];
    if (!child->is_leaf) {
        for (int i = child->num_keys; i >= 0; i--)
            child->children[i+1] = child->children[i];
    }
    child->keys[0] = node->keys[idx-1];
    if (!child->is_leaf)
        child->children[0] = sibling->children[sibling->num_keys];
    node->keys[idx-1] = sibling->keys[sibling->num_keys -1];
    sibling->keys[sibling->num_keys -1] = NULL;
    child->num_keys++;
    sibling->num_keys--;
}

void borrow_from_next(BTreeNode *node, int idx) {
    BTreeNode *child   = node->children[idx];
    BTreeNode *sibling = node->children[idx+1];
    child->keys[child->num_keys] = node->keys[idx];
    node->keys[idx] = sibling->keys[0];
    for (int i = 1; i < sibling->num_keys; i++)
        sibling->keys[i-1] = sibling->keys[i];
    sibling->keys[sibling->num_keys -1] = NULL;
    if (!child->is_leaf) {
        child->children[child->num_keys+1] = sibling->children[0];
        for (int i = 1; i <= sibling->num_keys; i++)
            sibling->children[i-1] = sibling->children[i];
    }
    child->num_keys++;
    sibling->num_keys--;
}

void merge_children(BTree *tree, BTreeNode *node, int idx) {
    BTreeNode *child   = node->children[idx];
    BTreeNode *sibling = node->children[idx+1];
    child->keys[child->num_keys] = node->keys[idx];
    child->num_keys++;
    for (int i = 0; i < sibling->num_keys; i++) {
        child->keys[child->num_keys + i] = sibling->keys[i];
        sibling->keys[i] = NULL;
    }
    if (!child->is_leaf) {
        for (int i = 0; i <= sibling->num_keys; i++) {
            child->children[child->num_keys + i] = sibling->children[i];
            sibling->children[i] = NULL;
        }
    }
    child->num_keys += sibling->num_keys;
    for (int i = idx; i < node->num_keys -1; i++) {
        node->keys[i] = node->keys[i+1];
        node->children[i+1] = node->children[i+2];
    }
    node->keys[node->num_keys-1] = NULL;
    node->children[node->num_keys] = NULL;
    node->num_keys--;
    free(sibling->keys);
    free(sibling->children);
    free(sibling);
    tree->node_count--;
}

void fill_child(BTree *tree, BTreeNode *node, int idx) {
    if (idx > 0 && node->children[idx-1]->num_keys > MIN_KEYS) {
        borrow_from_prev(node, idx);
    } else if (idx < node->num_keys && node->children[idx+1]->num_keys > MIN_KEYS) {
        borrow_from_next(node, idx);
    } else {
        if (idx < node->num_keys)
            merge_children(tree, node, idx);
        else
            merge_children(tree, node, idx-1);
    }
}

void delete_from_leaf(BTreeNode *node, int idx) {
    free(node->keys[idx]);
    for (int i = idx+1; i < node->num_keys; i++)
        node->keys[i-1] = node->keys[i];
    node->keys[node->num_keys-1] = NULL;
    node->num_keys--;
}

void delete_from_non_leaf(BTree *tree, BTreeNode *node, int idx) {
    char *key = node->keys[idx];
    if (node->children[idx]->num_keys >= ORDER) {
        char *pred = get_predecessor(node->children[idx]);
        free(node->keys[idx]);
        node->keys[idx] = strdup(pred);
        delete_key(tree, node->children[idx], pred);
    } else if (node->children[idx+1]->num_keys >= ORDER) {
        char *succ = get_successor(node->children[idx+1]);
        free(node->keys[idx]);
        node->keys[idx] = strdup(succ);
        delete_key(tree, node->children[idx+1], succ);
    } else {
        merge_children(tree, node, idx);
        delete_key(tree, node->children[idx], key);
    }
}

void delete_key(BTree *tree, BTreeNode *node, const char *key) {
    int idx = 0;
    while (idx < node->num_keys && strcmp(key, node->keys[idx]) > 0)
        idx++;
    if (idx < node->num_keys && strcmp(key, node->keys[idx]) == 0) {
        if (node->is_leaf)
            delete_from_leaf(node, idx);
        else
            delete_from_non_leaf(tree, node, idx);
    } else {
        if (node->is_leaf) return;
        int is_last = (idx == node->num_keys);
        if (node->children[idx]->num_keys < ORDER)
            fill_child(tree, node, idx);
        if (is_last && idx > node->num_keys)
            delete_key(tree, node->children[idx-1], key);
        else
            delete_key(tree, node->children[idx], key);
    }
}

void remove_key(BTree *tree, const char *key) {
    delete_key(tree, tree->root, key);
    if (tree->root->num_keys == 0) {
        BTreeNode *old = tree->root;
        if (old->is_leaf) {
            tree->root = create_node(1);
            tree->node_count++;
        } else {
            tree->root = old->children[0];
        }
        free(old->keys);
        free(old->children);
        free(old);
        tree->node_count--;
    }
}


void create_file(const char *filename, char ***tokens, int *token_count) {
    FILE *f = fopen(filename, "r");
    if (!f) { perror("Erro ao abrir arquivo"); exit(1); }
    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    fseek(f, 0, SEEK_SET);
    char *buf = malloc(size+1);
    fread(buf,1,size,f);
    buf[size]='\0';
    fclose(f);

    *token_count = 0;
    *tokens = NULL;
    char *tok = strtok(buf, " \t\n\r.,;:!?()[]{}'\"");
    while (tok) {
        for (char *p=tok; *p; p++) *p = tolower(*p);
        *tokens = realloc(*tokens, (*token_count+1)*sizeof(char*));
        (*tokens)[(*token_count)++] = strdup(tok);
        tok = strtok(NULL, " \t\n\r.,;:!?()[]{}'\"");
    }
    free(buf);
}

int main() {
    char **tokens;
    int token_count;

    create_file("blivre.txt", &tokens, &token_count);

    BTree *tree = create_btree();

    // Abre um único CSV para ambos insert e delete
    FILE *log_file = fopen("../B-tree.csv", "w");
    if (!log_file) { perror("Erro ao criar B-tree.csv"); return 1; }
    fprintf(log_file, "operation,node_count,time_us\n");

    #ifdef _WIN32
    LARGE_INTEGER freq; QueryPerformanceFrequency(&freq);
    #endif
    long long cum_time = 0;

    for (int i = 0; i < token_count; i++) {
        #ifdef _WIN32
        LARGE_INTEGER start, end; QueryPerformanceCounter(&start);
        #else
        struct timespec start, end; clock_gettime(CLOCK_MONOTONIC, &start);
        #endif

        insert_key(tree, tokens[i]);

        #ifdef _WIN32
        QueryPerformanceCounter(&end);
        long long ns = (end.QuadPart - start.QuadPart) * 1000000000LL / freq.QuadPart;
        #else
        clock_gettime(CLOCK_MONOTONIC, &end);
        long long ns = (end.tv_sec - start.tv_sec)*1000000000LL + (end.tv_nsec - start.tv_nsec);
        #endif
        long long us = ns/1000;
        if (us == 0) { cum_time++; us = cum_time; }
        else { cum_time = us; }

        fprintf(log_file, "insert,%d,%lld\n", tree->node_count, us);
    }

    int *del_order = malloc(token_count*sizeof(int));
    for (int i = 0; i < token_count; i++) del_order[i] = i;
    srand(time(NULL));
    for (int i = token_count-1; i > 0; i--) {
        int j = rand() % (i+1);
        int tmp = del_order[i]; del_order[i] = del_order[j]; del_order[j] = tmp;
    }

    cum_time = 0;
    for (int i = 0; i < token_count; i++) {
        #ifdef _WIN32
        LARGE_INTEGER start, end; QueryPerformanceCounter(&start);
        #else
        struct timespec start, end; clock_gettime(CLOCK_MONOTONIC, &start);
        #endif

        remove_key(tree, tokens[del_order[i]]);

        #ifdef _WIN32
        QueryPerformanceCounter(&end);
        long long ns = (end.QuadPart - start.QuadPart) * 1000000000LL / freq.QuadPart;
        #else
        clock_gettime(CLOCK_MONOTONIC, &end);
        long long ns = (end.tv_sec - start.tv_sec)*1000000000LL + (end.tv_nsec - start.tv_nsec);
        #endif
        long long us = ns/1000;
        if (us == 0) { cum_time++; us = cum_time; }
        else { cum_time = us; }

        fprintf(log_file, "delete,%d,%lld\n", tree->node_count, us);
    }

    fclose(log_file);

    for (int i = 0; i < token_count; i++) free(tokens[i]);
    free(tokens);
    free(del_order);
    free_btree(tree);
    return 0;
}
