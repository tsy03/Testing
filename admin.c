#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <errno.h>
#include <arpa/inet.h> // For TCP Sockets

#define MENU_FILE "menu_db.txt"
#define ORDERS_FILE "orders_db.txt"
#define MSG_KEY 12345
#define PIPE_NAME "order_pipe"
#define CARD_FILE "card.txt"
#define PORT 8080 // TCP Server Port

sem_t file_sem; // Semaphore for synchronizing file access

// Message structure for message queue
struct message {
    long mtype;
    char mtext[100];
};

// Function declarations
void send_fifo_notification(const char *msg_text);
void admin_menu();
void view_menu();
void add_item_to_menu();
void edit_menu();
void view_and_manage_orders();
void update_order_status(const char *order_id, const char *status);
void *notification_handler(void *arg);
void setup_fifo();
void send_notification(const char *msg_text);
void view_orders(); // View orders function
void start_tcp_server(); // TCP server to handle customer requests
void *receive_fifo_notifications(void *arg); // Function to receive notifications from FIFO

int msgid; // Message Queue ID
int fifo_fd; // FIFO file descriptor for communication
int client_socket; // Client socket for communication

// Admin main function
int main() {
    printf("Starting admin program...\n");

    // Initialize semaphore
    sem_init(&file_sem, 0, 1);

    // Setup message queue
    msgid = msgget(MSG_KEY, 0666 | IPC_CREAT);
    if (msgid < 0) {
        perror("Message queue creation failed");
        exit(EXIT_FAILURE);
    }

    // Set up FIFO for receiving notifications about successful payments
    setup_fifo();

    // Start TCP server to listen for customer connections
    pthread_t tcp_thread;
    pthread_create(&tcp_thread, NULL, (void *)start_tcp_server, NULL);

    // Start receiving notifications from FIFO (order payment success) in a separate thread
    pthread_t fifo_thread;
    pthread_create(&fifo_thread, NULL, receive_fifo_notifications, NULL);

    // Admin menu
    admin_menu();

    // Wait for threads to finish
    pthread_join(tcp_thread, NULL);
    pthread_join(fifo_thread, NULL);

    return 0;
}

// Admin menu interface
void admin_menu() {
    int choice;
        printf("\n--- Admin Panel ---\n");
        printf("1. View Menu\n");
        printf("2. Add Item to Menu\n");
        printf("3. Edit Menu\n");
        printf("4. View Orders\n");
        printf("5. Manage Orders\n");
        printf("6. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);
        switch (choice) {
            case 1:
                view_menu();
                break;
            case 2:
                add_item_to_menu();
                break;
            case 3:
                edit_menu();
                break;
            case 4:
                view_orders();
                break;
            case 5:
                view_and_manage_orders();
                break;
            case 6:
                printf("Exiting Admin Panel...\n");
                exit(0);
        }
    
}

// View orders in the orders_db.txt
void view_orders() {
    FILE *orders_fp = fopen(ORDERS_FILE, "r");
    if (!orders_fp) {
        perror("Failed to open orders file");
        return;
    }
    char line[256];
    printf("\nOrders:\n");
    while (fgets(line, sizeof(line), orders_fp)) {
        printf("%s", line);
    }
    fclose(orders_fp);
    admin_menu();
}

// View and manage orders
void view_and_manage_orders() {
    char order_id[50];
    int status_choice;
    FILE *orders_fp = fopen(ORDERS_FILE, "r");
    if (!orders_fp) {
        perror("Failed to open orders file");
        return;
    }
    char line[256];
    printf("\nOrders with ID:\n");
    while (fgets(line, sizeof(line), orders_fp)) {
        printf("%s", line);
    }
    fclose(orders_fp);
    
    printf("Enter Order ID to update: ");
    fflush(stdout); // Ensure the prompt is displayed immediately
    scanf("%s", order_id);

    // Check if the order ID exists
    orders_fp = fopen(ORDERS_FILE, "r"); // Reuse the same variable
    if (!orders_fp) {
        perror("Failed to open orders file");
        return;
    }
    int order_found = 0;
    while (fgets(line, sizeof(line), orders_fp)) { // Reuse the same variable
        // Remove newline character from the line
        line[strcspn(line, "\n")] = '\0';
        // Compare the entire order ID
        if (strcmp(line, order_id) == 0) {
            order_found = 1;
            printf("\nOrder found:\n");
            printf("%s\n", line);
            while (fgets(line, sizeof(line), orders_fp) && strcmp(line, "\n") != 0) {
                printf("%s", line);
            }
            break;
        }
    }
    fclose(orders_fp);

    if (!order_found) {
        printf("Order ID not found. Please enter a valid Order ID.\n");
        return; // Return to the panel choice
    }
    
    printf("Enter new status (1 for Preparing, 2 for Completed): ");
    scanf("%d", &status_choice);
    fflush(stdin); // Ensure the prompt is displayed immediately
    printf("test1");
    // Clear the input buffer
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {}

    if (status_choice == 1) {
        update_order_status(order_id, "Preparing");
    } else if (status_choice == 2) {
        update_order_status(order_id, "Completed");
    } else {
        printf("Invalid status choice.\n");
    }
    admin_menu();
}

// Setup FIFO to receive notifications from customer
void setup_fifo() {
    // Check if FIFO exists and remove it
    if (access(PIPE_NAME, F_OK) != -1) {
        // FIFO exists, so remove it
        if (unlink(PIPE_NAME) != 0) {
            perror("Failed to remove existing FIFO");
            exit(EXIT_FAILURE);
        }
        printf("Removed existing FIFO.\n");
    }

    // Create FIFO
    if (mkfifo(PIPE_NAME, 0666) == -1) {
        perror("Failed to create FIFO");
        exit(EXIT_FAILURE);
    }
    printf("FIFO created successfully.\n");

    // Open FIFO for reading in non-blocking mode
    fifo_fd = open(PIPE_NAME, O_RDONLY | O_NONBLOCK);
    if (fifo_fd == -1) {
        perror("Failed to open FIFO");
        exit(EXIT_FAILURE);
    }
    printf("FIFO opened for reading.\n");
}

// Receive notifications from FIFO
void *receive_fifo_notifications(void *arg) {
    char buffer[256];
    while (1) {
        int bytes_read = read(fifo_fd, buffer, sizeof(buffer));
        if (bytes_read > 0) {
            buffer[bytes_read] = '\0'; // Null-terminate the string
            printf("\nAdmin Notification: %s\n", buffer);
            // Notify the customer via TCP
            if (client_socket > 0) {
                send(client_socket, "Order received", strlen("Order received"), 0);
            }
        } else {
            // Optionally, you can add a short sleep to prevent busy-waiting
            sleep(1);
        }
    }
    return NULL;
}

// Send notification to admin via message queue
void send_notification(const char *msg_text) {
    struct message msg;
    msg.mtype = 1; // Notification type
    strncpy(msg.mtext, msg_text, sizeof(msg.mtext)); // Fixed the use of `mtext`
    if (msgsnd(msgid, &msg, sizeof(msg.mtext), 0) == -1) {
        perror("Message send failed");
    }
}

// Update order status
void update_order_status(const char *order_id, const char *new_status) {
    sem_wait(&file_sem); // Lock file access
    FILE *orders_fp = fopen(ORDERS_FILE, "r");
    if (!orders_fp) {
        perror("Failed to open orders file");
        sem_post(&file_sem); // Unlock file access
        return;
    }

    // Create a temporary file to write the updated content
    FILE *temp_fp = fopen("temp_orders_db.txt", "w");
    if (!temp_fp) {
        perror("Failed to open temporary file");
        fclose(orders_fp);
        sem_post(&file_sem); // Unlock file access
        return;
    }

    char line[256];
    int order_found = 0;
    while (fgets(line, sizeof(line), orders_fp)) {
        // Remove newline character from the line
        line[strcspn(line, "\n")] = '\0';
        // Compare the entire order ID
        if (strcmp(line, order_id) == 0) {
            order_found = 1;
            fprintf(temp_fp, "%s\n", line); // Write the order ID
            while (fgets(line, sizeof(line), orders_fp)) {
                // Remove newline character from the line
                line[strcspn(line, "\n")] = '\0';
                if (strcmp(line, "Order received") == 0 || strcmp(line, "Preparing") == 0 || strcmp(line, "Completed") == 0) {
                    fprintf(temp_fp, "%s\n", new_status); // Write the new status
                } else {
                    fprintf(temp_fp, "%s\n", line); // Write the rest of the order details
                }
                if (strcmp(line, "") == 0) {
                    break; // End of the current order
                }
            }
        } else {
            fprintf(temp_fp, "%s\n", line); // Write the line as is
        }
    }

    fclose(orders_fp);
    fclose(temp_fp);

    if (!order_found) {
        printf("Order ID not found. Please enter a valid Order ID.\n");
        remove("temp_orders_db.txt"); // Remove the temporary file
    } else {
        // Replace the original file with the updated file
        remove(ORDERS_FILE);
        rename("temp_orders_db.txt", ORDERS_FILE);
        printf("Order status updated to %s.\n", new_status);
    }
    admin_menu();
    sem_post(&file_sem); // Unlock file access
}

// Send notification to customer via FIFO
void send_fifo_notification(const char *msg_text) {
    int fifo_fd = open(PIPE_NAME, O_WRONLY);
    if (fifo_fd == -1) {
        perror("Failed to open FIFO for writing");
        return;
    }
    write(fifo_fd, msg_text, strlen(msg_text) + 1);
    close(fifo_fd);
}

// Placeholder functions for undefined functions
void view_menu() {
    printf("View Menu functionality is not implemented yet.\n");
    admin_menu();
}

void add_item_to_menu() {
    printf("Add Item to Menu functionality is not implemented yet.\n");
    admin_menu();
}

void edit_menu() {
    printf("Edit Menu functionality is not implemented yet.\n");
    admin_menu();
}

// TCP Server to handle customer requests
void start_tcp_server() {
    int server_fd;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    char buffer[1024] = {0};
    char customer_name[100]; // Buffer to store customer name

    // Create socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket failed");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Bind socket
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (listen(server_fd, 3) < 0) {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }
    printf("\nServer listening on port %d\n", PORT);

    while (1) {
        printf("Waiting for a customer connection...\n");
        // Accept the incoming connection
        if ((client_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
            perror("Accept failed");
            continue; // Retry accepting
        }

        // Prompt customer to enter their name
        send(client_socket, "Enter your name: ", strlen("Enter your name: "), 0);
        int name_len = read(client_socket, customer_name, sizeof(customer_name));
        customer_name[name_len] = '\0'; // Null-terminate the string

        // Display customer's name
        printf("Customer '%s' connected from IP: %s, Port: %d\n", customer_name, inet_ntoa(address.sin_addr), ntohs(address.sin_port));

        // Show admin panel after customer name is received
        admin_menu();

        // Read customer message
        read(client_socket, buffer, sizeof(buffer));
        printf("Message from customer: %s\n", buffer);

        // Simulate order processing and send response back to customer
        send(client_socket, "Order received and processed", strlen("Order received and processed"), 0);
        printf("Response sent to customer: Order received and processed\n");

        // Keep the connection open to send order status updates manually
        while (1) {
            // Admin can manually update order status
            int status_choice;
            printf("Enter new status (1 for Preparing, 2 for Completed, 0 to exit): ");
            scanf("%d", &status_choice);

            if (status_choice == 0) {
                printf("Closing connection with customer.\n");
                close(client_socket);
                break;
            }

            char *status;
            if (status_choice == 1) {
                status = "Preparing";
            } else if (status_choice == 2) {
                status = "Completed";
            } else {
                printf("Invalid status choice.\n");
                continue;
            }

            send(client_socket, status, strlen(status), 0);
            printf("Order status update sent to customer: %s\n", status);
        }
    }
}