#include "tests.h"
#include "x86_desc.h"
#include "lib.h"
#include "drivers/rtc.h"

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
	const void* buf = (void*) &test_f;
	// rtc_write(2, buf, 4);
	// set_frequency(test_f);
	rtc_open(2);
}


// add more tests here

/* Checkpoint 2 tests */
/* Checkpoint 3 tests */
/* Checkpoint 4 tests */
/* Checkpoint 5 tests */


/* Test suite entry point */
void launch_tests(){
	TEST_OUTPUT("rtc test rate test", rtc_test_rate());
	// launch your tests here
}
