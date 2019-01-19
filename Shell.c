/**
 * CS 241 - Systems Programming
 *
 * Pied Piper - Spring 2017
 */

#include "pied_piper.h"
#include "utils.h"
#include <fcntl.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h> 
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#define TRIES 3

/** 
		   FUNCTION PROTOTYPES
							**/

void descriptor_close(int * arr, int num_commands);
void descriptor_close_all(int * arr1, int *arr2,int num_commands);
int count_num_commands(char** executables);
//void kill_all_process(pid_t cur, pid_t pid[],int num_commands);
void freefunc(failure_information* info, int num_commands);

int pied_piper(int input_fd, int output_fd, char **executables) {
   	
/*
	if(input_fd<0)
		print_invalid_input();
	if(output_fd<0)
		print_invalid_output();
*/
	int num_commands=count_num_commands(executables);			//number of processes to execute
	
	failure_information info[num_commands]; 
	
	for(int i=0;i<num_commands;i++){					// setting COMMAND part of failure info 
		info[i].command=strdup(executables[i]);
		info[i].error_message=strdup("");
	}

	int tries=3;
	int num_pipes=num_commands-1;

	while(tries){
		
		int fd_arr[2*num_pipes];						//array for pipe fd's num_pipes=num_commands-1
		int fd_err_arr[2*(num_commands)];					//array fo storing err pipe fd's 1 for every process
		int fds[num_pipes][2];						
		int fds_err[num_commands][2];

		int k=0;
	
		for(int i=0;i<num_commands;i++){					//initialising err pipes for all processes
	
			pipe2(fds_err[i],O_CLOEXEC);

			fd_err_arr[k]=fds_err[i][0];					//adding err fd's to array
			++k;
			fd_err_arr[k]=fds_err[i][1];
			++k;
		}	
		
		k=0;				

		for(int i=0;i<num_pipes;i++){						//initialising pipes 
	
			pipe2(fds[i], O_CLOEXEC);

                	fd_arr[k]=fds[i][0];						//adding fd's to arr
	                ++k;
        	        fd_arr[k]=fds[i][1];
                	++k;
		}

		pid_t pid[num_commands];						//pid arr for all processes
		
		int flag=0;								//checks if all processes ran without any issue 

		for(int i=0;i<num_commands;i++){					// forking and execing all processes
	
			pid[i]=fork();

			if(pid[i]==-1){
											//fork failed
			}
			else if(pid[i]==0){						// IN CHILD

				dup2(fds_err[i][1],2);
			
				if(i==0){						//first process
					dup2(fds[i][1],1);				//process 1 output
					if(input_fd>=0){						
						dup2(input_fd,0);			//process 1 input if file exists otherwise stdin 
					}

					descriptor_close_all(fd_arr,fd_err_arr,num_commands);

					if( exec_command(executables[i]) == -1){
						print_invalid_input();
						exit(1);
					}	
				}
				else if(i == num_commands-1){				//last process
					dup2(fds[i-1][0],0);				//input
					if(output_fd>=0){				//output if file exists
						dup2(output_fd,1);
					}

					descriptor_close_all(fd_arr,fd_err_arr, num_commands);

					if( exec_command(executables[i]) == -1){
        	                        	print_invalid_input();
						exit(1);
	                        	}
				}
		                else{							//intermediate processes
                                        dup2(fds[i][1],1);				//output
                                        dup2(fds[i-1][0],0);				//input

                                        descriptor_close_all(fd_arr,fd_err_arr, num_commands);

                                        if( exec_command(executables[i]) == -1){
                                                print_invalid_input();
                                                exit(1);
                                        }
                                }

			}								//BACK IN PARENT

				close(fds_err[i][1]);					//closing input end of err pipe of process just forked

				if(i==0){
					close(fds[i][1]);
				}
				else if(i==num_commands-1){
					close(fds[i-1][0]);
				}
				else{
					close(fds[i][1]);
					close(fds[i-1][0]);
				}
		
		}

		for(int i=0;i<num_commands;i++){					//waiting on the processes to finish execution
			
			int status=0;
			int index=0;
			pid_t cur_pid;		

			if(tries==1){							// in the final run

				if((cur_pid=wait(&status)) != -1){
								
	                              	for(int j=0;j<num_commands;j++){			//finding index of the pid 
                                            	if(cur_pid==pid[j]){
							index=j;
							break;
						}
                                        }

				
//					printf("%d\n",status);

					if(WIFEXITED(status)){
					//	printf("stat: %d\n",status);
                                               	if(WEXITSTATUS(status)!=0){		//if the process didnt execute properly
							
							flag=1 ;			//indicates failure in execution in the final run

//                                                        kill_all_process(cur_pid,pid,num_commands);	//kill other running processes
//  							for(int k=index+1;k<num_commands;k++){
//								kill(pid[k],SIGKILL);	
//							}                            									
			        
 	                                              	char buf[1024];					//reading the err pipe of the faulty process
							memset(buf,0,1024);

							int bytesread=read(fds_err[index][0],&buf,1024);

							if(bytesread!=0){
								free((void*)info[index].error_message);
								info[index].error_message=strdup(buf);
							}
							info[index].status=status;
							
					      	}
						else{							//process executed properly
							info[index].status=status;
						}
					}
					else if(WIFSIGNALED(status)){					//processes that were killed by SIGKILL
						info[index].status=status;					
					}             
                              	}
		
			}
		
			else{										// RUN 1 and 2
				if( (cur_pid=wait(&status)) != -1){
					if(WIFEXITED(status)){
						if(WEXITSTATUS(status)!=0){
							for(int j=0;j<num_commands;j++){                    //finding index of the pid
        		                                        if(cur_pid==pid[j]){
									index=j;
	                                                        	break;
								}
                        		                }
                                        	

							//kill_all_process(cur_pid,pid,num_commands);
//							for(int k=index+1;k<num_commands;k++){
//                                                              kill(pid[k],SIGKILL);
//                                                        }
							

							flag=1;
						}	
					}
				}
			}
		}
		
		if(flag==0)								//checking if none of the process was faulty
			break;

		off_t len=0;								//if something was printed to o/p file it is reset
		reset_file(output_fd);
		ftruncate(output_fd, len);

		--tries;	

	}

	if(tries==0){
		print_failure_report(info, num_commands);
		freefunc(info,num_commands);
		return EXIT_OUT_OF_RETRIES;
	}
	else{
		freefunc(info,num_commands);	
    		return 0;
	}
}

void freefunc(failure_information *info, int num_commands){
	for(int i=0;i<num_commands;i++){
		free((void*)info[i].command);
		free((void*)info[i].error_message);
	}
}

int  count_num_commands(char** executables){
int num_commands=0;

	while(*executables!=NULL){
		++executables;
		++num_commands;
	}

	return(num_commands);
}
/*
void kill_all_process(pid_t cur, pid_t pid[],int num_commands){
	for(int i=0;i<num_commands;i++){
		if(cur != pid[i])	
			kill(pid[i],SIGKILL);
	}
}
*/
void descriptor_close_all(int *fd_arr, int *fd_err_arr,int num_commands){
	for(int i=0;i<(num_commands*2);i++){
		close(fd_err_arr[i]);
	}
	
	for(int i=0;i<(2*(num_commands-1));i++){
		close(fd_arr[i]);
	}

}

void descriptor_close( int * fd_arr,int num_commands){

     for(int i=1;i<(2*(num_commands-1));i+=2){
                close(fd_arr[i]);
        }

}