#include"../inc/primary_deps.h"
#include"../bt/bt.h"

 LOG_MODULE_REGISTER(bt,LOG_LEVEL_DBG);


int main()
{  

  if(!gpio_init()){
    return 1;
  }
    int err;
  err=bluetooth_init();
       if(err){
        return 1;
       }
       
    return 0;
}