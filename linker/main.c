#define _GNU_SOURCE
#include <sys/types.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct ilc_struct ILC;

int readFile(char** input, char* file_in);
int check_comment(char* line);
char *replace(const char *src, const char *from, const char *to);


char* file_in = "output2.mif";

struct ilc_struct{
    int line;
    int size;
    int value;
    char* label_name;
};

int main(int argc, const char * argv[]) {
	char** input = (char**)malloc(sizeof(char) * 300 * 15);
	char** codes = (char**)malloc(sizeof(char) * 300 * 15);
	ILC* ilc = (ILC*)malloc(sizeof(ILC)*16*20);
	int i;
	int ilccounter = 0;
	int step1 = 1;
	int step2 = 1;
	/*precisa de um loop para cada entrada*/
	int input_size;
    input_size = readFile(input, file_in);
	printf("end read\n");
	for (i=0; i<input_size; i++)
	{

		if(strcmp(input[i],"END;\n") != 0 && step1)
		{
			printf("code written %s\n",input[i]);
			codes[i] = (char *) malloc(30*sizeof(char));
			strcpy(codes[i],input[i]);
			printf("code saved %d %s\n",i,codes[i]);
		}
		else
		{
			step1 = 0;
			if(strcmp(input[i],"END;\n") != 0 && strcmp(input[i],"data;\n") != 0 && step2 )
			{
				ilc[ilccounter].label_name = strtok(input[i]," ");
				ilc[ilccounter].line = atoi(strtok(NULL," "));
				printf("label: %s %d\n",ilc[ilccounter].label_name,ilc[ilccounter].line);
				ilccounter++;
			}
			else 
			{
				if(strcmp(input[i],"data;\n") == 0) step2 = 0;
				if(strcmp(input[i],"END;\n") != 0 && strcmp(input[i],"data;\n") != 0){
				ilc[ilccounter].label_name = strtok(input[i]," ");
				ilc[ilccounter].line = atoi(strtok(NULL," "));
				ilc[ilccounter].value = atoi(strtok(NULL," "));
				ilc[ilccounter].size = atoi(strtok(NULL," "));

				

				printf("data: %s %d %d %d\n",ilc[ilccounter].label_name,ilc[ilccounter].line,ilc[ilccounter].value, ilc[ilccounter].size);}
			}
		}
	 }


	return 0;
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
        printf("read %d %s", i, input[i]);
        i++;
    }
    fclose(fp);
		printf("end read\n");


    return i ;
    
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

char *replace(const char *src, const char *from, const char *to)
{
    size_t size    = strlen(src) + 1;
    size_t fromlen = strlen(from);
    size_t tolen   = strlen(to);
    char *value = malloc(size);
    char *dst = value;
    if ( value != NULL )
    {
        for ( ;; )
        {
            const char *match = strstr(src, from);
            if ( match != NULL )
            {
                size_t count = match - src;
                char *temp;
                size += tolen - fromlen;
                temp = malloc( size);
                if ( temp == NULL )
                {
                    free(value);
                    return NULL;
                }
                dst = temp + (dst - value);
                value = temp;
                memmove(dst, src, count);
                src += count;
                dst += count;
                memmove(dst, to, tolen);
                src += fromlen;
                dst += tolen;
            }
            else
            {
                strcpy(dst, src);
                break;
            }
        }
    }
    return value;
}