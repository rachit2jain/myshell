
/*
####################################################################################################################################


													COMP3230A myshell.c   															
												    Name:   JAIN Rachit                                              
												    UID:    3035134721                                            
												    Email:  rachit96@hku.hk  
													 
													Development platform: Ubuntu 14.04
													Last Modified: 22 October 2016
													Compilation method: gcc myshell.c -o myshell                                                   


####################################################################################################################################
*/
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>


int background =0;
int background_process =0;
int not_background =0;
int foreground =0;
int pipePresent =0;
int timex =0;
void everything();

void timeX(int who){		//timex function

	char str[50], state;
	FILE * file;
	int z;
	unsigned long long i, x;
	unsigned long h, ut, st;
	unsigned long long int start;
	
	sprintf(str, "/proc/%d/stat", who);	//store file name in str
	file = fopen(str, "r");			//open file
	if (file == NULL) {
		printf("Error in open my proc file for %s\n", str);
		//exit(0);
	}
	fscanf(file, "%d %s %c %d %d %d %d %d %u %lu %lu %lu %lu %lu %lu %ld %ld %ld %ld %ld %ld %llu", &z, str, &state, &z, &z, &z, &z, &z, (unsigned *)&z, &h, &h, &h, &h, &ut, &st, &h, &h, &h, &h, &h, &h, &start); //read all params and save the ones needed
	fclose(file);

	float up;							
	file = fopen("/proc/uptime", "r");		//getting current time for calculating runtime of process
	fscanf(file, "%f", &up);
	fclose(file);

	printf("\nPID \t CMD \t\t RTIME \t\t UTIME \t\t STIME \n");
	printf("%d \t %-10s \t %0.2f s \t %0.2f s \t %0.2f s \n", who, str, up- (start)/sysconf(_SC_CLK_TCK), ut*1.0f/sysconf(_SC_CLK_TCK), st*1.0f/sysconf(_SC_CLK_TCK));

}
void sigint_handler(int signum) {		//sigint handler
    printf("\n## myshell $ " );			//print the command line again
    fflush(stdout);				//flush all the variables
}

void sigchld_handler(int signum, siginfo_t *sig, void *context){		//sig child handler for all the processes
		int status;
		if(timex){
			timex=0;
			timeX(sig->si_pid);	
		}
		
		if(!foreground){												//if the process was backfground
			printf("Done\n");
		}
		
		int stop =0;
		
		waitpid(-1, &status,0);
		foreground=1;
}
void sigusr1_handler(int signum) { //Signal handler for SIGUSR1 
									//used by the child process to unpause and start execution
} 
void sigusr2_handler(int signum, siginfo_t *sig, void *v) { 
	kill(sig->si_pid, SIGUSR1); //sending this signal to inform child process to unpause and start execution
}
void everything() {
	fflush(stdout);
	fflush(stdin);
	char command[256];
	char command1[256];
	char command2[256];    
    char* argv[100]; 
    char* argv3[100];
    char* argv1[100];      
    char* argv2[1000];       
    char* path= "";    
    char filepath[64];      
    char filepath1[64];  
    char filepath2[64];      
    int argc;
    int pfd1[2], i ; //pfd2[2], i;
	pipe(pfd1);
	
	signal(SIGINT, sigint_handler);			//handling sigint
	signal(SIGUSR1, sigusr1_handler);	//handling sigusr1
		
	struct sigaction su2;				//handling sigusr2
	sigaction(SIGUSR2, NULL, &su2);
	su2.sa_flags = SA_SIGINFO;
	su2.sa_sigaction = sigusr2_handler;
	sigaction(SIGUSR2, &su2, NULL);
	int counter =0;
	
				
	while(1==1){
		//printf("foreground in main handler is %d\n", foreground);
		    printf("## myshell $ ");
		     
		    fflush(stdout);
			
			pid_t who;
			int talk;
			for(talk=0; talk<256; talk++)
				command[talk]= '\0';
		    while(fgets(command, 256, stdin)==NULL){

		    }
		    	
		    
		     if(strlen(command)==0){			//if command is empty 
		    
		     	continue;
		     }
		    counter++;
		    int status;
		    strcpy(command1,command);

		    char *pos;
				if ((pos=strchr(command, '\n')) != NULL)	//removing the traling \n
	   				*pos = '\0';
	   
	   		if(strcmp(command, "exit")==0){ 
	   			printf("myshell: Terminated\n");           //check if command is exit
	        	exit(0);
	    	}
	    	if(strcmp(command, "")==0){			//check if the command is enter
		    	continue;
		    }
		    
		    char *tok;                  
		    tok = strtok(command," ");				//spliting the command with space as delimiter
		    int i=0;
		    while(tok!=NULL){
		    	argv[i]=tok;				//storing the command in argv with space as delimiter
		   		
		        if(strcmp(tok, "|")==0){			//checking if the pipe is present
		       		pipePresent =1; 				//if pipe present, make the global variable 1
		       	}
				tok = strtok(NULL," ");
		        i++;
		    }

		    argv[i]=NULL;               	//last argument of argv as NULL      
		    argc=i;  
		    int p=0;
		   
		    if(strcmp (argv[0], "timeX") ==0){	//checking if the command is timeX
		    	 timex = 1;
		    	 p=1;
		    	int idx=0;
		    	for (idx = 0; idx < i-1; ++idx) 	//removing the word "timeX" from the command the shifting all the 
		    		argv3[idx] = argv[idx+1];		//arguments in agrv3 after removing timeX

		    	argv3[idx]=NULL;
		    	i--;
		    }
		    else{										//else if timex is not present
		    	p=0;									//shifting all the arguments of the command to agrv3
		    	while(argv[p]!=NULL){					
		    		argv3[p] = argv[p];
		    		p++;
		    	}
		    	argv3[p]=NULL;
		    }
		    
		    if(timex == 1 && argv3[0]==NULL){						//handling timex improper usage
		    	printf("Timex cant be a standalone command \n");
		    	timex=0;
		    	fflush(stdout);
		    	everything();			//calling the everything function after detecting improper usage
		    }
		   
			if(strcmp (argv3[i-1], "&")==0 && timex !=1 ){
		    	background =1;									//global variable to detect the backgroudn processes
		    	background_process = 1;
		    	//foreground =1;
		    	argv3[i-1] = NULL;
		    }
		    else if (strcmp (argv3[i-1], "&")==0 && timex ==1){				//handling timex improper usage
		    	printf("Timex cant be used in background process\n");
		    	timex=0;
		    	continue;							//calling the everything function after detecting improper usage

		    }
		    strcpy(filepath, path);           		//removing the \n from the command 
			strcat(filepath, argv3[0]);
			for(i=0; i<strlen(filepath); i++){    
			        if(filepath[i]=='\n'){      
			            filepath[i]='\0';
			        }
			}
			if(strcmp(filepath, "exit") ==0){
				printf("myshell: \"exit\" with other arguments\n");
				continue;
			}

//##################################### our command is in arg3 and the main command is in filepath ##########################
		   if(!pipePresent){				//if pipe not present 
		    	
		    	if(background!=1)
		    		foreground=1;
		    	else	
		    		foreground = 0;	

		    	struct sigaction sa1;						//sigchld handler
				sigaction(SIGCHLD, NULL, &sa1);
				sa1.sa_flags = SA_SIGINFO;
				sa1.sa_sigaction = sigchld_handler;
				sigaction(SIGCHLD, &sa1, NULL);	
				
				who = fork();
				//divided the main execution of commands in 4 parts
				//background child, foreground child, parent of background child, parent of foreground child
			
				if(who==0 && background == 1 ){						//child process running in background
					background_process=1;
					background = 0;

					sigset_t new;									//ignoring sigint when the background process	
					sigemptyset(&new);								//is running, masking sigint
					sigaddset(&new, SIGINT);
					sigprocmask(SIG_BLOCK,&new,NULL);
					printf("%s\n",filepath );

					kill(getppid(), SIGUSR2);					//sigusr1
					pause();
					if (execvp(filepath,argv3) == -1) {
						printf("myshell $ %s %s\n",command, strerror(errno));
						exit(-1);
					}
				}
				else if(who == 0 && background !=1){						// foreground child process 	
					
					kill(getppid(), SIGUSR2);								//sigusr
					pause();
					if (execvp(filepath,argv3) == -1) {

						printf("myshell $ %s %s\n",command, strerror(errno));
						exit(-1);
					}
				}
				else if (who>0 && background !=1){						//parent of foreground child	
					
				}
				else if(who > 0 && background == 1){					//parent of background child
					
				}
				strcpy(command,"");		//making the command string empty
				background=0;
				
			 }
			else{										//if pipe is not present
				int fd =0;
				struct sigaction sa2;	
				sigaction(SIGCHLD, NULL, &sa2);
				sa2.sa_flags = SA_SIGINFO;
				sa2.sa_sigaction = sigchld_handler;
				sigaction(SIGCHLD, &sa2, NULL);	
			    foreground = 1;

				strcpy(command2, command1);				//copying the original command to command 2
				//counting the number of commands
				char *to;                  
			   		to = strtok(command2,"|");	//splitting the command to find out the number of pipes used
			   		int count=0;
			    
					while(to!=NULL){
						to = strtok(NULL,"|");
						count++;					//variable count has the number of pipes 
					}
				count--;
				//ends here 	
				int pfd1[2], pfd2[2], pfd3[2], pfd4[2], pfd5[2];
				pipe(pfd1);
				pipe(pfd2);
				pipe(pfd3);
				pipe(pfd4);
				pipe(pfd5);
			
				char *tok1;                  
		   		tok1 = strtok(command1,"|");				//splitting command1 with delimiter as "|"
		   		int iteration=0;
		    	int argc1=0;
		    	int count1 = count +1;	
				while(tok1!=NULL){
					if(timex==1){							//if timeX in pipe, remove the command timeX and prepare 
						char *res;								//the command for execution
						for (res = tok1 ; *res && *res != ' ' ; res++);
						if (*res) res++;

						argv1[argc1]=res;
						argc1++;
						tok1 = strtok(NULL, "|");
						argv1[argc1] = tok1;
					}
					else{								//else copy the command to agrv1
						argv1[argc1] = tok1; 
						argc1++;
						tok1 = strtok(NULL, "|");
					}
				}
				argv1[argc1] =NULL;			//last agrument of argv1 to be NULL
				argc1 =0;
				
//if our command was "cat Makefile | wc -l", argv1[0] will have cat Makefile and argv[1] will have wc -l
				int j;
				while(argv1[argc1]!=NULL){		
					char *token;              
				   	token = strtok(argv1[argc1]," ");			//splitting the already splitted command with space as limiter
				    j=0;	//j contains the number of commands
				    while(token!=NULL){
				    	argv2[j]=token;
				    	j++;
						token = strtok(NULL," ");
						
				    }
				    argv2[j]=NULL;			//last argument as NULL

				    strcpy(filepath1, path);           
					strcat(filepath1, argv2[0]);
					for(i=0; i<strlen(filepath1); i++){  	//removing \n from command  
					        if(filepath1[i]=='\n'){      
					            filepath1[i]='\0';
					        }
					}
					
					int x =0;
					char comp[100];
					while(argv2[x]!=NULL){	
						x++;
					}
					strcpy(filepath2, path);
					strcat(filepath2, argv2[x-1]);

					for ( i = 0; i < strlen(filepath2); i++){	//removing \n from the last agrument of command
						if(filepath2[i]=='\n'){
							filepath2[i]='\0';
						}
					}
					strcpy(argv2[x-1], filepath2);
					if(j+1<=count){
						
						printf("myshell: Incomplete '|' sequence\n");
						continue;
					}

//handling pipes, count contains the number of pipes
					if(count==4){					//if there are 4 pipes
						
						if (iteration==0){		//iteration is used to differentiate between different parts  
							if(fork()==0){		//of the commands e.g. cat Makefile | wc -l
													//when iteration is 0, cat Makefile will be handled
								close(pfd1[0]);		//when iteration is 1, wc -l will be handled
								close(pfd2[0]);
								close(pfd2[1]);
								close(pfd3[0]);
								close(pfd3[1]);
								close(pfd4[0]);
								close(pfd4[1]);
								dup2(pfd1[1],1);
							
								if (execvp(filepath1, argv2) == -1) {
									printf("execvp: error no = %s\n", strerror(errno));
									exit(-1);
								}
							}
						}
						else if(iteration==1){
							if(fork()==0){
								close(pfd1[1]);
								close(pfd2[0]);	
								close(pfd3[0]);
								close(pfd3[1]);
								close(pfd4[0]);
								close(pfd4[1]);
								dup2(pfd1[0],0);
								dup2(pfd2[1],1);
								if (execvp(filepath1, argv2) == -1) {
									printf("execvp: error no = %s\n", strerror(errno));
									exit(-1);
								}
							}
							
						}
						else if (iteration==2){
							if(fork()==0){
								close(pfd1[1]);
								close(pfd1[0]);
								close(pfd2[1]);
								close(pfd3[0]);
								close(pfd4[1]);
								close(pfd4[0]);
								dup2(pfd2[0],0);	
								dup2(pfd3[1],1);
								if (execvp(filepath1, argv2) == -1) {
									printf("execvp: error no = %s\n", strerror(errno));
									exit(-1);
								}
							}
						}
						else if (iteration==3){
							if(fork()==0){
								close(pfd1[1]);
								close(pfd1[0]);
								close(pfd2[1]);
								close(pfd2[0]);	
								close(pfd3[1]);
								close(pfd4[0]);
								dup2(pfd3[0],0);
								dup2(pfd4[1],1);
								
								if (execvp(filepath1, argv2) == -1) {
									printf("execvp: error no = %s\n", strerror(errno));
									exit(-1);
								}
							}
						}
						else if(iteration==4){
							if(fork()==0){
								close(pfd1[1]);
								close(pfd1[0]);
								close(pfd2[1]);
								close(pfd2[0]);	
								close(pfd3[1]);
								close(pfd3[0]);
								close(pfd4[1]);
								dup2(pfd4[0],0);
								
								if (execvp(filepath1, argv2) == -1) {
									printf("execvp: error no = %s\n", strerror(errno));
									exit(-1);
								}
							}
						}
					}
					else if(count==3){					//if there are 3 pipes
						//printf("%d commands\n", count);
						if (iteration==0){
							if(fork()==0){
								close(pfd1[0]);
								close(pfd2[0]);
								close(pfd2[1]);
								close(pfd3[0]);
								close(pfd3[1]);
								dup2(pfd1[1],1);
								if (execvp(filepath1, argv2) == -1) {
									printf("execvp: error no = %s\n", strerror(errno));
									exit(-1);
								}
							}
						}
						else if(iteration==1){
							if(fork()==0){
								close(pfd1[1]);
								close(pfd2[0]);	
								close(pfd3[0]);
								close(pfd3[1]);
								dup2(pfd1[0],0);
								dup2(pfd2[1],1);

								if (execvp(filepath1, argv2) == -1) {
									printf("execvp: error no = %s\n", strerror(errno));
									exit(-1);
								}
							}
						}
						else if (iteration==2){
							if(fork()==0){
								close(pfd1[1]);
								close(pfd1[0]);
								close(pfd2[1]);
								close(pfd3[0]);
								dup2(pfd2[0],0);	
								dup2(pfd3[1],1);
						
								if (execvp(filepath1, argv2) == -1) {
									printf("execvp: error no = %s\n", strerror(errno));
									exit(-1);
								}
							}	
						}
						else if (iteration==3){
							if(fork()==0){
								close(pfd1[1]);
								close(pfd1[0]);
								close(pfd2[1]);
								close(pfd2[0]);	
								close(pfd3[1]);
								dup2(pfd3[0],0);
							
								if (execvp(filepath1, argv2) == -1) {
									printf("execvp: error no = %s\n", strerror(errno));
									exit(-1);
								}
							}
						}
					}
					else if(count==2){						//if there are 2 pipes
					
						if (iteration==0){	
							if(fork()==0){
								close(pfd1[0]);
								close(pfd2[0]);
								close(pfd2[1]);	
								dup2(pfd1[1],1);
								
								if (execvp(filepath1, argv2) == -1) {
									printf("execvp: error no = %s\n", strerror(errno));
									exit(-1);
								}	
							}
						}
						else if(iteration==1){
							//printf("iter 1\n");
							if(fork()==0){
								//printf("executing 2nd command\n");
								close(pfd1[1]);
								close(pfd2[0]);
								dup2(pfd1[0],0);
								dup2(pfd2[1],1);
							
								if (execvp(filepath1, argv2) == -1) {
									printf("execvp: error no = %s\n", strerror(errno));
									exit(-1);
								}	
							}
						}
						else if (iteration==2){
							//printf("iter 2\n");
							if(fork()==0){
								//printf("executing 3rd command\n");
								close(pfd1[1]);
								close(pfd1[0]);
								close(pfd2[1]);
								dup2(pfd2[0],0);
								
								if (execvp(filepath1, argv2) == -1) {
									printf("execvp: error no = %s\n", strerror(errno));
									exit(-1);
									
								}		
							}
						}
					}
					else if(count==1){						//if there is 1 pipe
											
							if (iteration==0){
								int pidd = fork();
								if(pidd==0){

									close(pfd1[0]);	
									dup2(pfd1[1],1);
									
									if (execvp(filepath1, argv2) == -1) {
										printf("execvp: error no = %s\n", strerror(errno));
										exit(-1);
										
									}
								}
								
							}
							else if(iteration==1){
								int piid = fork();
								if(piid==0){
									close(pfd1[1]);
									dup2(pfd1[0],0);
									
									if (execvp(filepath1, argv2) == -1) {
										printf("execvp: error no = %s\n", strerror(errno));	
										exit(-1);
									}
								}	
								
							}
					}
					iteration++;	//incrementing iteration to get the next component of the command
					argc1++;
					
					if(iteration==count+1){					//closing all the pipes
						
						close(pfd1[0]);	
						close(pfd1[1]);
						close(pfd2[0]);
						close(pfd2[1]);
						close(pfd3[0]);	
						close(pfd3[1]);
						close(pfd4[0]);
						close(pfd4[1]);
					}
			    }
			pipePresent=0;	//toggling the pipePresent variable to make it 0 
			}
		
		fflush(stdout);    
	}
}
int main()
{
	everything();	//calling everything function
	return 0;
}
