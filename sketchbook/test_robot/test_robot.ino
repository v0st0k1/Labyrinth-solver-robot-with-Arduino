#include <Robot.h>

Robot r1(9,10,5,6,6,5,1,0);
void setup() {
  r1.wait_communication();
  r1.send_bluetooth("s\n");
  boolean fin = false;
  while(!fin){
    if(r1.algorithm()) fin=true;
  }
  r1.send_bluetooth("f\n");
}

void loop() {

}
