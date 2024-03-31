#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<limits.h>
typedef struct Node{
    char* letter;
    struct Node* next;
    int weight;
}Node;

typedef struct Stack{
    Node* headofStack;
    Node* tail;
} Stack;

void freeList(Node* head){
    Node* ptr= head;
    while(ptr){
        free(ptr->letter);
        Node* next = ptr->next;
        free(ptr);
        ptr = next;
    }
}
Node* allocateNode(char* info, int weight){
    Node* temp = malloc(sizeof(Node));
    temp->letter = info;
    temp->next = NULL;
    temp->weight = weight;
    return temp;
}


Node* findNode(Node** graph, int num, char* letter){
    for(int i = 0; i < num; i++){
        if(strcmp(graph[i]->letter, letter)==0){
            Node* returnvalue = graph[i];
            return returnvalue;
        }
    }
    return NULL;
}

void addNode(Node** graph, int num, char* vertex1, char* vertex2, int weight){
    Node* curr= findNode(graph, num, vertex1);
   
    if(curr->next==NULL){
        curr->next= allocateNode(vertex2, weight);
    }
    else{
        Node* pointer= curr->next;
        Node* prev= curr;
        while(pointer){
            if(strcmp(vertex2,pointer->letter)<0){
                Node* new= allocateNode(vertex2, weight);
                new->next=pointer;
                prev->next=new;
                break;
            }
            if(pointer->next==NULL && strcmp (vertex2, pointer->letter) > 0){
                pointer->next=allocateNode(vertex2, weight);
                break;
            }
            if(strcmp(vertex2,pointer->letter)>0){
                prev=pointer;
                pointer=pointer->next;
                continue;
            }
            if(vertex2 == pointer->letter){
                break;
            }
        }
    }
}
int isEmpty(Stack* Stack){
    if(Stack->headofStack==NULL||Stack->tail==NULL){
        return 1;
    }
    else return 0;
}
Stack* allocatestack(){
    Stack* temp= malloc(sizeof(Stack));
    temp->headofStack=NULL;
    temp->tail=NULL;
    return temp;
}

void push(Stack* Stack, char* letter){
    char* copy=strdup(letter);
    int weight=0;
    Node* new= allocateNode(copy, weight);
    if(isEmpty(Stack)==1){
        Stack->headofStack= new;
        Stack->tail=new;
    }else{
        Stack->tail->next=new;
        Stack->tail=Stack->tail->next;
    }
}
char* pop(Stack* Stack){
    if(isEmpty(Stack)==1){
        return NULL;
    }
    char* letter=strdup(Stack->tail->letter);
    Node* temp = Stack->tail;
    Node* ptr = Stack->headofStack;
    while(ptr != NULL){
        if(ptr->next==temp){
            ptr->next=NULL;
            break;
        }
        ptr = ptr->next;
    }
    free(temp->letter);
    free(temp);
    Stack->tail=ptr;
    return letter;
}
int isInside(Node* headofStack, char* letter){
    Node* ptr=headofStack;
    while(ptr != NULL){
        if(strcmp(ptr->letter,letter)==0){
            return 1;
        }
        ptr=ptr->next;
    }
    return 0;
}
int weight (char* letter, Node* ptr1, Node** graph, int num){
    Node* pointer=findNode(graph, num, letter);

    while(pointer){
        if(strcmp(pointer->letter, ptr1->letter)==0){
            return pointer->weight;
        }
        pointer=pointer->next;
    }
    return INT_MAX;

}
void depthFirstSearch(Node** graph,Node* curr, Stack* topological, Stack* visited, int number){
    push(visited, curr->letter);
    Node* ptr= findNode(graph, number, curr->letter)->next;
    while(ptr!=NULL){
        if(isInside(visited->headofStack, ptr->letter)==0){
            depthFirstSearch(graph, ptr, topological, visited, number);
        }
        ptr=ptr->next;
    }
    push(topological, curr->letter);
   
}
void checkForCycles(Node **graph, int numofVertices, FILE *fP, FILE *queryFile) {
    for (int i = 0; i < numofVertices; i++) {
        Node *ptr = graph[i]->next;
        while (ptr != NULL) {
            Node *reverse = findNode(graph, numofVertices, ptr->letter);
            if (isInside(graph[i], ptr->letter) == 1 && isInside(reverse, graph[i]->letter) == 1) {
                printf("CYCLE\n");
                for (int j = 0; j < numofVertices; j++) {
                    freeList(graph[j]);
                }
                free(graph);
                fclose(fP);
                fclose(queryFile);
                exit(EXIT_SUCCESS);
            }
            ptr = ptr->next;
        }
    }
}
int findIndexofNode(Node* ptr, Node** graph, int num){
    for(int i = 0; i < num; i++){
        if(strcmp(graph[i]->letter, ptr->letter) == 0){
            return i;
        }
    }
    return-1;
}
void performDFSIfNotVisited(Node **graph, int numofVertices, Stack *visited, Stack *topological) {
    for (int i = 0; i < numofVertices; i++) {
        if (isInside(visited->headofStack, graph[i]->letter) == 0) {
            depthFirstSearch(graph, graph[i], topological, visited, numofVertices);
        }
    }
}
int main(int argc, char **argv){
    if(argc < 3){
        printf("enter the input arguments\n");
        exit(EXIT_SUCCESS);
    }
    FILE* fP = fopen(argv[1], "r"), * queryFile = fopen(argv[2], "r");
    
   
    if(fP == NULL ||queryFile == NULL){
        printf("unable to open input file \n");
        exit(EXIT_SUCCESS);
    }
   
    int numofVertices;
    fscanf(fP,"%d\n", &numofVertices);
    char* letter= malloc(80*sizeof(char));
    Node** graph= malloc(numofVertices* sizeof(Node*));
    fscanf(fP,"%s\n", letter);
    Node* chars = allocateNode(letter, 0);

    for(int i = 0; i < numofVertices-1; i++){
        char* words= malloc(80*sizeof(char));
        fscanf(fP,"%s\n", words);

        if (strcmp(words, chars->letter ) < 0){
            Node* newnode = allocateNode(words, 0);
            newnode->next = chars; 
            chars=newnode;
        }
        if (strcmp(words, chars->letter )>0){
            Node* prevPtr;
            Node* ptr= chars;
            while(ptr){
                if (strcmp(words, ptr->letter) < 0){
                    Node* newnode= allocateNode(words,0);
                    prevPtr->next=newnode;
                    newnode->next=ptr;
                    break;
                }
                if(ptr->next==NULL&&strcmp(words, ptr->letter)>0){
                    ptr->next= allocateNode(words, 0);
                    break;
                }
                if(strcmp(words, ptr->letter) > 0){
                    prevPtr = ptr;
                    ptr = ptr->next;
                    continue;
                }
            }
        }
    }
    Node* ptr=chars;
    
    int count = 0;
    while(ptr != NULL){
        char* Duplicate= strdup(ptr->letter);
        graph[count]= allocateNode(Duplicate, 0);
        count++;
        ptr=ptr->next;
    }
    
    freeList(chars);

    while(!feof(fP)) {
        char* vertex1 = malloc(10*sizeof(char));
        char* vertex2 = malloc(10*sizeof(char));
        int weight;
        fscanf(fP,"%s %s %d\n", vertex1, vertex2, &weight);
        addNode(graph, numofVertices, vertex1, vertex2, weight);
        free(vertex1);
    }
   checkForCycles(graph, numofVertices, fP, queryFile);
    while(!feof(queryFile)){
        printf("\n");
        char* vertex1= malloc(80*sizeof(char));
        fscanf(queryFile,"%s\n", vertex1);
        Node* ptr = findNode(graph, numofVertices, vertex1);
        Stack* topological= allocatestack();
        Stack* visited= allocatestack();
        depthFirstSearch(graph, ptr, topological, visited, numofVertices);
       
        performDFSIfNotVisited(graph, numofVertices, visited, topological);

        int* distance= malloc(numofVertices* sizeof(int));
        for(int i = 0; i < numofVertices; i++){
            distance[i]=INT_MAX;
        }
        for(int i = 0; i < numofVertices;i++){
            if(strcmp(graph[i]->letter, vertex1)==0){
                distance[i]=0;
                break;
            }
        }
        char* Popped = pop(topological);
        ptr = findNode(graph, numofVertices, Popped);
        free(Popped);
        while(ptr){
            int i= findIndexofNode(ptr, graph, numofVertices);
            if(distance[i]==INT_MAX){
                char* Popped= pop(topological);
                ptr=findNode(graph, numofVertices, Popped);
                free(Popped);
                continue;
            }
            Node* ptr1 = findNode(graph, numofVertices, ptr->letter);
            while(ptr1 != NULL){
                int j= findIndexofNode(ptr1, graph, numofVertices);
                if(distance[j] > (distance[i] + weight(ptr->letter, ptr1, graph, numofVertices))){
                    int sum = distance[i] + weight(ptr->letter, ptr1, graph, numofVertices);
                    distance[j] = sum;
                }
                ptr1 = ptr1->next;
            }
            char* Popped = pop(topological);
            if(Popped == NULL){
                break;
            }
            ptr = findNode(graph, numofVertices, Popped);
            free(Popped);
        }
        for(int i=0; i < numofVertices; i++){
            if(distance[i]==INT_MAX){
                printf("%s %s\n", graph[i]->letter, "INF"); //print INF for unreachable vertices
            }
            else{
                printf("%s %d\n", graph[i]->letter, distance[i]);
            }  
        }
        freeList(visited->headofStack);
        free(visited);
        free(vertex1);
        free(distance);
        free(topological);
       
    }
    for(int i = 0; i < numofVertices;i++){
        freeList(graph[i]);
    }
    free(graph);
    fclose(fP);
    fclose(queryFile);
    return EXIT_SUCCESS;
}


