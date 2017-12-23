#include "TestCase1.h"
#define MAX_TEXT 512

struct mymsgbuf{
        long mytype;
        char mtext[MAX_TEXT];

};
int mykey = 1234;

void ReadInTestCase1()
{
    int running = 1;
    int msgid;
    struct mymsgbuf some_data;
    long int msg_to_receive = 1;

/* First, we set up the message queue. */

    msgid = mymsgget(mykey, 0);

    if (msgid == -1) {
        perror("msget err in readMsg :");
        exit(0);
    }

/* Then the messages are retrieved from the queue, until an end message is encountered.
 Lastly, the message queue is deleted. */

    while(running) {
        if (mymsgrcv(msgid, (void *)&some_data, MAX_TEXT,msg_to_receive, 0) == -1) {
            perror("msgrcv error in readMsg : ");
            exit(0);
        }
        printf("\nYou wrote: %s", some_data.mtext);
        if (strncmp(some_data.mtext, "end", 3) == 0) {
            running = 0;
        }
    }

    if (mymsgctl(msgid,MY_IPC_RMID, 0) == -1) {
        fprintf(stderr, "msgctl(IPC_RMID) failed\n");
        exit(0);
    }

}

void SendInTestCase1()
{
    int running = 1;
    struct mymsgbuf some_data;
    int msgid, len;
    char buffer[MAX_TEXT];

    msgid = mymsgget(mykey, 0);

    if (msgid == -1) {
        fprintf(stderr, "msgget failed with errord\n");
        exit(-1);
    }

    
    some_data.mytype = 1;

    while(running) {
        printf("Enter some text or :msg_type (type 'end' is exit)");
        fgets(buffer, MAX_TEXT, stdin);
        strcpy(some_data.mtext, buffer);
        if (buffer[0] == ':')
        {
            printf("Change msg_type from: %d\n", (int)some_data.mytype);
            some_data.mytype = atoi(&buffer[1]);
            printf("    to msg_type: %d\n", (int)some_data.mytype);
            continue; 
        }
        len = strlen(some_data.mtext) +1;
        if (mymsgsnd(msgid, (void *)&some_data, len , 1) == -1) {
            fprintf(stderr, "msgsnd failed\n");
            exit(0);
        }
        if (strncmp(buffer, "end", 3) == 0) {
            printf("Entering Ended\n");
            running = 0;
            exit(0);

        }
    }
}

void TestCase1(void)
{
    thread_t pid[2];

    thread_create(&pid[0],NULL,(void*)ReadInTestCase1,0);
    thread_create(&pid[1],NULL,(void*)SendInTestCase1,0);

}


