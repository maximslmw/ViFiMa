// TODO LIST
// - Create text file datatype
// - Enable writing and reading from text file
//      - Maybe a custom text editor for that?
// - Handle case for when user wants to create folder/file with already used name (use getNodeFromName for folders)
// - Create delete command
//      - Needs to recursively delete all subdirectories and files (with dfs? )

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define MAX_PARAMETER_LENGTH 20
#define MAX_INPUT_SIZE 100
#define MAX_COMMAND_SIZE 10
#define MAX_NAME_LENGTH 20

#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

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
    char name[MAX_PARAMETER_LENGTH];

    Node** items;
    size_t item_count;
    size_t item_capacity;

    Node** children;
    Node* parent;
    size_t ch_count;
    size_t ch_capacity;

};


typedef struct {
    char username[MAX_PARAMETER_LENGTH];
    Node* root;
    Node* current;
}Navigation;


typedef enum {
    mkdir,
    ls
}Commands;

//TODO: As of planned now, all commands have either 1 or 2 parameters as input.
//      This may change, so there needs to be a dynamic array of parameters.
typedef struct {
    char command[MAX_COMMAND_SIZE];
    char parameter1[MAX_PARAMETER_LENGTH];
    char parameter2[MAX_PARAMETER_LENGTH];
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
Node* getNodeFromName(Node* current, const char name[MAX_PARAMETER_LENGTH]) {
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
int make_directory(Node* current, const char name[MAX_PARAMETER_LENGTH]) {
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
* Lists all contents of current node; includes the subdirectories and files in that node
*/
void list_directory(Node* current) {
    printf("\n");
    size_t i = 0;
    for (i; i < current->ch_count; i++) {
        Node* child = current->children[i];
        printf("   " ANSI_COLOR_GREEN "%s" ANSI_COLOR_RESET "\n", child->name);
    }

    if (i == 0) printf("No content in this directory to list");
    printf("\n");
}


/**
* Given a user input as string, chunks the string into its individual tokens
* and stores them in a struct of type Tokenized
*/
Tokenized* tokenize_input(char userInput[MAX_INPUT_SIZE]) {
    Tokenized* tokenized = calloc(1, sizeof(Tokenized));
    if (!tokenized) return NULL;

    char* token;
    char* whiteSpace = " \t\n\f\r\v";
    token = strtok(userInput, whiteSpace);

    int isCommand = 1;
    int p_index = 0;

    while (token != NULL) {
        if(isCommand) {
            isCommand = 0;
            strcpy(tokenized->command, token);
        }

        else {
            if (p_index < 1) strcpy(tokenized->parameter1, token);
            else strcpy(tokenized->parameter2, token);
            p_index++;

        }

        token = strtok(NULL, whiteSpace);
    }

    return tokenized;
}


/**
* Given the current node in navigation and a name of a goal node;
* tries to change the current node of the navigation to the goal node
*/
void change_directory(Navigation* navigation, char directory[MAX_PARAMETER_LENGTH]) {
    Node* next_node = getNodeFromName(navigation->current, directory);
    if (next_node == NULL) return;

    navigation->current = next_node;
}


void change_to_parent(Navigation* navigation) {
    navigation->current = navigation->current->parent;
}


void execute_command(Navigation* navigation, Tokenized* tokenized) {
    char command[MAX_COMMAND_SIZE];
    strcpy(command, tokenized->command);

    // Make Directory
    if (strcmp(command, "mkdir") == 0) {
        make_directory(navigation->current, tokenized->parameter1);
    }

    // List
    else if (strcmp(command, "ls") == 0) {
        list_directory(navigation->current);
    }

    // Change Directory
    else if ((strcmp(command, "cd") == 0)) {
        change_directory(navigation, tokenized->parameter1);
    }

    // Go back
    else if ((strcmp(command, "gb") == 0)) {
        change_to_parent(navigation);
    }

    // Clear
    else if ((strcmp(command, "clear") == 0)) {
        clear_screen();
    }
}


int vifima_loop(Navigation* navigation) {
    char userInput[MAX_INPUT_SIZE] = {0};
    printf("[%s @ %s]$ ", navigation->username, navigation->current->name);
    fgets(userInput, MAX_INPUT_SIZE, stdin);
    userInput[strcspn(userInput, "\n")] = 0;

    Tokenized* tokenized = tokenize_input(userInput);
    memset(userInput,0,strlen(userInput));
    execute_command(navigation, tokenized);

    while(strcmp(userInput, "exit") != 0) {
        printf("[%s @ %s]$ ", navigation->username, navigation->current->name);
        fgets(userInput, MAX_INPUT_SIZE, stdin);
        userInput[strcspn(userInput, "\n")] = 0;

        tokenized = tokenize_input(userInput);
        execute_command(navigation, tokenized);
        memset(userInput,0,strlen(userInput));
    }

    printf("Exiting programm");

    return 0;
}

void setup_dev_navigation(Navigation* navigation) {
    make_directory(navigation->current, "Documents");
    make_directory(navigation->current, "Desktop");
    make_directory(navigation->current, "Downloads");
}

int main() {
    int dev_flag = 1; // Set this to 1 to start program with premade navigation graph

    Node* root = calloc(1, sizeof(Node));

    if (!root) return error_calloc();
    strcpy(root->name, "root");

    char username[MAX_PARAMETER_LENGTH] = {0};
    printf("Enter your name (%d characters max): ", MAX_PARAMETER_LENGTH);

    fgets(username, MAX_INPUT_SIZE, stdin);
    username[strcspn(username, "\n")] = 0;

    Navigation* navigation = calloc(1, sizeof(Navigation));
    if (!navigation) return error_calloc();
    strcpy(navigation->username, username);
    navigation->root = root;
    navigation->current = root;

    if (dev_flag == 1) {
        setup_dev_navigation(navigation);
    }

    clear_screen();

    return vifima_loop(navigation);
}
