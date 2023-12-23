#include "tests.h"
#include "x86_desc.h"
#include "lib.h"
#include "rtc.h"
#include "keyboard.h"
#include "terminal.h"
#include "filesystem.h"

#define PASS 1
#define FAIL 0

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
/*division 0 exception*/
// int division_0_Test()
// {
// 	TEST_HEADER;		
// 	int tenghou=1;				
// 	int haoyangzhou=0;
// 	haoyangzhou=tenghou/0;		//test if it can divide 0
// 	haoyangzhou++;
// 	return FAIL;
// }
/*system call test*/
int syscall_test() 
{
	TEST_HEADER;
	asm volatile("int $0x80");		//test if it can call system call
	return PASS;
}
/* another exception test for invalid opcode*/
int invadlid_opcode_test()
{
	TEST_HEADER;
	asm volatile("ud2");	//test if it can call invalid opcode
	return FAIL;
}
/* rtc test function*/
int rtc_test()
{
	int i=0x28;
	int result = PASS;
	if ((idt[i].offset_15_00 == NULL) && (idt[i].offset_31_16 == NULL))
	{
		assertion_failure();
		result = FAIL;
	}
	printf("idt: %d\n 15_00: %u \n 31_16: %u \n", i, idt[i].offset_15_00, idt[i].offset_31_16);
	return PASS;
}
/* keyboard test function*/
int key_test()
{
	TEST_HEADER;
	int i=0x21;
	int result = PASS;
	if ((idt[i].offset_15_00 == NULL) && (idt[i].offset_31_16 == NULL))
	{
		assertion_failure();
		result = FAIL;
	}
	printf("idt: %d\n 15_00: %u \n 31_16: %u \n", i, idt[i].offset_15_00, idt[i].offset_31_16);
	return PASS;
}
/* page test function*/
int page_test(){
	TEST_HEADER;
	int result = PASS;

	// Test writing to and reading from memory
	int val;
	int* ptr = (int*)0x400000;	//0x400000 is the start point for kernel
	ptr[0] = 1;
	val = ptr[0];

	// Test accessing video memory and kernel memory
	unsigned char* test_video;
	unsigned char* test_video_ed;
	unsigned char* test_kernel;
	unsigned char* test_kernel_ed;
	unsigned char* loop;
	unsigned char tester;
	test_video = (unsigned char*)0xB8000;		//0xB8000 is the position for video memory
	test_video_ed = test_video+4*1024;			//video memory takes 4*1024=4kb memory
	test_kernel = (unsigned char*)0x400000;		//0x400000 is the start point for kernel
	test_kernel_ed = test_kernel+0x400000;		//0x800000 is the end point for kernel
	for(loop = test_video; loop<test_video_ed ; loop++){
		tester = *loop;
	}
	for(loop = test_kernel; loop<test_kernel_ed; loop++){
		tester = *loop;
	}

	// Return test result
	return result;
}

// add more tests here


/* Three page test function for boundary*/
int page_noaccess_test1(){
	TEST_HEADER;
	uint8_t i;
	uint8_t *not_accessible = (uint8_t*)0x800000;	//test if it can access 0x800001, which is a not accessible position
	i = *not_accessible;
	return FAIL;
}
/* Three page test function for boundary*/
int page_noaccess_test2(){
	TEST_HEADER;
	uint8_t i;
	uint8_t *not_accessible = (uint8_t*)0xB8000+4*1024;	//test if it can access 0x800001, which is a not accessible position
	i = *not_accessible;
	return FAIL;
}
/* Three page test function for boundary*/
int page_noaccess_test3(){
	TEST_HEADER;
	uint8_t i;
	uint8_t *not_accessible = (uint8_t*)0x0;	//test if it can access 0x800001, which is a not accessible position
	i = *not_accessible;
	return FAIL;
}

/* Checkpoint 2 tests */
int rtc_driver_test() {
	TEST_HEADER;

	int i, rtc_freq=2;
	while (rtc_freq <= 1024) {	//test if it can change the frequency
		write_rtc(0, &rtc_freq, 4);	//write the frequency
		for (i = 0; i < rtc_freq; i++) {
			read_rtc(0, NULL, 0);	//print the string at different frequencies
			printf("zhy ");
		}
		
		printf("\n");
		rtc_freq *= 2;
	}

	return PASS;
}

int32_t terminal_test1() {
	TEST_HEADER;
	uint8_t buf[128];
	// int i;
	// for(i=0;i<128;i++)
	// {
	// 	buf[i]='\0';
	// }
	terminal_open(0);
	int n=2;
	while (n--) {	//keep reading and writing
		terminal_read(0, buf, 128);	//read 15 bytes
		terminal_write(0, buf, 128);	//write 15 bytes

	}
	terminal_close(0);
	return 1;
}
int terminal_page_fault(int32_t write_nbytes) {
	TEST_HEADER;

	uint8_t buffer[12] = {'m','p','3','_','g','r','o','u','p','_','2','0'};
	terminal_init();
	terminal_write(0, buffer, write_nbytes);	//write 12 bytes, the string above. write_nbytes is 12 in testing 
	
	return PASS;
}

int read_file_name_test(uint8_t* file_name) {
	TEST_HEADER;
	dentry_t test;
	if (read_dentry_by_name(file_name,&test) == -1){
		printf("U file has gone.\n");
		return FAIL;
	}
	test.file_name[32] = '\0';
	printf("File name: %s\n",test.file_name);

	if(strncmp((int8_t*)test.file_name,(int8_t*)file_name,32)!=0)
		return FAIL;
	return PASS;
}


int read_data_test(uint8_t* filename) {
	TEST_HEADER;
	dentry_t test;
	char buffer[40000] = {'\0'};
	int i;
	int32_t length_bytes;
	read_dentry_by_name(filename,&test);
	length_bytes = read_data(test.inode_num,0,(uint8_t*)buffer,40000);
	for(i=0; i <length_bytes; i++){
		if(buffer[i] == '\0'){
			continue;
		}
		putc(buffer[i]);
		}
	return PASS;
}


// int read_dir_test(){
// 	TEST_HEADER;
// 	int i;
// 	int j;
// 	int name_length;
// 	uint8_t buf[33];
// 	buf[32] = '\0';
// 	uint8_t buf2[40000];
// 	dentry_t test_dentry;
// 	int32_t data_read;
// 	// 63 is the max directory number in filesystem
// 	for (i = 0; i < 63; i++){
// 		name_length = dread(buf, i, 32);

// 		if (name_length == -1) {
// 			break;
// 		}
// 		read_dentry_by_index(i, &test_dentry);
// 		printf("file name: ");
// 		for (j = 0; j < 32 - name_length; j++){
// 			printf(" ");
// 		}
// 		printf((int8_t*)buf);
// 		printf(", ");
// 		printf("file type: %d, ", test_dentry.file_type);
// 		data_read = read_data(test_dentry.inode_num, 0, buf2, 40000);
// 		printf("file size: %d", data_read);
// 		printf("\n");
// 	}
// 	return PASS;
// }

int op_test(uint8_t* filename){
	TEST_HEADER;
	if (fopen(filename) == -1)
		return FAIL;
	return PASS;
}

int cf_test(){
	TEST_HEADER;
	int fd = 0;
	if (fclose(fd) == 0)
		return PASS;
	return FAIL;
}

int rf_test(){
	TEST_HEADER;
	if (fread(0, NULL, 0) == 0)
		return PASS;
	return FAIL;
}

int wf_test(int32_t fd, void *buf, int32_t nbytes){
	TEST_HEADER;

	if (fwrite(fd, buf, nbytes) == -1)
		return PASS;
	return FAIL;
}




int od_test(uint8_t* filename){
	TEST_HEADER;
	if (dopen(filename) == -1)
		return FAIL;
	return PASS;
}

int cd_test(){
	TEST_HEADER;
	int fd = 0;
	if (dclose(fd) == 0)
		return PASS;
	return FAIL;
}

int wd_test(int32_t fd, void *buf, int32_t nbytes){
	TEST_HEADER;

	if (dwrite(fd, buf, nbytes) == -1)
		return PASS;
	return FAIL;
}


/* Checkpoint 3 tests */
/* Checkpoint 4 tests */
/* Checkpoint 5 tests */

/* Test suite entry point */
void launch_tests(){
	//putc('\0');
	//TEST_OUTPUT("idt_test", idt_test());
	//TEST_OUTPUT("division_0_Test", division_0_Test());
	//TEST_OUTPUT("rtc_test",rtc_test());
	//TEST_OUTPUT("page_test", page_test());
	//TEST_OUTPUT("key_test", key_test());
	//TEST_OUTPUT("syscall_test", syscall_test());
	//TEST_OUTPUT("page_noaccess_test1", page_noaccess_test1());
	//TEST_OUTPUT("page_noaccess_test2", page_noaccess_test2());
	//TEST_OUTPUT("page_noaccess_test3", page_noaccess_test3());
	// launch your tests here
	//TEST_OUTPUT("invadlid_opcode_test", invadlid_opcode_test());
	//TEST_OUTPUT("rtc_driver_test",rtc_driver_test());
	//TEST_OUTPUT("terminal test 1", terminal_test1());
	//TEST_OUTPUT("terminal_page_fault", terminal_page_fault(12));

	//TEST_OUTPUT("read_data_test", read_data_test((uint8_t*)"verylargetextwithverylongname.tx"));
	//TEST_OUTPUT("read_file_name_test", read_file_name_test((uint8_t*)"verylargetextwithverylongname.tx"));
	//TEST_OUTPUT("read_dir_test", read_dir_test());

// 	TEST_OUTPUT("op_test", op_test((uint8_t*)"frame0.txt"));
// 	TEST_OUTPUT("cf_test", cf_test());
// 	TEST_OUTPUT("rf_test", rf_test());
// 	TEST_OUTPUT("wf_test", wf_test(0, 0, 0));
// 	TEST_OUTPUT("od_test", od_test((uint8_t*)"frame0.txt"));
// 	TEST_OUTPUT("cd_test", cd_test());
// 	TEST_OUTPUT("wd_test", wd_test(0, 0, 0));
}
