#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <regex.h>

typedef struct radix_tree_node{

	char * key;
	char * value;
	int num_edges;
	struct radix_tree_edge ** edges;

}radix_node;

typedef struct radix_tree_edge{

	radix_node * target;
	char * label;
	//int label_len;

}radix_edge;

typedef struct pair{

	int m_len;
	int p_len;
	radix_edge * edge;
}pair;

typedef struct prefix_nodes_list{

	char ** keys;
	char ** values;
	int num_prefixes;

}prefix_list;

radix_node * init_root_node (void);
radix_node * init_node (char * key, char * value );
radix_edge * init_edge (radix_node * root, radix_node * new_node, char * key);
void insert (radix_node * root, char * key, char * value, const char * type);
void insert_in_array (radix_node * node, radix_edge * edge);
void set_edge_label (radix_edge * edge, char * key, int len);
prefix_list * allocate (prefix_list * list,  char * key, char * value);
int max_prefix_len (const char * str1, const char * prefix, int matched);
pair * find_node (radix_node * current, radix_edge * current_edge, const char * key, int match, int max_prefix);
void edgesplit (radix_edge * current_edge, pair * edge_pair, radix_edge * new_edge);
void prefix_load (radix_node * root, const char * key);
void prefix_print (prefix_list * list);
void free_pair (pair * edge_pair);
bool key_exists (radix_node * root, const char * key);
prefix_list * search (radix_node * node, prefix_list * list);
const char * parser (const char * str, const char * type);
char * find_type (const char * string);
void check_querystring_type_and_prefix_load (radix_node * domain_root, radix_node * ip_root,char * string);
void driver (char * string);
void free_prefix_list (prefix_list * list);
