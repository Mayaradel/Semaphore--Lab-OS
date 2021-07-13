#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#define counter 30
#define BufferSize 10

//empty : number of empty slots in the buffer
//full : number of slots filled in the buffer
sem_t empty,full;
sem_t msg;

pthread_t messages[counter];
pthread_t mmonitor,mcollector;

//message count
int count =0;
//buffer used to save the message and reuse it
int buffer[BufferSize];

//function of received message
void * message (void * arg)
{
//sleep function to not receive all the messages at the same time
    sleep(rand()%100);

//wait , increament the counter , post
    sem_wait(&msg);
    count = count + 1;
    sem_post(&msg);
    printf("recieved a message %d\n\n",count);
    printf("waiting to write\n\n");
}

//monitor function
//gets the count of threads
//sets the count to zero
void * monitor (void * arg)
{
    int k =0;
    while(1)
    {
        sleep(rand()%100);
        sem_wait(&full);
//the monitor gets the count of threads
//saves it in data and add it to the buffer
//sets count to zero
        int data = count;
        count = 0;
        buffer[k]=data;
        printf("Now adding to counter \n\n");
        printf("waiting to read the count \n\n");
        printf("reading a count of value %d\n\n",data);
        printf("writing to buffer at position %d\n\n",k);
        k++;
//if > than size of the buffer reset it to zero
        if (k>9)
        {
            k=0;
        }
        sem_post(&full);
        sem_post(&empty);
    }
}

//the collector function that reads the value from the buffer
void * collector (void * arg)
{
    int k=0;
    while (1)
    {
        sleep(rand()%100);
        sem_wait(&empty);
        sem_wait(&full);

        int data = buffer[k];
        printf("buffer reads %d from position %d\n\n",data,k);
        k++;
        if (k>9)
        {
            k=0;
        }
        sem_post(&full);
    }
}

int main()
{
    pthread_t pid, cid;
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setscope(&attr, PTHREAD_SCOPE_SYSTEM);

    //initialize the semaphores
    //address of the semaphore , 1 because between processes ,
    // value of the newly intialized semaphore
    sem_init(&msg,1,1);
    sem_init(&full,1,1);
    sem_init(&empty,1,0);
  //  sem_init(&size,1,10);

    int i;
    for(i=0;i<counter; i++)
    {
        pthread_create(&messages[i],&attr,message,NULL);

    }

    pthread_create(&mmonitor,&attr,monitor,NULL);
    pthread_create(&mcollector,&attr,collector,NULL);

    for(i=0; i<counter; i++)
    {
        pthread_join(messages[i],NULL);

    }
    pthread_join(mmonitor,NULL);
    pthread_join(mcollector,NULL);

    return 0;
}

