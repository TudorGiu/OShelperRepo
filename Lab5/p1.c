#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>   
#include <sys/stat.h> 
#include <sys/types.h>

#include <sys/stat.h>

#include <fcntl.h> 

int search_dir(char *dir_name, char *searched_name)
{
    DIR* dir = opendir(dir_name);

    struct dirent* d;
    while((d = readdir(dir)) != NULL)
    {
        if(strcmp(d->d_name, searched_name) == 0)
        {
            char absolute_path[200];
            realpath(d->d_name, absolute_path);

            struct stat ino;

            char name[1024];
            snprintf(name, 1024, "%s/%s", dir_name, d->d_name);
          
            if(lstat(name, &ino) == -1)
            {
                printf("Error with lstat\n");
            }

            printf("Absolute path: %s\n", absolute_path);

            printf("Type: ");

            if (S_ISDIR(ino.st_mode))
            {
                printf("dir\n");
                return 0;
            }
            else 
                if (S_ISREG(ino.st_mode))
                {
                    printf ("file\n");
                    return 1;
                }
                else
                    if (S_ISLNK(ino.st_mode))
                    {
                        printf ("   link\n");
                        return 2;
                    }
                        else
                        {
                            printf("other\n");
                            return 3;
                        }
            closedir(dir);
        }
    }
    closedir(dir);
    return -1;
}


int copy_file(char *src, char *dst)
{
    
	int from, to, nr, nw;
	char buf[256];

	if ((from = open(src, O_RDONLY)) < 0) {
		printf("Error opening the source file");
		exit(2);
	}
	
	if ((to = creat(dst, 0666)) < 0) {
		printf("Error creating the destination file");
		exit(3);
	}

	while((nr = read(from, buf, sizeof(buf))) != 0) {
		if (nr < 0) {
			printf("Error reading from the source file");
			exit(4);
		}

		if ((nw = write(to, &buf, nr)) < 0) {
			printf("Error writing in destination file");
			exit(5);
		}
	}
	
	close(from);
	close(to);
	
	return 0;
}

int search_tree(char *dir_name, char *searched_name)
{

    int out;

    DIR* dir = opendir(dir_name);
    if(dir == 0){
        printf("Error with the dir");
        return -1;
    }

    out = search_dir(dir_name, searched_name);
    if(out == 1)
    {

        char srcc[1000] = "";
        strcat(srcc, dir_name);
        strcat(srcc, "/");
        strcat(srcc, searched_name);

        char new_file_name[1000] = "";
        strcat(new_file_name, dir_name);
        strcat(new_file_name, "/");
        strcat(new_file_name, searched_name);
        strcat(new_file_name, ".");

        char aux[] = "3";
        strcat(new_file_name, aux);

        copy_file(srcc, new_file_name);
    }

    struct dirent* d;
    while((d = readdir(dir)) != 0)
    {
        struct stat ino;

        char name[1024];
        snprintf(name, 1024, "%s/%s", dir_name, d->d_name);
          
        if(lstat(name, &ino) == -1)
        {
            printf("Error with lstat\n");
        }

        if(S_ISDIR(ino.st_mode) && strcmp(d->d_name, ".") != 0 &&  strcmp(d->d_name, "..") != 0 )
        {   
            int temp = search_tree(name, searched_name);

            if(temp != -1)
                out = temp;
        }
        
    }    

    return out;
}


int main(int argc, char* argv[])
{

    if(argc != 3)
        printf("Error: You must enter 3 parameters\n");
    else{
        printf("Name: %s   Starting directory: %s\n", argv[1], argv[2]);
            search_tree(argv[2], argv[1]);
            
    }
    return 0;
}