#include<stdio.h>
int g=-1;
void fn_0(){
  g=1;
}
void fn_1(){
  g=2;
}
void fn_2(){
  g=3;
}
void fn_3(){
  g=4;
}
void fn_4(){
  g=5;
}
void fn_5(int a){
  g=a*a;
}
static void (*vtable[6])(void) = { fn_0, fn_1, fn_2, fn_3, fn_4, fn_5 };
void dispatch(unsigned state) {
  if (state >= 6) abort();
  (*vtable[state])();
}
void dispatch2(unsigned state) {
  if (state >= 6) abort();
  (*vtable[state])();
}
void abort(){
  g=0;
}
int main(int argc, char **argv){
  int a = argv[1][0] - '0';
  dispatch(1);
  dispatch2(1);
  if (a > 0 && a < 10) 
    dispatch(a);
  return 0;
}
