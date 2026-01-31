#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_SIZE 15
#define MAX_INPUT_SIZE 100
#define MAX_COMMAND_SIZE 10
#define MAX_NAME_LENGTH 20
#define MAX_PARAMETER_LENGTH 20

typedef struct Node Node;

#include <stdlib.h>

// TODO: Adjust this
void clear_screen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}


struct Node{
    char name[MAX_SIZE];

    Node** items;
    size_t item_count;
    size_t item_capacity;

    Node** children;
    Node* parent;
    size_t ch_count;
    size_t ch_capacity;

};


typedef struct {
    char username[MAX_SIZE];
    Node* root;
    Node* current;
}Navigation;


typedef enum {
    mkdir,
    ls
}Commands;


typedef struct {
    char command[MAX_COMMAND_SIZE];
    char* parameters[MAX_PARAMETER_LENGTH];
}Tokenized;


int error_realloc() {
    fprintf(stderr, "Error: Couldn't reallocate memory!\n");
    return 1;
}


int error_calloc() {
    fprintf(stderr, "Error: Couldn't allocate memory!\n");
    return 1;
}


/**
* Function for appending a new node as a child node to a given node
* Dynamically reallocates memory in case memory is used up.
*/
int da_append_child(Node* parent, Node* child) {
    if (parent->ch_count >= parent->ch_capacity) {
        if (parent->ch_capacity == 0) parent->ch_capacity = 256;
        else parent->ch_capacity *= 2;

        Node** temp = realloc(
            parent->children,
            parent->ch_capacity * sizeof(Node*)
        );

        if (temp == NULL) {
            return error_realloc();
        }

        parent->children = temp;
    }
    parent->children[parent->ch_count++] = child;

    return 0;
}


/**
* Given a name, tries to find the node with that name.
* If no node with that name exists, returns null pointer
*/
Node* getNodeFromName(Node* current, const char name[MAX_SIZE]) {
    for(size_t i = 0; i < (*current).ch_count; i++) {

        Node* child = current->children[i];

        if (strcmp(child->name, name) == 0) {
            return child;
        }
    }

    return NULL;
}


/**
* Given the current node and a name, creates a new node with the given name
*/
int make_directory(Node* current, const char name[MAX_SIZE]) {
    Node* node = calloc(1, sizeof(Node));
    if (!node) return 1;

    strcpy(node->name, name);
    node->parent = current;

    int ret = da_append_child(current, node);

    return ret;
}


//TODO: Check for empty
/**
* Called when user uses command "ls"
* Lists all contents of current node
*/
int list_directory(Node* current) {
    for (size_t i = 0; i < current->ch_count; i++) {
        Node* child = current->children[i];
        printf("%s\n", child->name);
    }
}

/**
* Tokenizes a given string into its commands and its parameters
*/
int tokenize_input(char userInput[MAX_INPUT_SIZE]) {
    //Tokenized* tokenized = calloc(1, sizeof(Tokenized));

    char* token;
    char* whiteSpace = " \t\n\f\r\v";
    token = strtok(userInput, whiteSpace);

    int isCommand = 1;

    while (token != NULL) {
        if(isCommand) {
            printf("Command = %s\n", token);
            isCommand = 0;
        }

        else {
            printf("Parameter = %s\n\n", token);
        }

        token = strtok(NULL, whiteSpace);
    }

    return 0;
}


int vifima_loop(Navigation* navigation) {
    char userInput[MAX_INPUT_SIZE] = {0};
    printf("[%s @ %s]$ ", navigation->username, navigation->root->name);
    fgets(userInput, MAX_INPUT_SIZE, stdin);
    userInput[strcspn(userInput, "\n")] = 0;

    int ret = tokenize_input(userInput);
    memset(userInput,0,strlen(userInput));

    while(strcmp(userInput, "exit") != 0) {
        printf("[%s @ %s]$ ", navigation->username, navigation->root->name);
        fgets(userInput, MAX_INPUT_SIZE, stdin);
        userInput[strcspn(userInput, "\n")] = 0;

        int ret = tokenize_input(userInput);
        memset(userInput,0,strlen(userInput));
    }

    printf("Exiting programm");

    return 0;
}


int main() {
    Node* root = calloc(1, sizeof(Node));
    if (!root) return error_calloc();
    strcpy(root->name, "root");

    char username[MAX_SIZE] = {0};
    printf("Enter your name (%d characters max): ", MAX_SIZE);

    fgets(username, MAX_INPUT_SIZE, stdin);
    username[strcspn(username, "\n")] = 0;

    Navigation* navigation = calloc(1, sizeof(Navigation));
    if (!navigation) return error_calloc();
    strcpy(navigation->username, username);
    navigation->root = root;

    clear_screen();

    return vifima_loop(navigation);
}
