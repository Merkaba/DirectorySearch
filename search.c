#include<sys/types.h>
#include<sys/stat.h>
#include<dirent.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<pthread.h>
#include<fcntl.h>

pthread_t threads[1000];
int counter = 0;

struct arg {
	char pattern[256];
	int file_index;
	char path[256];
};

void * find_match(void * arguments)
{
	struct arg args = *((struct arg * )(arguments));
	long long line_number = 0;
	FILE * ofp = fopen(args.path, "r");

	char line[1000];
	if(ofp == NULL)
		perror("cannot open file"), exit(1);
	while(fgets(line, 1000, ofp) != NULL)
	{
		line_number++;
		if(strstr(line, args.pattern) != NULL){
			printf("%d %s:%d: %s", args.file_index, args.path, line_number, line);
            printf("im not doing mutex locking!\n");
            printf("still not doing mutex locking!\n");
        }
	}
	pthread_exit(NULL);
}

void search(char * pattern, int file_index, char * path)
{
	DIR *dirpath;
	int thread;
	char dir[1000], name[1000];
	
	struct stat buf;
	struct dirent * result;
	
	stat(path, &buf);

	if(!S_ISREG(buf.st_mode) && !S_ISDIR(buf.st_mode))
		return;

	else if(S_ISREG(buf.st_mode))
	{
		struct arg * args = malloc(sizeof(struct arg));
		strcpy(args->pattern, pattern);
		args->file_index = file_index;
		strcpy(args->path, path);

		if(pthread_create(&threads[counter], NULL, &find_match, (void *) args))
        {
			printf("CREATE THREAD FAILURE\n");
            exit(1);
        }
		counter++;
		
	}	
	else if(S_ISDIR(buf.st_mode))
	{
		if((dirpath = opendir(path)) == NULL){
			printf("DIRECTORY OPEN FAILURE\n");
            exit(1);
        }
		while((result = readdir(dirpath)) != NULL)
		{
			strcpy(dir, path);
			strcpy(name, result->d_name);
			if(strcmp(name, ".") == 0 || strcmp(name, "..") == 0)
				continue;

			strcat(dir, "/");
			strcat(dir, name);
			search(pattern, file_index, dir); 
		}
		closedir(dirpath);
	}
				
}
int main(int argc, char * argv[])
{
	
	int i;
	if(argc < 3)
		perror("No file/path specified");
	for(i = 2;i < argc;i++)
		search(argv[1], i-1, argv[i]);
	pthread_exit(NULL);
	return 0;
}
