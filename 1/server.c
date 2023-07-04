#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/time.h>

#define NOT_ANSWERED 10
#define ANSWERED 11
#define IN_TALK 12
#define WAIT_FOR_STUDENT 13
#define EMPTY -1

int setupServer(int port) {
    struct sockaddr_in address;
    int server_fd;
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);
    bind(server_fd, (struct sockaddr *)&address, sizeof(address));
    listen(server_fd, 4);
    return server_fd;
}

int acceptClient(int server_fd) {
    int client_fd;
    struct sockaddr_in client_address;
    int address_len = sizeof(client_address);
    client_fd = accept(server_fd, (struct sockaddr *)&client_address, (socklen_t*) &address_len);
    return client_fd;
}



int main(int argc, char const *argv[]) {

    int req_counter = 0, client_counter = 0;
    char req_titles[15][1024];
    int requests_list[] = {EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY};
    int clients_sockets[] = {EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY};
    int TA_ID[] = {EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY};
    int Student_ID[] = {EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY};
    
    int server_fd, new_socket, max_sd;
    char buffer[1024] = {0};
    fd_set master_set, working_set;
    int ServerPort = atoi(argv[1]);
    server_fd = setupServer(ServerPort);
    FD_ZERO(&master_set);
    max_sd = server_fd;
    FD_SET(server_fd, &master_set);

    write(1, "Server is running\n", 18);

    int check_empty_DB = 0;
    //int file = open("database.txt", O_CREAT | O_RDWR);
    //close(file);
    //printf("lol%d\n",file);

    while (1) {
        working_set = master_set;
        select(max_sd + 1, &working_set, NULL, NULL, NULL);

        for (int i = 0; i <= max_sd; i++) {
            if (FD_ISSET(i, &working_set)) {
                
                if (i == server_fd) {  // new clinet
                    new_socket = acceptClient(server_fd);
                    FD_SET(new_socket, &master_set);
                    if (new_socket > max_sd)
                        max_sd = new_socket;
                    clients_sockets[client_counter] = new_socket;
                    client_counter = client_counter + 1;
                    printf("New client connected. fd = %d\n", new_socket);
                }
                
                else { // client sending msg
                    int bytes_received;
                    memset(buffer, 0, 1024);
                    bytes_received = recv(i , buffer, 1024, 0);
                    
                    if (bytes_received == 0) { // EOF
                        printf("client fd = %d closed\n", i);
                        close(i);
                        FD_CLR(i, &master_set);
                        continue;
                    }

                    printf("client %d: %s\n", i, buffer);

                    if (buffer[0] == '+'){  //student adding request
                        for (int j = 1; j < strlen(buffer); j++){
                            req_titles[req_counter][j - 1] = buffer[j];
                        }
                        requests_list[req_counter] = NOT_ANSWERED;
                        Student_ID[req_counter] = i;
                        memset(buffer, 0, 1024);
                        strcpy(buffer, "Request registerd\n");
                        send(clients_sockets[i - 4], buffer, 1024, 0);
                        memset(buffer, 0, 1024);
                        req_counter = req_counter + 1;
                        continue;
                    }

                    if (strcmp(buffer, "CheckMyReqs\n") == 0){     // student checking if exist a request in talk
                        memset(buffer, 0, 1024);
                        int check = 0;
                        for (int j = 0; j < req_counter; j ++){
                            if (Student_ID[j] == i){
                                if (requests_list[j] == WAIT_FOR_STUDENT){
                                    sprintf(buffer, "800%d", j);
                                    requests_list[j] = IN_TALK;
                                    send(clients_sockets[i - 4], buffer, 1024, 0);
                                    memset(buffer, 0, 1024);
                                    check = 1;
                                }
                            }
                        }
                        if (check == 1){
                            continue;
                        }
                        strcpy(buffer, "-");
                        send(clients_sockets[i - 4], buffer, 1024, 0);
                        memset(buffer, 0, 1024);
                        continue;
                    }

                    if (strcmp(buffer, "BadChat") == 0){   //student finishing his request
                        memset(buffer, 0, 1024);
                        for (int j = 0; j < req_counter; j ++){
                            if (Student_ID[j] == i){
                                if (requests_list[j] == IN_TALK){
                                    requests_list[j] = NOT_ANSWERED;
                                    TA_ID[j] = EMPTY;
                                    break;
                                }
                            }
                        }
                        strcpy(buffer, "your request isnt answerd and return to list\n");
                        send(clients_sockets[i - 4], buffer, 1024, 0);
                        memset(buffer, 0, 1024);
                        continue;
                    }

                    if (strcmp(buffer, "done") == 0){   //student finishing his request
                        memset(buffer, 0, 1024);
                        int req_id;
                        for (int j = 0; j < req_counter; j ++){
                            if (Student_ID[j] == i){
                                if (requests_list[j] == IN_TALK){
                                    req_id = j;
                                    requests_list[j] = ANSWERED;
                                    break;
                                }
                            }
                        }
                        strcpy(buffer, "your request is done\nPlease enter your answer\n");
                        send(clients_sockets[i - 4], buffer, 1024, 0);
                        memset(buffer, 0, 1024);
                        recv(clients_sockets[i - 4], buffer, 1024, 0);
                        char temp[10240];
                        if(check_empty_DB != 0){
                            int tfile = open("database.txt", O_RDONLY);
                            read(tfile, temp, 10240);
                            close(tfile);
                            char sub_buffer[1024];
                            int wfile = open("database.txt", O_RDWR, 0);
                            sprintf(sub_buffer, "student:%d reqID:%d\n title:%sanswer:%s\n", i, req_id, req_titles[req_id], buffer);
                            strcat(temp, sub_buffer);
                            write(wfile, temp, strlen(temp));
                            close(wfile);
                        }
                        else{
                            char sub_buffer[1024];
                            int wfile = open("database.txt", O_RDWR, 0);
                            sprintf(sub_buffer, "student:%d reqID:%d\n title:%sanswer:%s\n", i, req_id, req_titles[req_id], buffer);
                            write(wfile, sub_buffer, strlen(sub_buffer));
                            close(wfile);
                        }
                        check_empty_DB = 1;
                        memset(buffer, 0, 1024);
                        strcpy(buffer, "Your Answer added to DB\n");
                        send(clients_sockets[i - 4], buffer, 1024, 0);
                        memset(buffer, 0 , 1024);
                        continue;
                    }

                    if (strcmp(buffer, "SeeRooms\n") == 0){
                        memset(buffer, 0, 1024);
                        int check = 0;
                        for (int j = 0; j < req_counter; j ++){
                            if (requests_list[j] == IN_TALK){
                                check = 1;
                                char temp[1024];
                                sprintf(temp, "%d_%s\n", j, req_titles[j]);
                                strcat(buffer, temp);
                            }
                        }
                        if (check == 0){
                            strcpy(buffer, "-");
                        }
                        send(clients_sockets[i - 4], buffer, 1024, 0);
                        memset(buffer, 0, 1024);
                        continue;
                    }





                    if (strcmp(buffer, "SeeReqs\n") == 0){  //TA see Reqs
                        memset(buffer, 0, 1024);
                        int check = 0;
                        if (req_counter == 0){
                            strcpy(buffer, "None\n");
                        }
                        else{
                            for (int j = 0; j < req_counter; j ++){
                                if (requests_list[j] == NOT_ANSWERED){
                                    char temp[1024];
                                    check = 1;
                                    sprintf(temp, "%d:%s", j, req_titles[j]);
                                    strcat(buffer, temp);
                                }
                            }
                        }
                        if (check == 0){
                            strcpy(buffer, "None\n");
                        }
                        send(clients_sockets[i - 4], buffer, 1024, 0);
                        memset(buffer, 0, 1024);
                        continue;
                    }

                    if (buffer[0] == '>'){  //TA accepting a req
                        char temp[1024];
                        for (int j = 1; j < strlen(buffer); j ++){
                            temp[j - 1] = buffer[j];
                        }
                        memset(buffer, 0, 1024);
                        int req_id = atoi(temp);
                        if (requests_list[req_id] == EMPTY){
                            strcpy(buffer, "Bad Request ID");
                        }
                        if (requests_list[req_id] == IN_TALK || requests_list[req_id] == WAIT_FOR_STUDENT){
                            strcpy(buffer, "Request is in talk with someone else");
                        }
                        if (requests_list[req_id] == NOT_ANSWERED){
                            sprintf(buffer, "!800%d", req_id);
                            requests_list[req_id] = WAIT_FOR_STUDENT;
                            TA_ID[req_id] = i;                        }
                        if (requests_list[req_id] == ANSWERED){
                            strcpy(buffer, "The request has been answered");
                        }
                        send(clients_sockets[i - 4], buffer, 1024, 0);
                        memset(buffer, 0, 1024);
                        continue;
                    }
                }
            }
        }

    }

    return 0;
}