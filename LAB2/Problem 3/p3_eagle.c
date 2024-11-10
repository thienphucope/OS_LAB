#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>

#define MSG_QUEUE_KEY 1234
#define MSG_SIZE 100

struct message {
    long msg_type;
    char content[MSG_SIZE];
};

void* send_message(void* arg) {
    int msg_id = *((int*)arg);
    struct message msg;
    msg.msg_type = 1;

    while (1) {
        printf("Enter message: ");
        fgets(msg.content, MSG_SIZE, stdin);
        msgsnd(msg_id, &msg, sizeof(msg.content), 0);
	sleep(1);
        if (strncmp(msg.content, "exit", 4) == 0)
            break;
    }
    return NULL;
}

void* receive_message(void* arg) {
    int msg_id = *((int*)arg);
    struct message msg;

    while (1) {
        msgrcv(msg_id, &msg, sizeof(msg.content), 1, 0);
        printf("Received: %s", msg.content);

        if (strncmp(msg.content, "exit", 4) == 0)
            break;
    }
    return NULL;
}

int main() {
    printf("Sparrow Calls Eagle\n");
    int msg_id = msgget(MSG_QUEUE_KEY, 0666 | IPC_CREAT);
    pthread_t Sparrow, Eagle;

    //pthread_create(&Sparrow, NULL, send_message, &msg_id);
    pthread_create(&Eagle, NULL, receive_message, &msg_id);

    //pthread_join(Sparrow, NULL);
    pthread_join(Eagle, NULL);
    pthread_cancel(Eagle);
    msgctl(msg_id, IPC_RMID, NULL);
    //Tra loi
    printf("Eagle Calls Sparrow\n");
    msg_id = msgget(MSG_QUEUE_KEY, 0666 | IPC_CREAT);

    pthread_create(&Eagle, NULL, send_message, &msg_id);
    //pthread_create(&Sparrow, NULL, receive_message, &msg_id);

    pthread_join(Eagle, NULL);
    //pthread_join(Sparrow, NULL);
    pthread_cancel(Eagle);
    msgctl(msg_id, IPC_RMID, NULL);
    return 0;
}

