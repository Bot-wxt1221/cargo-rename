#include <algorithm>
#include <unistd.h>
#include <stdlib.h>
#include <iostream>
#include <sys/syscall.h>
#include <sys/types.h>
#include <cstring>
#include <linux/stat.h>
#include <sys/stat.h>
#include <stdio.h>
#include <fcntl.h>
#include <filesystem>
#include <sys/mman.h>
struct stat buf;
std::string str;
std::string find_lockfile(std::string tt){
  std::string tar="lockFile";
  std::string ans;
  for(int i=0;i+tar.size()<=tt.size();i++){
    std::string str(tt.c_str()+i,tar.size());
    if(str==tar){
      i+=tar.size();
      while(tt[i]!='='){
        i++;
      }
      i++;
      while(tt[i]!=';'){
        if(isprint(tt[i])&&tt[i]!=' '){
          ans+=tt[i];
        }
        i++;
      }
      i++;
      break;
    }
  }
  return ans;
}
int fdd;
void backup(char *path){
  std::string ans="cp ";
  ans+=path;
  ans+=" backup";
  system(ans.c_str());
}
void copyback(char *path){
  close(fdd);
  std::string ans="cp backup ";
  ans+=path;
  system(ans.c_str());
}
char buffer[10005];
std::string gethash(std::string pre,std::string tt,char *path){
  std::string temp="fetch-cargo-vendor-util create-vendor-staging ";
  temp+=pre;
  temp+='/';
  temp+=tt;
  temp+=" ../temp";
  std::cerr<<temp;
  //int ret=system(temp.c_str());
  int ret=0;
  if(ret!=0){
    copyback(path);
    system("rm -rf ../temp");
    std::cerr<<"error when calcing hash!";
    exit(-1);
  }
  temp="nix-hash --sri --type sha256 ../temp/ > hashout";
  system(temp.c_str());
  FILE* fd=fopen("hashout","r");
  fscanf(fd,"%s",::buffer);
  fclose(fd);
  temp = "rm -rf ../temp ";
  temp+=pre+'/'+tt;
  std::cerr<<temp;
  system(temp.c_str());
  return std::string(::buffer);
}
std::string getfilefolder(std::string a){
  while(a[a.size()-1]!='/'){
    a.pop_back();
  }
  a.pop_back();
  return a;
}
int main(int argc,char *argv[]){
  std::string file_folder;
  std::string ans1,ans2;
  backup(argv[1]);
  bool yes=0;
  int fd=open(argv[1],O_RDWR);
  fdd=fd;
  fstat(fd,&buf);
  file_folder=getfilefolder(std::string(argv[1]));
  char *ori=(char *)mmap(NULL,buf.st_size,PROT_READ|PROT_WRITE,MAP_SHARED,fd,0);
  char *ttt=ori;
  char *temp=(char *)malloc(buf.st_size+100);
  char *begg=temp;
  char *beg=temp;
  std::string ans;
  while(*ori!='\0'){
    *temp=*ori;
    temp++;
    ori++;
  }
  while(beg!=temp){
    if(memcmp(beg,"cargoLock.lockFile",strlen("cargoLock.lockFile"))==0){
      beg+=strlen("cargoLock.lockFile");
      str+="cargoLock.lockFile";
      while(*beg!=';'){ //73 is ;
        str=str+*beg;
        beg++;
      }
      str=str+*beg;
      beg++;
      if(yes==0){
        ans1+="useFetchCargoVendor = true;";
        ans1+="\ncargoHash = \"";
        ans2+="\";";
        ans2+="\n";
      }
      yes=1;
    }else if(memcmp(beg,"cargoLock",strlen("cargoLock"))==0){
      while(*beg!='{'){
        str=str+*beg;
        beg++;
      }
      int cnt=1;
      str=str+*beg;
      beg++;
      while(cnt>0){
        str=str+*beg;
        if(*beg=='{'){
          cnt++;
        }
        if(*beg=='}'){
          cnt--;
        }
        beg++;
      }
      while(*beg!=';'){
        str=str+*beg;
        beg++;
      }
      str=str+*beg;
      beg++;
      if(yes==0){
        ans1+="useFetchCargoVendor = true;";
        ans1+="\ncargoHash = \"";
        ans2+="\";";
        ans2+="\n";
      }
      yes=1;
    }else{
      if(yes){
        ans2+=*beg;
      }else{
        ans1+=*beg;
      }
      beg++;
    }
  }
  munmap(ttt,buf.st_size);
  close(fd);
  FILE *a=fopen(argv[1],"w");
  std::string ans3=find_lockfile(str);
  if(ans3==""){
    copyback(argv[1]);
    std::cerr<<"error when find lockFile";
    system("rm -rf ../temp");
    exit(-1);
  }
  ans=ans1+gethash(file_folder,ans3,argv[1])+ans2;
  fprintf(a,"%s",ans.c_str());
  fclose(a);
  std::string tt="nixfmt ";
  tt+=argv[1];
  int ret=system(tt.c_str());
  if(ret!=0){
    copyback(argv[1]);
    std::cerr<<"error when nixfmt";
    system("rm -rf temp");
    exit(-1);
  }
  free(begg);
  return 0;
}
