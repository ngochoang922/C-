#include<iostream>
#include<string>
using namespace std;
void push(int stack[], int x); // th?m v?o
void pop(int stack[]); // l?y ra
void runStack(int stack[], char charInput);
void runStack(int stack[], string stringInput);
int maxStack = 5;
int viTriDinh = -1;

int main(int argc, char const *argv[])
{
    int stack[maxStack] = {0};
    runStack(stack,"EAS*Y**QUE***ST***I**ON");
//    push(stack,'E');
//    push(stack,'A');
//    push(stack,'S');
//  	pop(stack);
//  	push(stack,'Q');
//  	push(stack,'U');
//  	push(stack,'E');
//  	pop(stack);
//  	pop(stack);
//  	pop(stack);
//  	push(stack,'S');
//  	push(stack,'T');
//  	pop(stack);
//  	pop(stack);
//  	pop(stack);
//  	push(stack,'I');
//  	pop(stack);
//  	push(stack,'O');
//  	push(stack,'N');
//  	
  	
//  runStack(stack,'E');
//	runStack(stack,'A');
//	runStack(stack,'S');
//	runStack(stack,'*');
//	runStack(stack,'Y');
//	runStack(stack,'*');
//	runStack(stack,'*');
//	runStack(stack,'Q');
//	runStack(stack,'U');
//	runStack(stack,'E');
//	runStack(stack,'*');
//	runStack(stack,'*');
//	runStack(stack,'*');
//	runStack(stack,'S');
//	runStack(stack,'T');
//	runStack(stack,'*');
//	runStack(stack,'*');
//	runStack(stack,'*');
//	runStack(stack,'I');
//	runStack(stack,'*');
//	runStack(stack,'O');
//	runStack(stack,'N');
	  	
    return 0;
}
void push(int stack[], int x){
    if(viTriDinh >= maxStack - 1){
        cout<<"\nStack d? d?y";
        }else
		{
            viTriDinh++;
            stack[viTriDinh] = x;
        }
}
void pop(int stack[]){
    if (viTriDinh<0)
    {
        cout<<"\nStack d? tr?ng";
    }else{
        cout<<"\n"<<(char)stack[viTriDinh];
        viTriDinh--;
    }
    
}
void runStack(int stack[], char charInput){
	if(charInput == 42){
		pop(stack);
		}
		else{
			push(stack,charInput);
		}
	}
void runStack(int stack[], string stringInput){
	for(int i=0;i<stringInput.length();i++){
		if(stringInput[i]==42){
			pop(stack);
			}else{
				push(stack,stringInput[i]);
			}
		}
}
 




