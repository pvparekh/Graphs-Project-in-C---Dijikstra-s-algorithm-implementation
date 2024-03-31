#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

typedef struct Node {
    char* letter;
    struct Node* next;
    int weight;
} Node;

typedef struct {
    Node* front;
    Node* rear;
} Queue;

Node* findNode(Node** graph, int numVertices, const char* letter) {
    for (int i = 0; i < numVertices; i++) {
        if (strcmp(graph[i]->letter, letter) == 0) {
            return graph[i];
        }
    }
    return NULL;
}
void freeList(Node* head) {
    while (head != NULL) {
        Node* temp = head;
        head = head->next;
        free(temp->letter);
        free(temp);
    }
}

int getDistance(Node* source, const char* dest) {
    Node* current = source;
    while (current != NULL) {
        if (strcmp(current->letter, dest) == 0) {
            return current->weight;
        }
        current = current->next;
    }
    return INT_MAX;
}
Node* createNode(const char* info, int weight) {
    Node* newNode = malloc(sizeof(Node));
    newNode->letter = strdup(info); // Copy to ensure the string is owned by the Node
    newNode->weight = weight;
    newNode->next = NULL;
    return newNode;
}
Node* allocateNode(char* info, int weight){
    Node* temp = malloc(sizeof(Node));
    temp->letter = info;
    temp->next = NULL;
    temp->weight = weight;
    return temp;
}
void addNode(Node** graph, int num, char* char1, char* char2, int weight) {
    Node* srcNode = findNode(graph, num, char1);
    if (!srcNode) return; // Source Node not found, error handling could be added here
    Node* newNode = allocateNode(strdup(char2), weight); // Ensure new Node has its own copy of char2
    newNode->next = srcNode->next; // Insert new Node at the beginning of the adjacency list for simplicity
    srcNode->next = newNode;
}

void enqueue(Queue* queue, const char* letter) {
    Node* newNode = createNode(letter, 0);
    if (queue->rear == NULL) {
        queue->front = queue->rear = newNode;
        return;
    }
    queue->rear->next = newNode;
    queue->rear = newNode;
}

char* dequeue(Queue* queue) {
    if (queue->front == NULL) return NULL;
    Node* temp = queue->front;
    char* letter = strdup(temp->letter); // Copy the letter for return
    queue->front = queue->front->next;
    if (queue->front == NULL) queue->rear = NULL;
    free(temp->letter);
    free(temp);
    return letter;
}

int isQueueEmpty(Queue* queue) {
    return queue->front == NULL;
}

int findIndex(Node** graph, int numVertices, const char* letter) {
    for (int i = 0; i < numVertices; i++) {
        if (strcmp(graph[i]->letter, letter) == 0) {
            return i;
        }
    }
    return -1;
}
Queue* createQueue() {
    Queue* newQueue = malloc(sizeof(Queue));
    newQueue->front = newQueue->rear = NULL;
    return newQueue;
}

void freeQueue(Queue* queue) {
    while (!isQueueEmpty(queue)) {
        char* letter = dequeue(queue); // Use dequeue to free nodes correctly
        free(letter); // Free the letter copied by dequeue
    }
    free(queue);
}
void addEdge(Node** graph, int numVertices, const char* src, const char* dest, int weight) {
    for (int i = 0; i < numVertices; i++) {
        if (strcmp(graph[i]->letter, src) == 0) {
            Node* current = graph[i];
            while (current->next != NULL && strcmp(current->next->letter, dest) < 0) {
                current = current->next;
            }
            Node* newNode = createNode(dest, weight);
            newNode->next = current->next;
            current->next = newNode;
            break;
        }
    }
}
void dijkstra(Node** graph, int numVertices, const char* source) {
    int distances[100];
    int visited[100];
    memset(visited, 0, sizeof(visited));
    for (int i = 0; i < numVertices; i++) {
        distances[i] = INT_MAX;
    }
    int sourceIndex = findIndex(graph, numVertices, source);
    if (sourceIndex == -1) {
        printf("Source vertex %s not found in the graph.\n", source);
        return;
    }
    distances[sourceIndex] = 0;
    for (int i = 0; i < numVertices - 1; i++) {
        int minDist = INT_MAX, u = -1;
        for (int v = 0; v < numVertices; v++) {
            if (!visited[v] && distances[v] < minDist) {
                minDist = distances[v], u = v;
            }
        }
        if (u == -1) break; // No more reachable vertices
        visited[u] = 1;
        Node* adjList = graph[u]->next;
        while (adjList) {
            int adjIndex = findIndex(graph, numVertices, adjList->letter);
            if (!visited[adjIndex] && distances[u] != INT_MAX && distances[u] + adjList->weight < distances[adjIndex]) {
                distances[adjIndex] = distances[u] + adjList->weight;
            }
            adjList = adjList->next;
        }
    }
    // Output the results
    for (int i = 0; i < numVertices; i++) {
        if (distances[i] == INT_MAX) {
            printf("%s INF\n", graph[i]->letter);
        } else {
            printf("%s %d\n", graph[i]->letter, distances[i]);
        }
    }
}



void processFile(FILE* fp, Node*** graph, int* numVertices) {
    if (fscanf(fp, "%d", numVertices) != 1) {
        fprintf(stderr, "Error reading number of vertices.\n");
        exit(EXIT_FAILURE);
    }

    *graph = malloc(sizeof(Node*) * (*numVertices));
    if (*graph == NULL) {
        fprintf(stderr, "Memory allocation failed.\n");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < *numVertices; i++) {
        char vertex[100]; // Using stack allocation to avoid manual free
        if (fscanf(fp, "%s", vertex) != 1) {
            fprintf(stderr, "Error reading vertex.\n");
            exit(EXIT_FAILURE);
        }
        (*graph)[i] = createNode(vertex, 0);
    }

    // Reading edges and weights with checks
    char src[100], dest[100];
    int weight;
    while (fscanf(fp, "%s %s %d", src, dest, &weight) == 3) {
        addEdge(*graph, *numVertices, src, dest, weight);
    }
}

int main(int argc, char** argv) {
    if (argc != 3) {
        printf("Usage: %s <graph file> <query file>\n", argv[0]);
        return 1;
    }

    FILE* graphFile = fopen(argv[1], "r"), *queryFile = fopen(argv[2], "r");
    if (!graphFile || !queryFile) {
        perror("Error opening files");
        return 1;
    }

   int numVertices;
    Node** graph = NULL;
    processFile(graphFile, &graph, &numVertices);

    fclose(graphFile); // Close the graph file as it's no longer needed after reading

    // Check if the graph is empty
    if (numVertices == 0) {
        fprintf(stderr, "The graph is empty.\n");
        fclose(queryFile); // Make sure to close the query file before exiting
        return EXIT_FAILURE;
    }
    // Process queries from the query file
    char query[100];
    while (fscanf(queryFile, "%s", query) == 1) {
        if (findIndex(graph, numVertices, query) == -1) {
            printf("Source vertex %s not found in the graph.\n", query);
        } else {
            dijkstra(graph,numVertices, query);
            printf("\n");
        }
    }

    fclose(queryFile); // Close the query file after processing all queries

    // Clean up the graph data structure
    for (int i = 0; i < numVertices; i++) {
        freeList(graph[i]);
    }
    free(graph); // Free the array of vertex pointers

    return 0;
}