#include "radixtree.h"

//function to initialisee root node
radix_node * init_root (){

	radix_node * rnode = malloc(sizeof (radix_node) );
	rnode -> num_edges = 0;
	return rnode;
}

//function to initialise tree node with key and value
radix_node * init_node (char * key, char * value){

	radix_node * new_node = malloc (sizeof (radix_node) );
	new_node -> key = malloc (strlen (key) + 1 );
	memcpy (new_node -> key, key, strlen (key) + 1 );
	new_node -> value = malloc (strlen (value) + 1);
	memcpy (new_node -> value, value, strlen (value) + 1);
	new_node -> num_edges = 0;
	return new_node;

}

//initialising an edge and adding to its corresponding node 
radix_edge * init_edge (radix_node * root, radix_node * new_node, char * key){

	radix_edge * edge = malloc (sizeof (radix_edge) );
	set_edge_label (edge, key, strlen(key) );
	edge -> target = new_node;
	insert_in_array (root, edge);
	return edge;
}

//function to insert values into the edges to the corresponding nodes 
void insert_in_array (radix_node * node, radix_edge * edge){

		radix_edge ** temp = realloc (node -> edges, ( node -> num_edges + 1 ) * sizeof (radix_edge*) );

		if (temp == NULL){
			free(temp);
			exit(1);
		}

		node -> edges = temp;


	node -> edges[node -> num_edges] = edge;
	node -> num_edges++;
}

//function to set an edge label
void set_edge_label (radix_edge * edge, char * key, int len){

	if (edge == NULL){

		return;
	}

	edge -> label = malloc (len + 1);
	memcpy (edge -> label, key, len);
}

//function to find the maximum matching prefix length 
int max_prefix_len (const char * key, const char * prefix, int matched){

	if ( key == NULL || prefix == NULL )
	{
		return 0;
	}
		int key_len = strlen (key);
		int pre_len = strlen (prefix);
		int i = 0;

	while ( (i < key_len && i < pre_len)  && key[i + matched] == prefix[i] ){
		i++;
	}

	return i;
}

//functions returns the a pointer the an object pair
pair * find_edge (radix_node * current_node, radix_edge * current_edge, const char * key, int match, int max_prefix){

	int i;

	if (current_node != NULL)
		for (i = 0; i < current_node -> num_edges; i++){ //traversing through the nodes of an edge

			int prefix_len = max_prefix_len (key, current_node -> edges[i] -> label, match);//finding a match

			if ( prefix_len > 0 && prefix_len < strlen (current_node -> edges[i] -> label))
					return find_edge (NULL, current_node -> edges[i], key, match + prefix_len, prefix_len); //in case the match does not match the entire edge label return

			if (prefix_len > 0){
				return find_edge (current_node ->edges[i] -> target, current_node -> edges[i], key, match + prefix_len, prefix_len); //if there is a complete edge label match proceed to next node and edge to check for further matching
			}

		}

	pair  * p = malloc (sizeof (pair) );
	p -> edge = current_edge;	//stores the final edge where a match occurs
	p -> m_len = match;		//total matched length
	p -> p_len = max_prefix;	//matched length in the final edge where a match occurs

	return p;
}

void edge_split (radix_edge * final_prefix_edge, pair * edge_pair, radix_edge * new_edge ){

	radix_edge * old_new_edge = malloc (sizeof (radix_edge) ); //creating a new edge with label without the common prefix 

	set_edge_label (old_new_edge, &final_prefix_edge -> label[edge_pair -> p_len], strlen (final_prefix_edge -> label) - edge_pair -> p_len);//setting the new edge with label without the common prefix 
	old_new_edge -> target = final_prefix_edge -> target; //pointing this edge to the target pointed by the final matched edge

	radix_node * inter_mediate_node = malloc (sizeof (radix_node) );//creating an intermediate node which will hold the two new edges 
	final_prefix_edge -> target = inter_mediate_node;

	insert_in_array (inter_mediate_node, old_new_edge); //inserting the two new edges to the intermediate node
	insert_in_array (inter_mediate_node, new_edge);

	set_edge_label (final_prefix_edge, final_prefix_edge -> label, edge_pair -> p_len); //resetting the label of the final matched edge

}

//function to free a pair object
void free_pair (pair * edge_pair){

	edge_pair -> m_len = 0;
	edge_pair -> p_len = 0;
	edge_pair -> edge = NULL;
	free (edge_pair);
	edge_pair = NULL;
}

//if checking if a key exists so that we do not re-insert a domain name
bool key_exists (radix_node * root, const char * key){

	pair * edge_pair = find_edge (root, NULL, key, 0, 0); //finding the edge where the last character match occurs

	if ( edge_pair -> edge == NULL ){

		free_pair (edge_pair);
		return false;
	}

	char * edge_label = edge_pair -> edge -> label;

	if (edge_pair -> m_len == strlen (key) && strlen (edge_label) == edge_pair -> p_len ){//condition to check if a node a complete match occurs

		free_pair (edge_pair);
		return true;
	}

	free_pair (edge_pair);
	return false;
}

void insert (radix_node * root, char * key, char * value, const char * type){

	if (key_exists (root, key) && !strcmp (type, "domain") ){// to prevent re-insertion of domains

		printf ("Key: %s<br>", key); 
		return;
	}

	radix_node * new_node = init_node (key, value); // initialising a node

	if (root -> num_edges == 0){//when the tree is empty

		radix_edge * edge = init_edge (root, new_node, key);
		return;
	}

	pair * edge_pair = find_edge (root, NULL, key, 0, 0);//finding the final matched edge label
	radix_edge * final_prefix_edge = edge_pair -> edge;//the final matched edge

	if (final_prefix_edge == NULL ){// if the no edge is found insert at the root

		radix_edge * edge = init_edge (root, new_node, key);
		return;

	}

	radix_edge * new_edge = malloc (sizeof (radix_edge) );//initialising a new edge
	set_edge_label (new_edge, &key[edge_pair -> m_len], strlen (key) - edge_pair -> m_len); // creating the edge label by setting it without the common prefix
	new_edge -> target = new_node;//assigning the edge its corresponding targer node which contains the value

	if (strlen (final_prefix_edge -> label) == edge_pair -> p_len ){//if a complete edge match is found

		if ( final_prefix_edge -> target ->  num_edges == 0)
			edge_split (final_prefix_edge, edge_pair, new_edge);//splitting in case of a complete word match and the edge is a prefix is a prefix of the string

		else
			insert_in_array (final_prefix_edge -> target, new_edge);//insert into the node pointed to by a  to node where 
	}

	else{

		edge_split (final_prefix_edge, edge_pair, new_edge); //splitting the edge if a there is a not a complete match at an edge 
	}

	//free_pair ( edge_pair ); // freeing the allocated memory

}

void free_prefix_list (prefix_list * list){//free the list of all found nodes

	int i;

	for (i = 0; i < list -> num_prefixes; i++){

		free (list -> keys[i]);
		free (list -> values[i]);
		list -> keys[i] = NULL;
		list -> values[i] = NULL;
	}

	free (list -> keys);
	free (list -> values);
	list -> keys = NULL;
	list -> values = NULL;
	list -> num_prefixes = 0;
	free (list);
	list = NULL;
}

void prefix_print (prefix_list * list){//printing the list of all found nodes

	int i;

	if (list -> num_prefixes == 0){
		printf ("%s\n", list -> keys[0]);
		return;
	}

	for (i = 0; i < list -> num_prefixes; i++)
		printf ("%s: %s\n", list -> keys[i], list -> values[i]);

}

prefix_list * allocate (prefix_list * list,  char * key, char * value){//allocating all keys and values to a struct so that it can be printed out later 

	list -> keys = realloc (list -> keys, sizeof (char*) * (list -> num_prefixes + 1) );
	list -> keys[list -> num_prefixes ] = malloc (strlen (key) + 1 );
	memcpy (list -> keys[list -> num_prefixes ], key, strlen (key) + 1);

	list -> values = realloc (list -> values, sizeof (char*) * (list -> num_prefixes + 1) );
	list -> values[list -> num_prefixes ] = malloc (strlen (value) + 1 );
	memcpy (list -> values[list -> num_prefixes ], value, strlen (value) + 1);
	list -> num_prefixes ++;
	return list;


}

void prefix_load (radix_node * root, const char * prefix_key){

	pair * radix_pair =  find_edge (root, NULL, prefix_key, 0, 0);//finding the final matched edge
	radix_edge * prefix_match_edge = radix_pair -> edge ; //final matched edge
	prefix_list * list = malloc (sizeof (prefix_list) );
	list -> values = malloc (sizeof (char *) );
	list -> keys = malloc (sizeof (char *) );
	list -> num_prefixes = 0;

	if (prefix_match_edge == NULL || strlen (prefix_key) != radix_pair -> m_len) { //if match length is zero or match doesnt exist return

		list = allocate(list, "No strings found", "");
		list -> num_prefixes --;
		prefix_print (list);
		return;
	}

	radix_node * target_node = prefix_match_edge -> target; // target node of final matched edge if 

	if (target_node -> num_edges == 0 ){//if target node has no desecendants store it

		list = allocate (list, target_node -> key, target_node -> value);
		prefix_print(list);
		return;
	}

	list = search ( target_node, list ); // if target node has desecendants then continue seraching for more labels with a common prefix
	prefix_print(list);		    // print the list
	free_prefix_list (list);	    //free the list
}

prefix_list * search (radix_node * target_node, prefix_list * list){

	int i;

	for (i = 0; i < target_node -> num_edges; i++){//traversing through all edges and checking if their corresponding target nodes no have no further descendants

		if ( target_node -> edges[i] -> target ->  num_edges == 0 ){

			list = allocate (list, target_node -> edges[i] -> target -> key, target_node -> edges[i] -> target -> value);//if so allocate it to the list
		}

		else
			list = search (target_node -> edges[i] -> target, list );//if descendants exist continue to search until we reach the a target node without descendants
	}

	return list;
}

const char * parser  (const char * str, const char * type){

	if ( !strcmp(type,"domain") && strncmp(str, "www.", 4) ){//checking for whether websites start with www. or not

		char * querystring = malloc( 5 + strlen (str) );
		strcpy (querystring, "www.");
		return strcat (querystring, str);
	}

	return str ;

}

//function to find out the type of the querystring
char * find_type (const char * string){

	char * status;
	regex_t re;
	const char * pattern = "^([01]?[0-9][0-9]?\\.|2[0-4][0-9]\\.|25[0-5]\\.){0,3}([01]?[0-9][0-9]?|2[0-4][0-9]|25[0-5])?$";//pattern matching to find out whether query string is an ip address or domain name
	regmatch_t rm[2];

	if (strlen (string) == 0){//checking for empty strings
		status = malloc (strlen ("no type") + 1);
		memcpy (status,"no type", strlen("no type") + 1);
		return status;
	}

	if (regcomp (&re, pattern, REG_EXTENDED) != 0){// compiling the pattern into regex format and if returning if it fails
		regfree(&re);
		printf ("Regex pattern failed to compile<br>");
		status = malloc (strlen("no type") + 1);
		memcpy (status,"no type", strlen ("no type") + 1);
		return status;
	}

	else if (regexec (&re, string, 1, rm, 0) == 0){//checking if the pattern is a valid ip address search
		regfree(&re);
		status = malloc (strlen("ip") + 1);
		memcpy (status, "ip", strlen("ip") + 1);
		return status;
	}

	else{//domain name
		regfree(&re);
		status = malloc (strlen("domain") + 1);
		memcpy (status, "domain", strlen("domain") + 1);
		return status;

	}
}


void check_querystring_type_and_prefix_load (radix_node * domain_root, radix_node * ip_root, char * string){

	char * type = find_type (string);//type
	const char * querystring = parser (string, type);//string to be used to search

	if (!strcmp (type, "no type") ){//invalid string

		printf ("Invalid string<br>");
		free(type);
		return;
	}

	else if (!strcmp (type, "domain") )
		prefix_load (domain_root, querystring);//search for all domains

	else
		prefix_load (ip_root, querystring);//search for all ip addresses

	free (type);
}

void driver (char *string){

	radix_node * domain_root = init_root ();//initialising domain name radix tree root
	radix_node * ip_root = init_root();//initialising ip address radix tree root
	FILE * file;
	file = fopen("domainnames.txt","r");
	char str[60];
	char * delimiters = ",";
	char * running;
	char * str1;
	char * str2;
	const char * querystring;

	while ( fgets (str, 60, file) ){//reading from the file and splitting the string into domain name and ip address 

		str[strlen (str) - 1] = '\0';
		running = strdup (str);

		str1 = strsep (&running, delimiters);
		str2 = strsep (&running, delimiters);
		insert (domain_root, str1, str2, "domain");
		insert (ip_root, str2, str1, "ip");
		free (running);
	}

	check_querystring_type_and_prefix_load (domain_root, ip_root, string);//run the search
	fclose (file);//close the file

}

int main (){

	driver("google");
}
