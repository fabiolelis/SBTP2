/*
 main.c
 SoftwareBasicoTP
 
 Created by Fabio Lelis on 25/09/16.
 Copyright Ã‚Â© 2016 Fabio Lelis. All rights reserved.
 */


#include "Decoder.h"


int readFile(char** input, char* file_in);
void split_line();
int check_label();
int check_comment(char* line);
int check_data();
void writeFile(char** output, int input_size, char* file_out, ILC* ilc,int ilcsize, ILC* ilcData,int ilcdatasize);
void savedata(char** output, int value, int line_number);

int current_fragment;


int main(int argc, const char * argv[]) {
    
	char* file_in = (char*)malloc(sizeof(char) * 20);
	stpcpy(file_in, argv[1]);
	char* file_out = (char*)malloc(sizeof(char) * 20);
	stpcpy(file_out, argv[2]);
	
    char** input = (char**)malloc(sizeof(char) * 256 * 30);
    char** output = (char**)malloc(sizeof(char) * 256 * 30);
    int input_size = 0;
    int line_number = 0;
    int ilcsize = 0;
	int ilcdatasize = 0;
    printf("started reading file \n");
    input_size = readFile(input, file_in);
    fflush(stdout);
    
    init_output(output);
    ILC* ilc = (ILC*)malloc(sizeof(ILC)*16);
    ILC* ilcData = (ILC*)malloc(sizeof(ILC)*16);


    int count_data = 0;
    while(line_number < (input_size)){
        fflush(stdout);
        
        char** line = (char**)malloc(sizeof(char) * 6 * 150);
        char* input_index = (char*) malloc(sizeof(char) * 32);
        strcpy(input_index, input[line_number]);
        split_line(line, input_index);
        
        int has_label = check_label(line[0]);
        int is_data = check_data(line[1+has_label]);
        count_data += is_data;
        line_number++;
	free(line);
    }
    printf("\n\n step 1 \n\n"); 
    
    line_number = 0;
    int label_idx = 0;
    int data_idx = 0;
    while(line_number < (input_size)){
        
        fflush(stdout);
        
        char** line = (char**)malloc(sizeof(char) * 6 * 150);
        char* input_index = (char*) malloc(sizeof(char) * 32);
        strcpy(input_index, input[line_number]);
        split_line(line, input_index);
        
        char* labelName = (char*)malloc(sizeof(char) * 32);

        
        int has_label = check_label(line[0]);
        int is_data = check_data(line[1+has_label]);
        
        if(has_label){

            fflush(stdout);
            ilc[label_idx].line = 2*line_number; /*CPUSim conta a partir do 1*/
            strcpy(labelName, line[0]);
            strcpy(labelName, replace(labelName, ":", ""));
            
            ilc[label_idx].label_name =  labelName;
	    printf("\n label %s in line  %X;\n", ilc[label_idx].label_name, ilc[label_idx].line);
            label_idx++;
			ilcsize++;
        }
						printf("passed\n");

        if(is_data){
            ilcData[data_idx].line = 2*data_idx + 2*(input_size - count_data);
            printf("\n\n DATA found %s in line %d", line[0], line_number);
            strcpy(labelName, line[0]);
            strcpy(labelName, replace(labelName, ":", ""));
            ilcData[data_idx].label_name =  labelName;
            
            ilcData[data_idx].size = atoi(line[2]);
            ilcData[data_idx].value = atoi(line[3]);
            label_idx++;

	     
	     printf("\n data %s in line %X with value ", ilcData[data_idx].label_name , ilcData[data_idx].line);
	      printf(" %X\n",  ilcData[data_idx].value);
           
            /*Salvar data em posiÃ§Ãµes de memÃ³ria apÃ³s o fim input_size ... */
            savedata(output, ilcData[data_idx].value, ilcData[data_idx].line);
            data_idx++;
			ilcdatasize++;
	   
        }
        fflush(stdout);
        
        line_number++;
    }
    fflush(stdout);
        printf("\n\n step 2 \n\n"); 
    
    line_number = 0;
    
    while(line_number < (input_size)){
        
        char** line = (char**)malloc(sizeof(char) * 6 * 150);
        char* input_index = (char*) malloc(sizeof(char) * 32);
        strcpy(input_index, input[line_number]);
        split_line(line, input_index);
        int has_label = check_label(line[0]);
        int is_data = check_data(line[1+has_label]);

        if(is_data){
            line_number++;
            continue;
        }

        /*Cada pedaÃƒÂ§o da instrucao em line[pos] agora*/
        decode(line, output, line_number, has_label, ilc, ilcData);
        line_number++;
        
        
    }
        printf("\n\n step 3 \n\n"); 
    writeFile(output, input_size, file_out, ilc, ilcsize, ilcData, ilcdatasize);
    printf("\nwrote output in %s \n", file_out);
    
    free(output);
    free(input);
    
    
    return 0;
}

void savedata(char** output, int value, int line_number)
{
    char* binary = (char*) malloc(sizeof(char) * 16);
    char* decoded1 = (char*) malloc(sizeof(char) * 8);
    char* decoded2 = (char*) malloc(sizeof(char) * 8);
    strcpy(binary, getDecimalToBinary3(value));
    splitInTwo(binary, decoded1, decoded2);
    strcpy(output[line_number], decoded1);
    strcpy(output[line_number + 1], decoded2);
    
}


int readFile(char* input[], char* file_in){
    FILE* fp;
    char* line = NULL;
    size_t len = 30;
    ssize_t read;
    int i = 0;

    fp = fopen(file_in , "r");
    if (fp == NULL)
        printf("Endereco invalido");
    
    while ((read = getline(&line, &len, fp)) !=-1) {
        
        input[i] = (char *) malloc(30*sizeof(char));
        strcpy(line, replace(line, "\t", ""));
        
        if(check_comment(line)){
            continue;
        }
        strcpy(input[i], line);
	free(line);
	line = NULL;
        printf("%d %s", i, input[i]);
        i++;
    }
    fclose(fp);
    return i ;
    
}


void split_line(char** line, char* input){
    line[0] = (char *) malloc(32*sizeof(char));
    line[0] = strtok(input," ");
    
    int pos = 0;
    while (line[pos] != NULL)
    {
        pos ++;
        line[pos] = (char *) malloc(32*sizeof(char));
        line[pos] = strtok(NULL," ");
    }
}


int check_label(char* line){
    
    if(line == NULL)
        return 0;
    
    switch (line[0]) {
        case '_':
            return 1;
            break;
        default:
            return 0;
            break;
    }
}



int check_comment(char* line){
    
    if(line == NULL)
        return 1;
    if(strlen(line) == 0)
        return 1;
    if(line[0] == '\n')
        return 1;
    
    
    switch (line[0]) {
        case ';':
            return 1;
            break;
        default:
            return 0;
            break;
    }
}


int check_data(char* line){
    if(line == NULL)
        return 0;
    
    if(strcmp(line, ".data") == 0){
        return 1;
    }
    else{
        return 0;
    }
}



void writeFile(char** output, int input_size, char* file_out, ILC* ilc,int ilcsize, ILC* ilcData,int ilcdatasize){
    FILE* fp;
    	printf(" \n\n Start of file write\n");
    fp = fopen(file_out , "w");
    if (fp == NULL)
        printf("Endereco invalido");
    
    /*fprintf(fp, "DEPTH = %d;\n", 256);
    fprintf(fp, "WIDTH = 8;\n");
    fprintf(fp, "ADDRESS_RADIX = HEX;\n");
    fprintf(fp, "DATA_RADIX = BIN;\n");
    fprintf(fp, "CONTENT\n");
    fprintf(fp, "BEGIN\n");*/
    
    int i = 0;
    for(i = 0; i < 256; i++){
        
        fprintf(fp, "%X : %s;\n", i, output[i]);
	printf("%X : %s;\n", i, output[i]);
    }
    
    fprintf(fp, "END;\n");
 	printf( "END;\n"); 
	
	for(i=0; i<ilcsize; i++)
	{
		fprintf(fp,"%s %d\n", ilc[i].label_name, ilc[i].line);
		printf("%s %d\n", ilc[i].label_name, ilc[i].line);
	}
	fprintf(fp,"data;\n");
	for(i=0; i<ilcdatasize; i++)
	{
		fprintf(fp,"%s %d %d %d\n", ilcData[i].label_name, ilcData[i].line, ilcData[i].value, ilcData[i].size);
		printf("%s %d %d %d\n", ilcData[i].label_name, ilcData[i].line, ilcData[i].value, ilcData[i].size);
	}
	

}