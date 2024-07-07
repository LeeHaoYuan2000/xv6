#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

int find(char *Path, char *File_Name);
char* fmtname(char *path);

int main(int argc,char *argv[]){

    if(argc < 2){
        fprintf(2,"Please Input File Name You want to Find \n");
        exit(1);
    }
    else if(argc == 2){    
        if(strlen(argv[1]) > 256){
            fprintf(2,"The File Name is too Long \n");
            exit(1);
        }

        if(find(".",argv[1])){
            fprintf(2,"Find Fail \n");
            exit(1);
        }

        exit(0);
    }
    else if(argc > 3){
        fprintf(2,"The Find Can only accept [Path] [File_Name] \n");
        fprintf(2,"It don`t Support more Than 2 arguments now \n");
        exit(1);
    }


    if(strlen(argv[1]) > 256){
        fprintf(2,"The File Path is too Long \n");
         exit(1);
    }

    if(strlen(argv[2]) > 256){
        fprintf(2,"The File Name is too Long \n");
        exit(1);
    }

    if(find(argv[1], argv[2])){
        fprintf(2,"Find Fail \n");
        exit(1);
    }


    exit(0);
}

int find(char *Path, char *File_Name){
    char buf[512], *p;
    int fd;
    struct dirent de;
    struct stat st;
    
    if((fd = open(Path,0)) < 0){
        fprintf(2, "Find: cannot open %s\n", Path);
        return 1;
    }

    if(fstat(fd, &st) < 0){
        fprintf(2, "Find: cannot stat %s\n", Path);
        close(fd);
        return 1;
    }

    switch (st.type)
    {
    case T_DEVICE: break;

    case T_FILE:
    break;

    case T_DIR:

        if(strlen(Path) + 1 + DIRSIZ + 1 > sizeof(buf)){
            printf("Find: Path is too Long \n");
            break;
        }
        
        strcpy(buf, Path);
        p = buf + strlen(buf);
        *p++ ='/';
        while(read(fd, &de, sizeof(de)) == sizeof(de)){
            if(de.inum == 0){
                continue;
            }

            memmove(p,de.name,DIRSIZ);
            p[DIRSIZ] = 0;
            if(stat(buf, &st) < 0){
                printf("ls: cannot stat %s\n", buf);
                continue;
            }

            if(st.type == T_FILE){ 
                char *File;
                File = fmtname(buf);

                if( strcmp(File,File_Name) == 0){
                   // printf("%s \n",buf);
                    fprintf(1,"%s\n",buf);
                }

                continue;
            }
            else if(st.type == T_DIR){
                
                 if(strcmp(".",buf+strlen(buf) -1) == 0){ //to skip the "." and ".." file 
                    continue;
                }

                find(buf,File_Name);//this path is directory go to subdirectory to find the destinate files
            }
        }

    break;
    }

    close(fd);
    return 0;
}

char* fmtname(char *path) {
//printf("fmtname : %s \n",path);
  static char buf[DIRSIZ+1];
  char *p;

  // Find first character after last slash.
  for(p=path+strlen(path); p >= path && *p != '/'; p--)
    ;
  p++;

  // Return blank-padded name.
  if(strlen(p) >= DIRSIZ)
    return p;
  memmove(buf, p, strlen(p));
  memset(buf+strlen(p), '\0', DIRSIZ-strlen(p));
  return buf;
}