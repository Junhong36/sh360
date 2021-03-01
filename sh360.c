#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>


#define MAX_INPUT_LINE 80
//#define MAX_DIR_IN_PATH 10
#define MAX_ARGU 9
//#define MAX_PROMPT 10
#define RC_FILE ".sh360rc"
#define MAX_PATH 11
#define MAX_PATH_LENGTH 20
#define MAX_INPUT_LIST 15

void user_input(char* path_list[], int path_index);
void input_handle(char* path_list[], char* input, int path_index);
void normal(char* path_list[], char* input_list[], int input_index, int path_index );
void or_funct(char* path_list[], char* input_list[], int input_index, int path_index );
void pp_funct(char* path_list[], char* input_list[], int input_index, int path_index);


void user_input(char* path_list[], int path_index) {
	char input[MAX_INPUT_LINE];

	for (;;) {                        // repeatedly prompt the user for commands
		printf("%s", path_list[0]);

		fgets(input, MAX_INPUT_LINE, stdin);

        if((strlen(input)-1) <= 0){  // do nothing when input is empty
            continue;
        }

        if(input[strlen(input)-1]=='\n'){  // eliminate the '\n' for each input
            input[strlen(input)-1]='\0';
        }
        
		if (strcmp(input,"exit") == 0) {  // when enter "exit", terminates program
			return;
		}

		input_handle(path_list, input, path_index);
	}
}

void normal(char* path_list[], char* input_list[], int input_index, int path_index ){
	char* args[MAX_ARGU];
	char* envp[] = { 0 };
    char* fpath;
    char temp[MAX_PATH_LENGTH]; // store value for args
    int pid, status, path_match=0;

    
// matches the command from the path list
	for (int i = 1; i < path_index; i++) {
        temp[0]=0;
        strcat(temp, path_list[i]);
            
        if(temp[strlen(temp)] != '/'){
            strcat(temp, "/");
        }
            
        strcat(temp, input_list[0]);
        fpath = temp;
        
        if(access(fpath, F_OK) == 0){
            path_match = 1;
            args[0]=fpath;
            break;
        } 
        
	}
    
    if(path_match == 0){
        printf("Binary file not found!! Please try again.\n");
        return;
    }
    
    for(int i = 1; i < input_index; i++){ // store input commands into list
        args[i] = input_list[i];
    }
    args[input_index]=0;
    

	if ((pid = fork()) == 0) {
		//printf("Child: about to start...\n");
		execve(args[0], args, envp);
		//printf("Child:shouldn't be here!!\n");
	}

	//printf("parent: waiting for child to finish...\n");
    waitpid(pid, &status, 0);
    //printf("parent:child is finished.\n");
    
    return;
}

void or_funct(char* path_list[], char* input_list[], int input_index, int path_index ){
    char* args[MAX_ARGU];
	char* envp[] = { 0 };
    FILE *st_file;
    char* fpath;
    char temp[MAX_PATH_LENGTH]; // store value for args
    int pid;
	int status, symbol=0, path_match=0;
    
    for(int i=0; i < input_index; i++){
        if(strcmp(input_list[i], "->") == 0) {
            symbol=i;
            break;
        }
    }
    
    if((symbol == 0) || (symbol == (input_index-1))){  // checking format
        printf("Missing '->' symbol or output file!! Please try again.\n");
        return;
    }
    
    // matches the command from the path list
	for (int i = 1; i < path_index; i++) {
        temp[0]=0;
        strcat(temp, path_list[i]);
            
        if(temp[strlen(temp)] != '/'){
            strcat(temp, "/");
        }
            
        strcat(temp, input_list[0]);
        fpath = temp;
        
        if(access(fpath, F_OK) == 0){
            path_match = 1;
            args[0]=fpath;
            break;
        } 
	}
    
    if(path_match == 0){
        printf("Binary file not found!! Please try again.\n");
        return;
    }
  
    for(int i = 1; i < symbol; i++){
        args[i] = input_list[i];
    }
    args[symbol]=0;

    
    if((pid = fork()) == 0){
        //printf("child: in or_funct about to start...\n");
        
        if ((st_file = fopen(input_list[symbol+1], "w")) == NULL) {
            printf("Fail to open %s!! Please try again.\n", input_list[symbol+1] );
            exit(1);
	    }   
    
        dup2(fileno(st_file), 1);
        dup2(fileno(st_file), 2);
    
        execve(args[0], args, envp);
        //printf("child: shouldn't be here!!\n");
    }
    
    //printf("parent: waiting for child to finish...\n");
    waitpid(pid, &status, 0);
    //printf("parent:child is finished.\n");
    
    
}

void pp_funct(char* path_list[], char* input_list[], int input_index, int path_index ){
    char* args_first[MAX_ARGU];
    char* args_second[MAX_ARGU];
    char* args_third[MAX_ARGU];
	char* envp[] = { 0 };
    char* fpath;
    char temp1[MAX_PATH_LENGTH]; // store value for args_first
    char temp2[MAX_PATH_LENGTH]; // store value for args_second
    char temp3[MAX_PATH_LENGTH]; // store value for args_third
    int fd[4];
    int pid_first, pid_second, pid_third, status;
    int symbol_num=0;
    int symbol[2]={0,0};  // positions of "->"
    int path_match=0;     // file matched flag
    
    for(int i = 0; i < input_index; i++){
        if(strcmp(input_list[i], "->") == 0){
            symbol[symbol_num]=i;
            symbol_num+=1;
        }
    }
    
    if(symbol_num == 0){  // checking format
        printf("Missing '->' symbol or commands after '->'!! Please try again.\n");
        return;
    }
    
    if(symbol_num == 1){  // checking format
        if(symbol[0] == (input_index-1)){
            printf("Missing '->' symbol or commands after '->'!! Please try again.\n");
            return;
        }
        
        // matches the command from the path list (args_first)
        for (int i = 1; i < path_index; i++) {
            temp1[0]=0;
            strcat(temp1, path_list[i]);
            
            if(temp1[strlen(temp1)] != '/'){
                strcat(temp1, "/");
            }
            
            strcat(temp1, input_list[0]);
            fpath = temp1;
        
            if(access(fpath, F_OK) == 0){
                path_match = 1;
                args_first[0]=fpath;
                break;
            } 
        }
        
        if(path_match == 0){
            printf("Binary file not found!! Please try again.\n");
            return;
        }
        else{
            path_match=0;
        }

        
        // matches the command from the path list (args_second)
        for (int i = 1; i < path_index; i++) {
            temp2[0]=0;
            strcat(temp2, path_list[i]);
            
            if(temp2[strlen(temp2)] != '/'){
                strcat(temp2, "/");
            }
            
            strcat(temp2, input_list[symbol[0]+1]);
            fpath = temp2;
        
            if(access(fpath, F_OK) == 0){
                path_match = 1;
                args_second[0]=fpath;
                break;
            }
        }
       
        if(path_match == 0){
            printf("Binary file not found!! Please try again.\n");
            return;
        }
        
        // store the input commands into list
        for(int i = 1; i < (input_index - symbol[0] -1); i++){
            args_second[i]=input_list[i + symbol[0] + 1];
        }
        args_second[input_index - symbol[0] - 1]=0;
        
        for(int i = 1; i < symbol[0]; i++){
            args_first[i] = input_list[i];
        }
        args_first[symbol[0]] = 0;
        
        pipe(fd);
        
        //printf("parent: setting up piped commands...\n");
        if ((pid_first = fork()) == 0) {
            //printf("child (first): re-routing plumbing; STDOUT to pipe.\n");

            dup2(fd[1], 1);
            close(fd[0]);
            close(fd[1]);
            
            execve(args_first[0], args_first, envp);
            //printf("child (first): SHOULDN'T BE HERE.\n");
        }
        
        if ((pid_second = fork()) == 0) {
            //printf("child (second): re-routing plumbing; pipe to STDIN.\n");

            dup2(fd[0], 0);
            close(fd[0]);
            close(fd[1]);
            
            execve(args_second[0], args_second, envp);
            //printf("child (second): SHOULDN'T BE HERE.\n");
        }
        
        close(fd[0]);  // close pipe
        close(fd[1]);
        
        //printf("parent: waiting for child (first) to finish...\n");
        waitpid(pid_first, &status, 0);
        //printf("parent: child (first) is finished.\n");
        
        //printf("parent: waiting for child (second) to finish...\n");
        waitpid(pid_second, &status, 0);
        //printf("parent: child (second) is finished.\n");
        
    }
    
    if(symbol_num == 2){ // checking format
        if((symbol[1] == (input_index-1)) || (symbol[0] + 1 == symbol[1])){
            printf("Missing '->' symbol or commands after '->'!! Please try again.\n");
            return;
        }
        
        // matches the command from the path list (args_first)
        for (int i = 1; i < path_index; i++) {
            temp1[0]=0;
            strcat(temp1, path_list[i]);
            
            if(temp1[strlen(temp1)] != '/'){
                strcat(temp1, "/");
            }
            
            strcat(temp1, input_list[0]);
            fpath = temp1;
        
            if(access(fpath, F_OK) == 0){
                path_match = 1;
                args_first[0]=fpath;
                break;
            } 
        }
        
        if(path_match == 0){
            printf("Binary file not found!! Please try again.\n");
            return;
        }
        else{
            path_match=0;
        }
        
        // matches the command from the path list (args_second)
        for (int i = 1; i < path_index; i++) {
            temp2[0]=0;
            strcat(temp2, path_list[i]);
            
            if(temp2[strlen(temp2)] != '/'){
                strcat(temp2, "/");
            }
            
            strcat(temp2, input_list[symbol[0]+1]);
            fpath = temp2;
        
            if(access(fpath, F_OK) == 0){
                path_match = 1;
                args_second[0]=fpath;
                break;
            }
        }
       
        if(path_match == 0){
            printf("Binary file not found!! Please try again.\n");
            return;
        }
        else{
            path_match=0;
        }
        
        // matches the command from the path list (args_third)
        for (int i = 1; i < path_index; i++) {
            temp3[0]=0;
            strcat(temp3, path_list[i]);
            
            if(temp3[strlen(temp3)] != '/'){
                strcat(temp3, "/");
            }
            
            strcat(temp3, input_list[symbol[1]+1]);
            fpath = temp3;
        
            if(access(fpath, F_OK) == 0){
                path_match = 1;
                args_third[0]=fpath;
                break;
            }
        }
       
        if(path_match == 0){
            printf("Binary file not found!! Please try again.\n");
            return;
        }
        
        for(int i = 1; i < symbol[0]; i++){     // store the rest input commands into list
            args_first[i] = input_list[i];
        }
        args_first[symbol[0]] = 0;
    
        for(int i = 1; i < (symbol[1] - symbol[0] - 1); i++){
            args_second[i]=input_list[symbol[0] + i + 1];
        }
        args_second[symbol[1] - symbol[0] - 1]=0;
    
        for(int i = 1; i < (input_index - symbol[1] -1); i++){
            args_third[i]=input_list[symbol[1] + i + 1];
        }
        args_third[input_index-symbol[1] - 1]=0;
        
        pipe(fd);             // create two pipes
        pipe(fd + 2);
        
        if ((pid_first = fork()) == 0) {  // create child to execute commands from user
           //printf("child (first): re-routing plumbing; STDOUT to pipe.\n");

            dup2(fd[1], 1);
            close(fd[0]);
            close(fd[1]);
            close(fd[2]);
            close(fd[3]);
            
            execve(args_first[0], args_first, envp);
            //printf("child (first): SHOULDN'T BE HERE.\n");
        }
        
        if ((pid_second = fork()) == 0) {
            //printf("child (second): re-routing plumbing; pipe to STDIN then STDOUT to pipe.\n");

            dup2(fd[0], 0);
            dup2(fd[3], 1);
            close(fd[0]);
            close(fd[1]);
            close(fd[2]);
            close(fd[3]);
            
            execve(args_second[0], args_second, envp);
            //printf("child (second): SHOULDN'T BE HERE.\n");
        }
        
        if ((pid_third = fork()) == 0) {
            //printf("child (third): pipe to STDIN.\n");

            dup2(fd[2], 0);
            close(fd[0]);
            close(fd[1]);
            close(fd[2]);
            close(fd[3]);
            
            execve(args_third[0], args_third, envp);
            //printf("child (third): SHOULDN'T BE HERE.\n");
        }
        
        close(fd[0]); // close pipes
        close(fd[1]);
        close(fd[2]);
        close(fd[3]);
        
        //printf("parent: waiting for child (first) to finish...\n");
        waitpid(pid_first, &status, 0);
        //printf("parent: child (first) is finished.\n");
        
        //printf("parent: waiting for child (second) to finish...\n");
        waitpid(pid_second, &status, 0);
        //printf("parent: child (second) is finished.\n");
        
        //printf("parent: waiting for child (third) to finish...\n");
        waitpid(pid_third, &status, 0);
        //printf("parent: child (third) is finished.\n");
    }

}


void input_handle(char* path_list[],char* input, int path_index) {
    char* input_list[MAX_INPUT_LIST];
    char input_src[MAX_INPUT_LIST][MAX_INPUT_LINE];
	char* temp;
	int index=0;
    int flag;

    // stores input into a list
	temp = strtok(input, " ");
    if(strcmp(temp,"OR") == 0){    // star with "OR"
            flag=1;
            temp = strtok(NULL, " ");
        }
        else if(strcmp(temp,"PP") == 0){  // star with "PP"
            flag=2;
            temp = strtok(NULL, " ");
        }
        else{
            flag=3;
        }
	while (temp != NULL && index < MAX_INPUT_LIST) {
        
		strncpy(input_src[index], temp, strlen(temp));
        input_src[index][strlen(temp)]='\0';
        input_list[index]=input_src[index];
        index++;
		temp = strtok(NULL, " ");
	}
    
    if(flag == 1){
        or_funct(path_list, input_list, index, path_index);
    }
    else if(flag == 2){
        pp_funct(path_list, input_list, index, path_index);
    }
    else if(flag == 3){
        normal(path_list, input_list, index, path_index);
    }
    
    
    return;

}

int main() {
	FILE* fp;
	char* path_list[MAX_PATH];
    char path_src[MAX_PATH][MAX_PATH_LENGTH];  // hold the string value for list
	char temp[MAX_PATH_LENGTH];
	int index = 0;

	if ((fp = fopen(RC_FILE, "r")) == NULL) {
		printf("Fail to open .360rc!!\n");
		exit(1);
	}

    // read prompt and path from .sh360rc file,
    // then stores in a list
    
	while (((fgets(temp, MAX_PATH_LENGTH, fp)) != NULL) && index < MAX_PATH) {
        
        if(temp[strlen(temp)-1]=='\n'){
            temp[strlen(temp)-1]='\0';
        }
        
		strncpy(path_src[index], temp, strlen(temp));
        path_src[index][strlen(temp)]='\0';
        path_list[index]=path_src[index];
        index++;
	}


	user_input(path_list, index);

	fclose(fp);

    return 0;

}
