//base includeok
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <unistd.h>
#include <semaphore.h>
#include <signal.h>


//karakter structok szetszedve egy masik fajlba
#include "karakterek.h"

//Majd a szekunder functionoket is szerintem szetszedem egy masik header file-ba mert konnyebb kibogozni hogy ha valami baj lesz i guess idk

int ACCRamp = 2;
int kozelharcosBaseACC = 40;
int ijaszBaseACC = 55;

Hero kozelharcos = {100, 15, 40, 1};
Hero ijasz = {100, 10, 55, 1};

Beast szorny;


int randomInt(int a, int b){
    return rand() % b + a;
}

int makeBeast(int whichBeast){
    if(whichBeast == 1){
        //{"Troll", 50, 70 , 10};
        strcpy(szorny.nev, "Troll");
        szorny.HP = 50;
        szorny.DMG = 70;
        szorny.ACC = 10;
        return 0;
    }
    else if(whichBeast == 2){
        //Sarkany, 40 50 50
        strcpy(szorny.nev, "Sarkany");
        szorny.HP = 40;
        szorny.DMG = 50;
        szorny.ACC = 50;
        return 0;
    }
    else if(whichBeast == 3){
        //Bandita, 20 10 30 
        strcpy(szorny.nev, "Bandita");
        szorny.HP = 20;
        szorny.DMG = 10;
        szorny.ACC = 30;
        return 0;
    }
    else return -1;

    return 0;
}

int pipefds[2]; // 0ba irunk 1bol olvasunk
int hit = 1;
int miss= 0;
int pipeBuffer = sizeof(hit);

sem_t szemafor;

void ijaszAction(){
    if(ijasz.ACC <= randomInt(1,100)){ // pipeba ir egy 1est mert talalat es incrementalja a szemafort
        ijasz.ACC+= ACCRamp;
        write(pipefds[0], &hit, pipeBuffer);
        printf("Ijasz attacked and hit");
    }
    else{
        ijasz.ACC = ijaszBaseACC;
        write(pipefds[1], &miss, pipeBuffer);
        printf("Ijasz attacked and missed");
    }
    sem_post(&szemafor);
}
void kozelharcosAction(){}

void szornyActionIjasz(){}
void szornyActionKozelharcos(){}

int simulateGame(){

    pid_t child_pid[3];
    sem_init(&szemafor, 0, 0);

    for (int i = 0; i < 3; i++){// child processzek letrehozasa
    
        child_pid[i] = fork();
        if(child_pid[i] == -1) return -1;
    } 
    if(child_pid[0] == 0){ // ijasz process
        printf("Child process 0 runnig.");

        signal(SIGUSR1, ijaszAction);
        printf("ijasz set up");
        while(1) pause();
        exit(0);
    }
    if(child_pid[1] == 0){ //kozelharcos process
        printf("Child process 1 runnig.");

        signal(SIGUSR1, kozelharcosAction);
        printf("kozelharcos set up"); 
        while(1) pause();
        exit(0);
    }
    if(child_pid[2] == 0){ //szorny process
        printf("Child process 2 runnig.");

        signal(SIGUSR1, szornyActionIjasz);
        signal(SIGUSR2, szornyActionKozelharcos);
        printf("szornyprocess set up");
        while(1) pause();
        exit(0);
    }

    makeBeast(randomInt(1,3));
    printf("Uj Ellenfel %s , %d-es eletero , %d-es sebzes, %d-es pontossag.", szorny.nev, szorny.HP, szorny.DMG, szorny.ACC);
    kill(child_pid[0],SIGUSR1);

    return 0;
}



int main(){
    srand(time(NULL));
    int game = simulateGame();
    if(game == -1) printf("Problema a forkok letrehozasa kozben.");
    return 0;
}