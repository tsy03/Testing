#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <fcntl.h>
#include <signal.h>
#include <arpa/inet.h> // For TCP Sockets
#include <pthread.h> // For threading
#include <sys/select.h> // For select()
#include <time.h> // For generating unique order ID

#define MENU_FILE "menu_db.txt"
#define ORDERS_FILE "orders_db.txt"
#define CART_FILE "cart_db.txt"
#define CARD_FILE "card.txt"
#define MSG_KEY 12345
#define MAX_BUFFER 256
#define SERVER_IP "127.0.0.1" // Admin server IP
#define PORT 8080 // TCP Server Port

struct message {
    long mtype;
    char mtext[MAX_BUFFER]; // Fixed to use mtext
};

// FIFO file descriptor
int fifo_fd; // FIFO for customer notifications
int sock = 0; // TCP socket

// Function declarations
void send_fifo_notification(const char *msg_text);
int process_payment(float total_amount);
void handle_payment();
void signal_handler(int sig);
void send_notification(const char *msg_text);
void view_menu(); // Function to display menu to customer
void add_to_cart(); // Function to add items to cart
void display_cart(); // Function to display current cart
void *server_communication_handler(void *arg); // Thread function for server communication
void clear_cart(); // Function to clear the cart after successful payment
float calculate_total_amount(); // Function to calculate total amount
void delete_from_cart(); // Function to delete an item from the cart
void save_order(const char *order_details); // Function to save order details
char* generate_order_id(); // Function to generate a unique order ID

char customer_name[100]; // Buffer to store customer name

// Main function to connect to the TCP server (admin)
int main() {
    struct sockaddr_in serv_addr;
    char buffer[1024] = {0};
    fd_set readfds;

    // Create socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket creation failed");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // Convert IP address from text to binary form
    if (inet_pton(AF_INET, SERVER_IP, &serv_addr.sin_addr) <= 0) {
        perror("Invalid address or address not supported");
        return -1;
    }

    // Connect to the admin server
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("Connection failed");
        return -1;
    }
    printf("Connected to server\n");

    // Receive prompt for name and send name
    read(sock, buffer, sizeof(buffer));
    printf("\nMessage from server: %s", buffer);
    scanf("%s", customer_name);
    send(sock, customer_name, strlen(customer_name), 0);

    // Start server communication handler thread
    pthread_t server_thread;
    pthread_create(&server_thread, NULL, server_communication_handler, NULL);

    while (1) {
        FD_ZERO(&readfds);
        FD_SET(0, &readfds); // stdin for customer input

        // Wait for an activity on stdin
        if (select(1, &readfds, NULL, NULL, NULL) < 0) {
            perror("Select error");
            exit(1);
        }

        if (FD_ISSET(0, &readfds)) {
            // Read from stdin (user input)
            char choice;
            printf("\n--- Customer Panel ---\n");
            printf("1. View Menu\n");
            printf("2. Add Item to Cart\n");
            printf("3. View Cart\n");
            printf("4. Make Payment\n");
            printf("5. Exit\n");
            printf("Enter your choice: ");
            scanf(" %c", &choice);
            switch(choice) {
                case '1':
                    view_menu();
                    break;
                case '2':
                    add_to_cart();
                    break;
                case '3':
                    while (1) {
                        display_cart();
                        printf("\n1. Go Back\n");
                        printf("2. Proceed to Payment\n");
                        printf("3. Add More Items\n");
                        printf("4. Delete Item from Cart\n");
                        printf("Enter your choice: ");
                        char sub_choice;
                        scanf(" %c", &sub_choice);
                        if (sub_choice == '1') {
                            break;
                        } else if (sub_choice == '2') {
                            handle_payment();
                            break;
                        } else if (sub_choice == '3') {
                            add_to_cart();
                        } else if (sub_choice == '4') {
                            delete_from_cart();
                        } else {
                            printf("Invalid option. Please try again.\n");
                        }
                    }
                    break;
                case '4':
                    handle_payment();
                    break;
                case '5':
                    printf("Exiting Customer Panel...\n");
                    close(sock);
                    exit(0);
                default:
                    printf("Invalid option. Please try again.\n");
            }
        }
    }

    // Wait for server thread to finish
    pthread_join(server_thread, NULL);

    return 0;
}

// Function to simulate card payment
int process_payment(float total_amount) {
    FILE *card_fp = fopen(CARD_FILE, "r");
    if (!card_fp) {
        perror("Failed to open card file");
        return 0;
    }
    char entered_card_number[20];
    printf("Total amount: RM%.2f\n", total_amount);
    printf("Enter card number: ");
    scanf("%s", entered_card_number);

    char card_number[20];
    int payment_successful = 0;
    while (fgets(card_number, sizeof(card_number), card_fp)) {
        // Remove newline character from card_number
        card_number[strcspn(card_number, "\n")] = '\0';
        if (strcmp(entered_card_number, card_number) == 0) {
            payment_successful = 1;
            break;
        }
    }
    fclose(card_fp);

    if (payment_successful) {
        printf("\nPayment successful with card number: %s\n", entered_card_number);
        return 1; // Simulate successful payment
    } else {
        printf("\nPayment failed. Invalid card number.\n");
        return 0; // Simulate failed payment
    }
}

// Function to handle payment and order processing
void handle_payment() {
    float total_amount = calculate_total_amount();
    if (process_payment(total_amount)) {
        char order_details[MAX_BUFFER];
        char order_id[20];
        strcpy(order_id, generate_order_id());
        snprintf(order_details, sizeof(order_details), "%s\nCustomer Name: %s\nTotal Price:RM%.2f\nOrder received\n", order_id, customer_name, total_amount);

        // Append items to order details
        FILE *cart_fp = fopen(CART_FILE, "r");
        if (!cart_fp) {
            perror("Failed to open cart file");
            return;
        }
        char line[MAX_BUFFER];
        while (fgets(line, sizeof(line), cart_fp)) {
            strcat(order_details, line);
            strcat(order_details, "");
        }
        fclose(cart_fp);

        save_order(order_details);
        send_fifo_notification("Order received");
        send(sock, "Payment completed", strlen("Payment completed"), 0); // Notify admin via TCP
        clear_cart(); // Clear the cart after successful payment
    } else {
        printf("Payment failed. Please try again.\n");
    }
}

// Function to save order details
void save_order(const char *order_details) {
    FILE *orders_fp = fopen(ORDERS_FILE, "a");
    if (!orders_fp) {
        perror("Failed to open orders file");
        return;
    }
    fprintf(orders_fp, "%s\n", order_details);
    fclose(orders_fp);
}

// Function to generate a unique order ID
char* generate_order_id() {
    static char order_id[20];
    srand(time(NULL));
    snprintf(order_id, sizeof(order_id), "%d", rand());
    return order_id;
}

// Function to send notification to customer via FIFO
void send_fifo_notification(const char *msg_text) {
    fifo_fd = open("order_pipe", O_WRONLY);
    if (fifo_fd == -1) {
        perror("Failed to open FIFO for writing");
        return;
    }
    write(fifo_fd, msg_text, strlen(msg_text) + 1);
    close(fifo_fd);
}

// Signal handler for graceful exit
void signal_handler(int sig) {
    printf("\nExiting Customer Panel...\n");
    exit(0);
}

// Send notification to customer via message queue
void send_notification(const char *msg_text) {
    struct message msg;
    msg.mtype = 1; // Notification type
    strncpy(msg.mtext, msg_text, sizeof(msg.mtext)); // Fixed the use of `mtext`
    int msgid = msgget(MSG_KEY, 0666);
    if (msgid == -1) {
        perror("Failed to access message queue");
        return;
    }
    if (msgsnd(msgid, &msg, sizeof(msg.mtext), 0) == -1) {
        perror("Message send failed");
    }
}

// Function to view the menu for customer
void view_menu() {
    FILE *menu_fp = fopen(MENU_FILE, "r");
    if (!menu_fp) {
        perror("Failed to open menu file");
        return;
    }
    char line[MAX_BUFFER];
    printf("\n--- Menu ---\n");
    while (fgets(line, sizeof(line), menu_fp)) {
        printf("%s", line);
    }
    fclose(menu_fp);
}

// Function to add items to the cart
void add_to_cart() {
    FILE *menu_fp = fopen(MENU_FILE, "r");
    FILE *cart_fp = fopen(CART_FILE, "a");
    if (!menu_fp || !cart_fp) {
        perror("Failed to open menu or cart file");
        return;
    }
    int item_number, quantity;
    char line[MAX_BUFFER];
    printf("Enter item number to add to cart: ");
    scanf("%d", &item_number);
    printf("Enter quantity: ");
    scanf("%d", &quantity);

    // Find the item in the menu
    while (fgets(line, sizeof(line), menu_fp)) {
        int number;
        char name[100];
        float price;
        sscanf(line, "%d: %[^R] RM%f", &number, name, &price);
        if (number == item_number) {
            for (int i = 0; i < quantity; i++) {
                fprintf(cart_fp, "%s,%.2f\n", name, price);
            }
            printf("Added %d x %s to cart.\n", quantity, name);
            break;
        }
    }

    fclose(menu_fp);
    fclose(cart_fp);
}

// Function to display the current cart
void display_cart() {
    FILE *cart_fp = fopen(CART_FILE, "r");
    if (!cart_fp) {
        perror("Failed to open cart file");
        return;
    }
    char line[MAX_BUFFER];
    printf("\n--- Your Cart ---\n");
    while (fgets(line, sizeof(line), cart_fp)) {
        printf("%s", line);
    }
    fclose(cart_fp);
}

// Function to clear the cart after successful payment
void clear_cart() {
    FILE *cart_fp = fopen(CART_FILE, "w");
    if (!cart_fp) {
        perror("Failed to open cart file");
        return;
    }
    fclose(cart_fp);
    printf("Cart cleared.\n");
}

// Function to calculate total amount
float calculate_total_amount() {
    FILE *cart_fp = fopen(CART_FILE, "r");
    if (!cart_fp) {
        perror("Failed to open cart file");
        return 0.0;
    }
    char line[MAX_BUFFER];
    float total_amount = 0.0;
    while (fgets(line, sizeof(line), cart_fp)) {
        char name[100];
        float price;
        sscanf(line, "%[^,],%f", name, &price);
        total_amount += price;
    }
    fclose(cart_fp);
    return total_amount;
}

// Function to delete an item from the cart
void delete_from_cart() {
    char item_name[100];
    printf("Enter the name of the item to delete from cart: ");
    scanf("%s", item_name);

    FILE *cart_fp = fopen(CART_FILE, "r");
    if (!cart_fp) {
        perror("Failed to open cart file");
        return;
    }

    FILE *temp_fp = fopen("temp_cart_db.txt", "w");
    if (!temp_fp) {
        perror("Failed to open temporary cart file");
        fclose(cart_fp);
        return;
    }

    char line[MAX_BUFFER];
    int found = 0;
    while (fgets(line, sizeof(line), cart_fp)) {
        if (strstr(line, item_name) == NULL) {
            fputs(line, temp_fp);
        } else {
            found = 1;
        }
    }

    fclose(cart_fp);
    fclose(temp_fp);

    remove(CART_FILE);
    rename("temp_cart_db.txt", CART_FILE);

    if (found) {
        printf("Deleted %s from cart.\n", item_name);
    } else {
        printf("Item not found in cart.\n");
    }
}

// Thread function for server communication
void *server_communication_handler(void *arg) {
    char buffer[1024];
    while (1) {
        int bytes_read = read(sock, buffer, sizeof(buffer));
        if (bytes_read > 0) {
            buffer[bytes_read] = '\0'; // Null-terminate the string
            printf("\nMessage from server: %s\n", buffer);
        } else if (bytes_read == 0) {
            // Server closed the connection
            printf("\nServer disconnected. Exiting...\n");
            close(sock);
            exit(0);
        } else {
            // Optionally, you can add a short sleep to prevent busy-waiting
            sleep(1);
        }
    }
    return NULL;
}