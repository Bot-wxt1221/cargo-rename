#include <iostream>
#include <cstdlib>
#include <fstream>
#include <istream>
#include <streambuf>

int main(){
  std::filebuf buf;
  buf.open("filewithcargoLock",std::ios::in);
  std::istream is(&buf);
  std::filebuf buf4;
  buf4.open("packagename",std::ios::in);
  std::istream is2(&buf4);
  std::filebuf buf2,buf3;
  buf2.open("error",std::ios::ate|std::ios::out);
  std::ostream err(&buf2);
  buf3.open("com",std::ios::ate|std::ios::out);
  std::ostream out(&buf3);
  std::string ans;
  while(is>>ans){
  std::string path=ans;
    ans="update-one ./"+ans;
    std::cerr<<ans;
    int ret=system(ans.c_str());
    if(ret!=0){
      std::cerr<<ans<<std::endl;
      err<<ans<<std::endl;
      return -1;
    }else{
      out<<ans<<std::endl;
      path="git add "+path;
      system(path.c_str());
      std::string tt2;
      is2>>tt2;
      std::string tt="git commit -m ";
      tt+='"';
      tt+=tt2;
      tt+=": use new cargo fetcher";
      tt+='"';
      system(tt.c_str());
    }
  }
}
