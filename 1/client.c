#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <signal.h>

#define STUDENT 5
#define TA 6



int alarm_flag = 0;

void alarmHandler(){
    alarm_flag = 1;
    return;
}

int connectUdp(int UDPport) {
    int sock, broadcast = 1, opt = 1;
    char buffer[1024] = {0};
    struct sockaddr_in bc_address;

    sock = socket(AF_INET, SOCK_DGRAM, 0);
    setsockopt(sock, SOL_SOCKET, SO_BROADCAST, &broadcast, sizeof(broadcast));
    setsockopt(sock, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt));


    bc_address.sin_family = AF_INET; 
    bc_address.sin_port = htons(UDPport); 
    bc_address.sin_addr.s_addr = inet_addr("255.255.255.255");
    bind(sock, (struct sockaddr *)&bc_address, sizeof(bc_address));

    while (1) {
        memset(buffer, 0, 1024);
        recv(sock, buffer, 1024, 0);
        if (strcmp(buffer, "Exit\n") == 0){
            return 1;
        }
        printf("%s", buffer);
    }
    return 1;
}

int connectServer(int port) {
    int fd;
    struct sockaddr_in server_address;
    fd = socket(AF_INET, SOCK_STREAM, 0);
    server_address.sin_family = AF_INET; 
    server_address.sin_port = htons(port); 
    server_address.sin_addr.s_addr = inet_addr("127.0.0.1");
    if (connect(fd, (struct sockaddr *)&server_address, sizeof(server_address)) < 0) { // checking for errors
        printf("Error in connecting to server\n");
    }
    return fd;
}

int chat_with_TA_by_Student(int UDPport){
    
    printf("You are in chat\nTo exit type:Exit\n");
    
    int sock, broadcast = 1, opt = 1;
    char buffer[1024] = {0};
    struct sockaddr_in bc_address;
    sock = socket(AF_INET, SOCK_DGRAM, 0);
    setsockopt(sock, SOL_SOCKET, SO_BROADCAST, &broadcast, sizeof(broadcast));
    setsockopt(sock, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt));
    bc_address.sin_family = AF_INET; 
    bc_address.sin_port = htons(UDPport); 
    bc_address.sin_addr.s_addr = inet_addr("255.255.255.255");
    bind(sock, (struct sockaddr *)&bc_address, sizeof(bc_address));
    
    while(1) {
        char temp[1024] = {0};
        memset(buffer, 0, 1024);
        read(0, temp, 1024);
        if (strcmp(temp, "Exit\n") == 0){
            strcpy(buffer, temp);
            sendto(sock, buffer, strlen(buffer), 0,(struct sockaddr *)&bc_address, sizeof(bc_address));  
            alarm_flag = 0;         
            break;
        }
        sprintf(buffer, "Student:");
        strcat(buffer, temp);
        sendto(sock, buffer, strlen(buffer), 0,(struct sockaddr *)&bc_address, sizeof(bc_address));
        memset(temp, 0, 1024);
        memset(buffer, 0, 1024);
        recv(sock, buffer, 1024, 0);
        /*signal(SIGALRM, alarmHandler);
        siginterrupt(SIGALRM, 1);
        alarm(10);*/
        while (buffer[0] != 'T'){
            memset(buffer, 0, 1024);
            signal(SIGALRM, alarmHandler);
            siginterrupt(SIGALRM, 1);
            alarm(10);
            recv(sock, buffer, 1024, 0);
            alarm(0);
            if (alarm_flag == 1){
                alarm_flag = 0;
                printf("the TA didn't answer\n");
                memset(buffer, 0, 1024);
                strcpy(buffer, "Exit\n");
                sendto(sock, buffer, strlen(buffer), 0, (struct sockaddr *)&bc_address, sizeof(bc_address));
                return 1;
            }
        }
        /*if (alarm_flag == 1){
            alarm_flag = 0;
            printf("the TA didn't answer\n");
            memset(buffer, 0, 1024);
            strcpy(buffer, "Exit\n");
            sendto(sock, buffer, strlen(buffer), 0, (struct sockaddr *)&bc_address, sizeof(bc_address));
            return 1;
        }*/
        printf("%s", buffer);
    }
    return 1;
}

int chat_with_Student_by_TA(int UDPport){
    
    printf("You are in chat\nWait for student to start the chat\n");
    
    int sock, broadcast = 1, opt = 1;
    char buffer[1024] = {0};
    struct sockaddr_in bc_address;
    sock = socket(AF_INET, SOCK_DGRAM, 0);
    setsockopt(sock, SOL_SOCKET, SO_BROADCAST, &broadcast, sizeof(broadcast));
    setsockopt(sock, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt));
    bc_address.sin_family = AF_INET; 
    bc_address.sin_port = htons(UDPport); 
    bc_address.sin_addr.s_addr = inet_addr("255.255.255.255");
    bind(sock, (struct sockaddr *)&bc_address, sizeof(bc_address));
    
    while(1) {
        char temp[1024] = {0};
        memset(buffer, 0, 1024);
        recv(sock, buffer, 1024, 0);
        while (buffer[0] != 'S' & buffer[0] != 'E'){
            memset(buffer, 0, 1024);
            recv(sock, buffer, 1024, 0);
        }
        if (strcmp(buffer, "Exit\n") == 0){
            printf("The talk is over\n");
            alarm_flag = 0;
            return 1;
        }
        printf("%s", buffer);
        memset(buffer, 0, 1024);
        signal(SIGALRM, alarmHandler);
        siginterrupt(SIGALRM, 1);
        alarm(10);
        read(0, temp, 1024);
        alarm(0);
        if (alarm_flag == 1){
            printf("Time up!\tChat is over\n");
            alarm_flag = 0;
            return 1;
        }
        sprintf(buffer, "TA:");
        strcat(buffer, temp);
        sendto(sock, buffer, strlen(buffer), 0,(struct sockaddr *)&bc_address, sizeof(bc_address));    
    }
}


int main(int argc, char const *argv[]) {
    int type;
    int serversocket;
    char buff[1024] = {0};
    int serverport = atoi(argv[1]);

    write(1 , "Student or TA:" , 15);
    read(0 , buff, 1024);
    if (strcmp(buff, "TA\n") == 0){
        type = TA;
        printf("Sign in as TA\n");
        printf("to see all available requests type: SeeReqs\nto accept a request type:AcceptReq\n");
    }
    else{
        if (strcmp(buff, "Student\n") == 0){
            type = STUDENT;
            printf("Sign in as Student\n");
            printf("To add Request type: AddReq\nTo check your requests:CheckMyReqs\nTo See Rooms type:SeeRooms\n");
        }
        else{
            printf("Bad Role!\n");
            abort();
        }
    }
    memset(buff, 0, 1024);

    fd_set master_set, working_set;
    int max_sd = serversocket;
    int fd = connectServer(serverport);

    FD_ZERO(&master_set);
    FD_SET(fd, &master_set);
    FD_SET(0, &master_set);

    

    while (1 && (type == STUDENT)) {     // while for student
        working_set = master_set;
        select(max_sd + 1, &master_set, NULL, NULL, NULL);
        for (int i = 0; i <= max_sd; i ++){
            if (FD_ISSET(i, &working_set)){
                if (i == 0){
                    read(0, buff, 1024);
                    if (strcmp(buff, "AddReq\n") == 0){
                        memset(buff, 0, 1024);
                        printf("Write your request title\n");
                        read(0, buff, 1024);
                        char temp[1024];
                        temp[0] = '+';
                        for (int j = 0; j < strlen(buff); j ++){
                            temp[j + 1] = buff[j];
                        }
                        send(fd, temp, 1024, 0);
                        memset(buff, 0, 1024);
                        recv(fd, buff, 1024, 0);
                        write(1, buff, strlen(buff));
                    }
                    if (strcmp(buff, "CheckMyReqs\n") == 0){
                        send(fd, buff, 1024, 0);
                        memset(buff, 0, 1024);
                        recv(fd, buff, 1024, 0);
                        if (buff[0] == '-'){
                            printf("you have no available chat request\n");
                        }
                        else{
                            int UDP_port = atoi(buff);
                            chat_with_TA_by_Student(UDP_port);
                            memset(buff, 0, 1024);
                            printf("Is your request answered? type: Yes or No\n");
                            read(0, buff, 1024);
                            if (strcmp(buff, "No\n") == 0){
                                memset(buff, 0, 1024);
                                strcpy(buff, "BadChat");
                                send(fd, buff, 1024, 0);
                                memset(buff, 0, 1024);
                                recv(fd, buff, 1024, 0);
                                write(1, buff, strlen(buff));
                            }
                            else{
                                memset(buff, 0, 1024);
                                strcpy(buff, "done");
                                send(fd, buff, 1024, 0);
                                memset(buff, 0, 1024);
                                recv(fd, buff, 1024, 0);
                                write(1, buff, strlen(buff));
                                memset(buff, 0, 1024);
                                read(0, buff, 1024);
                                send(fd, buff, 1024, 0);
                                memset(buff, 0, 1024);
                                recv(fd, buff, 1024, 0);
                                write(1, buff, strlen(buff));
                            }
                            memset(buff, 0, 1024);
                        }
                    }
                    if (strcmp(buff, "SeeRooms\n") == 0){
                        send(fd, buff, 1024, 0);
                        memset(buff, 0, 1024);
                        recv(fd, buff, 1024, 0);
                        if (buff[0] == '-'){
                            printf("There is no active chat right now\n");
                        }
                        else{
                            printf("to select a room type its number\nto close type: Close\n");
                            write(1, buff, strlen(buff));
                            memset(buff, 0, 1024);
                            read(0, buff, 1024);
                            if (strcmp(buff, "Close\n") == 0){
                                printf("Closed\n");
                                memset(buff, 0, 1024);
                                continue;
                            }
                            else{
                                char temp[1024];
                                sprintf(temp, "800%s", buff);
                                int UDP_port = atoi(temp);
                                printf("You are in chat\n");
                                connectUdp(UDP_port);
                                printf("The chat is over\n");
                            }
                            memset(buff, 0, 1024);
                        }
                    }
                    memset(buff, 0, 1024);
                }
                else{

                    memset(buff, 1024, 0);
                }
            }
        }
    }



    while (1 && (type == TA)) {     // while for TA
        working_set = master_set;
        select(max_sd + 1, &master_set, NULL, NULL, NULL);
        for (int i = 0; i <= max_sd; i ++){
            if (FD_ISSET(i, &working_set)){
                if (i == 0){
                    read(0, buff, 1024);
                    
                    if (strcmp(buff, "SeeReqs\n") == 0){
                        send(fd, buff, 1024, 0);
                        memset(buff, 0, 1024);
                        recv(fd, buff, 1024, 0);
                        write(1, buff, strlen(buff));
                        memset(buff, 0, 1024);
                    }

                    if (strcmp(buff, "AcceptReq\n") == 0){
                        memset(buff, 0, 1024);
                        printf("write number of request\n");
                        read(0, buff, 1024);
                        char temp[1024];
                        temp[0] = '>';
                        for (int j = 0; j < strlen(buff); j ++){
                            temp[j + 1] = buff[j];
                        }
                        send(fd, temp, 1024, 0);
                        memset(buff, 0, 1024);
                        recv(fd, buff, 1024, 0);
                        memset(temp, 0, 1024);
                        if (buff[0] == '!'){
                            for (int j = 1; j < strlen(buff); j ++){
                                temp[j - 1] = buff[j]; 
                            }
                            int UDP_port = atoi(temp);
                            chat_with_Student_by_TA(UDP_port);
                        }
                        else{
                            printf("%s\n", buff);
                        }
                    }

                    memset(buff, 0, 1024);
                }
            }
        }
    }

    return 0;
}