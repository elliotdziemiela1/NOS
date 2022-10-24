#include "tests.h"
#include "x86_desc.h"
#include "lib.h"
#include "drivers/rtc.h"
#include "drivers/terminal.h"
#include "filesystem.h"

#define PASS 1
#define FAIL 0
#define BUFFER_SIZE 128 // size of terminal buffer
#define PATH_LENGTH 10 // length of path string displayed at left of terminal screen

/* format these macros as you see fit */
#define TEST_HEADER 	\
	printf("[TEST %s] Running %s at %s:%d\n", __FUNCTION__, __FUNCTION__, __FILE__, __LINE__)
#define TEST_OUTPUT(name, result)	\
	printf("[TEST %s] Result = %s\n", name, (result) ? "PASS" : "FAIL");

static inline void assertion_failure(){
	/* Use exception #15 for assertions, otherwise
	   reserved by Intel */
	asm volatile("int $15");
}


/* Checkpoint 1 tests */

/* IDT Test - Example
 * 
 * Asserts that first 10 IDT entries are not NULL
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 * Coverage: Load IDT, IDT definition
 * Files: x86_desc.h/S
 */

int idt_test(){
	TEST_HEADER;

	int i;
	int result = PASS;
	for (i = 0; i < 10; ++i){
		if ((idt[i].offset_15_00 == NULL) && 
			(idt[i].offset_31_16 == NULL)){
			assertion_failure();
			result = FAIL;
		}
	}

	return result;
}

/* Divide by Zero Test 
 * 
 * Checks if IDT works with divide by zero exception
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 * Coverage: IDT Exception Handling
 * Files: x86_desc.h
 */
int divide_zero(){
	TEST_HEADER;

	int result = FAIL;
	asm volatile("int $0");
	result = PASS;
	return result;
}
/* Page fault Test 
 * 
 * Checks if page faulting works
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 * Coverage: IDT Exception Handling
 * Files: x86_desc.h
 */
int page_fault(){
	TEST_HEADER;

	int result = FAIL;
	asm volatile("int $14");
	result = PASS;
	return result;
}
/* RTC Test 
 * 
 * Checks if RTC works
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 * Coverage: RTC handling
 * Files: 
 */
int rtc_test(){
	TEST_HEADER;

	int result = FAIL;
	asm volatile("int $40");
	result = PASS;
	return result;
}

int terminal_test(){
	TEST_HEADER;

	int result = PASS;
	terminal_open();
    while (1){
        terminal_read();
		char test[5] = {'f','i','l','e','\0'};
		terminal_write(test, 5);
    }
	return result;
}
/* RTC Test 
 * 
 * Checks if RTC works
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 * Coverage: RTC handling
 * Files: 
 */
int rtc_test_rate(){
	TEST_HEADER;
	int32_t test_f = 2;
	// rtc_write(2, buf, 4);
	// set_frequency(test_f);
	rtc_open(2);
}


// add more tests here
/* Checkpoint 2 tests */

int test_read_data(){
	TEST_HEADER;
	dentry_t cur_file;
	read_dentry_by_name((const uint8_t*) "frame0.txt", (dentry_t*) &cur_file);
	uint32_t cur_inode = cur_file.inode_num;
	uint32_t length = 264;
	uint8_t buf[264];
	uint32_t offset = 0;

	int32_t bytes_read = read_data(cur_inode, offset, buf, length);

	int i;

	clear();
	setCursor(0, 0);

	printf("bytes read: %d \n", bytes_read);

	for(i = 0; i < bytes_read; i++){
		printfBetter("%c", buf[i]);
	}

	//printf("%d", bytes_read);

	return 1;
}

int test_read_directory(){
	TEST_HEADER;
	int32_t fd = 0;
	int* buf = 0;
	int32_t nbytes = 0;

	clear();
	setCursor(0, 0);

	int32_t success = read_directory(fd, (void*) buf, nbytes);

	return success;
}

int test_open_files(uint8_t cmd){
	TEST_HEADER;
	int8_t* file_names[6] = {"frame0.txt", "frame1.txt", "hello", "pingpong", "testprint", "verylargetextwithverylongname.txt"};
	open_file((const uint8_t*) file_names[0]);
	open_file((const uint8_t*) file_names[1]);
	open_file((const uint8_t*) file_names[2]);
	open_file((const uint8_t*) file_names[3]);
	open_file((const uint8_t*) file_names[4]);
	open_file((const uint8_t*) file_names[6]);
	dentry_t dentry;

	read_dentry_by_name(file_names[cmd], (dentry_t*) &dentry);

	int32_t inode_num = dentry.inode_num;
	int32_t file_length = get_file_length(inode_num);
	int8_t buf[37000];

	int32_t bytes_read = read_file(inode_num, buf, file_length);
	clear();
	setCursor(0, 0);
	printf("file read: %s \n", file_names[cmd]);
	printf("bytes read: %d \n", bytes_read);
	int i;
	for(i = 0; i < bytes_read; i++){
		printfBetter("%c", buf[i]);
	}

	if(bytes_read != file_length){
		return 0;
	}

	

	return 1;
}

int rtc_1_test(){
    TEST_HEADER;
    int freq_pass;
    int buf_size;
    rtc_open(2);
    clear();
    setCursor(0,0);
    for(freq_pass = 2; freq_pass <= 1024; freq_pass *= 2){
        rtc_write(2, (void*)&freq_pass, 4);
        buf_size = freq_pass;
        while(buf_size > 0){
            rtc_read(2, (void*)&freq_pass, 4);
            putc('1');
            buf_size--;
            if((freq_pass - buf_size) % 80 == 0)
                putc('\n');
        }
        clear();
        setCursor(0,0);
    }
}
/* Checkpoint 3 tests */
/* Checkpoint 4 tests */
/* Checkpoint 5 tests */


/* Test suite entry point */
void launch_tests(){
	// Elliot: if you want to clear the screen, call lib::clear(). if you want to then reset the 
	// cursor to the top left of the screen to start printing from there, call lib::setCursor(0,0)
	// If you want to print to the screen, i recommend the functions I made printfBetter, putcBetter, 
	// end putsBetter because they dont wrap around once the cursor hits the edge (they make a new line)
	
	//TEST_OUTPUT("rtc test rate test", rtc_test_rate());
	// launch your tests here

	//rtc_1_test();
	test_open_files(0);
	//test_read_directory();
	return;
}
