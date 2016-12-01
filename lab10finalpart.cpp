//Gable Galusha and John Walter
//ECE 3220 Lab 9 and 10

#include <iostream>
#include <string>
#include <stdio.h>
#include <sys/mman.h>
#include <stdlib.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>

//#include <locale>

using namespace std;

class message{
	protected:
		string msg;
		string morse[37];
		string letters;
	public:
		int leng;
		message();
		message(string);
		void fill();
		~message();
		virtual void printInfo();
};

//wrote this to construct string object, rather than writing
//it twice for each constructor
void message::fill(){
	morse[0]= ".-"; //a
	morse[1]= "-..."; //b
	morse[2]= "-.-."; //c
	morse[3]=	"-.."; //d
	morse[4]= "."; //e
	morse[5]= "..-."; //f
	morse[6]= "--."; //g
	morse[7]= "...."; //h
	morse[8]= ".."; //i
	morse[9]= ".---"; //j
	morse[10]= "-.-"; //k
	morse[11]= ".-.."; //l
	morse[12]= "--"; //m
	morse[13]= "-.";//n
	morse[14]= "---";//o
	morse[15]= ".--.";//p
	morse[16]= "--.-";//q
	morse[17]= ".-.";//r
	morse[18]= "...";//s
	morse[19]= "-";//t
	morse[20]= "..-";//u
	morse[21]= "...-";//v
	morse[22]= ".--";//w
	morse[23]= "-..-";//x
	morse[24]= "-.--";//y
	morse[25]= "--..";//z
	morse[26]= "/";//space character.. does morse code have spaces?
	morse[27]= "-----";//0
	morse[28]= ".----";//1
	morse[29]= "..---";//2
	morse[30]= "...--";//3
	morse[31]= "....-";//4
	morse[32]= ".....";//5
	morse[33]= "-....";//6
	morse[34]= "--...";//7
	morse[35]= "---..";//8
	morse[36]= "----.";//9
	letters=("abcdefghijklmnopqrstuvwxyz 0123456789");
}

//simple constructor, dominantly used
message::message(){
	cout << "Enter your message: ";
	//use getline just in case user enters string with spaces
	getline(cin, msg);
	fill();
	leng = msg.length();
}

//simple constructor
message::message(string message){
	msg=message;
	fill();
}

message::~message(){
//	cout << "Destructing message object..." << endl;
	cout << endl;
}

void message::printInfo(){
	cout << msg << endl;
}

class morseCodeMessage: public message {
	public:
		string all;
		morseCodeMessage();
		string *trans_msg;
		~morseCodeMessage();
		int index;
		void translate();
		void printInfo();
		int trans_LED();
};

//allocate memory for array of strings times length
morseCodeMessage::morseCodeMessage(){
	trans_msg=new string[msg.length()];
}

//simple print
void morseCodeMessage::printInfo(){
	cout << "\nInput message: ";
	message::printInfo();
	cout << "Morse Code Translation: ";
	for(int i=0; i<msg.length(); i++){
 		cout << *(trans_msg+i);

	}
	cout << endl;

}

void morseCodeMessage::translate(){
	for(int i=0; i<msg.length(); i++){
		//if upper case, convert to lower case
		if (isupper(msg[i])) msg[i]=tolower(msg[i]);
		//find character from msg in letters, then save index
		index=letters.find(msg[i]);
		//set value at specific index to translated msg at ith index
		*(trans_msg+i)=*(morse+index);
	}
}

// morse code to LED function
int morseCodeMessage::trans_LED(){
	int fd;		// for the file descriptor of the special file we need to open.
	unsigned long *BasePtr;		// base pointer, for the beginning of the memory page (mmap)
	unsigned long *PBDR, *PBDDR;	// pointers for port B DR/DDR

    fd = open("/dev/mem", O_RDWR|O_SYNC);	// open the special file /dem/mem
	if(fd == -1){
		printf("\n error\n");
	    return(-1);  // failed open
	}

	// We need to map Address 0x80840000 (beginning of the page)
	BasePtr = (unsigned long*)mmap(NULL,4096,PROT_READ|PROT_WRITE,MAP_SHARED,fd,0x80840000);
    if(BasePtr == MAP_FAILED){
    	printf("\n Unable to map memory space \n");
    	return(-2);
    }  // failed mmap

	// To access other registers in the page, we need to offset the base pointer to reach the
	// corresponding addresses. Those can be found in the board's manual.
	PBDR = BasePtr + 1;		// Address of port B DR is 0x80840004
    PBDDR = BasePtr + 5;	// Address of port B DDR is 0x80840014

    *PBDDR |= 0xE0;			// configures the 3 ports for the 3 LEDs - "activates" them so we can use them
	*PBDDR &= 0xFFFFFFFE;	// configures port B0 as input (first push button). You could use: &= ~0x01
	char c;
    // loop through each word in the sentence
	for(int i=0; i<leng; i++){
		cout << "DEBUG" << trans_msg[i] << endl;
		int len2=trans_msg[i].length();
        // loop through each character
		for(int j=0; j<len2; j++) {
		    sleep(1);
			c=trans_msg[i][j];
			cout << c << endl;
            // if the character is period
		    if (c=='.'){
                    // set the red LED
		    		*PBDR |=0x20;
		    		sleep(1); // keeps the LED lit
		    		*PBDR&=~0x20; // turn off the LED
		    }
            // if the character is a dash
		    else  if (c=='-'){
                    // set the yellow LED
		    		*PBDR |=0x40;
		    		sleep(2); // keeps the LED lit
		    		*PBDR&=~0x40; // turn off the LED
		    }
            // if the character is a space
		    else {
                    // set the green bit
		    		*PBDR |=0x80;
		    		sleep(2); // keeps the LED lit
		    		*PBDR&=~0x80; // turn off the LED
		    }
//
			}
	}
}

morseCodeMessage::~morseCodeMessage(){
	cout << "Destructing morseCodeMessage..." << endl;
	delete[] trans_msg;
}

int main(void) {
	int br=0;
	cout << "\n--------------------Translate--------------------" << endl;
	//loop while user says so
	do{
		cout << "Translate a message? (1-Yes, 2-No) ";
		cin >> br;
		cin.ignore();
		if(br==1){
			morseCodeMessage m;
			m.translate();
			m.printInfo();
			cout << endl << "\n----------------Translate to LED-----------------" << endl;
			m.trans_LED();
		}
	}while(br!=2);

	return 0;
}
