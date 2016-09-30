#ifndef __PROTO__H /* riad */
#define __PROTO__H /* riad */

void *accept(), *append(), *bytesToInt(), *closeNADF();
void *creatNADF(), *match(), *print(), *println();
void *str2h(), *strToInt(), *writeNADF();

/* Hooman: I added these */
void *bitand(),*bitor(),*bitxor();

// A few additional functions to manipulate register contents easily
void *rX(), *rH(), *rL();
// Something to display an integer/byte/word in hexa
void *int2h();
void *byte2h();
void *word2h();

// Function sending a key/text to bochs
void *sendscancode();
void *sendtxt();
void *sendln();

// Function getting a goat name to exec if there is still one
void *getnextgoat();

// Getting different types of strings/ints from memory
void *readASCIIZ();
void *readASCIIdollar();
void *readmemword();

// Conversion of a bytestring into an _unsigned_ integer
void *strToU();

// Name and extension processing
void *getnameoffile();
void *getextoffile();
void *priority();

/* for feedback: */ // Not tested yet!
void *cancel(), *toggle_watch_ins();
void *mem_attach_marker(), *mem_detach_marker();
void *reg_attach_marker(), *reg_detach_marker();

// The memory watch functions
void *start_watch_mem();
void *stop_watch_mem();

// Some switches
void *start_watch_int(), *stop_watch_int();
void *start_watch_func();
void *start_watch_io(), *stop_watch_io();
void *start_watch_irq(), *stop_watch_irq();
void *start_watch_brkpt(), *stop_watch_brkpt();

// To set registers (not tested)
void *seteax(), *setebx(), *setecx(), *setedx(),
   *setesi(), *setedi(), *setebp(), *setesp(), *seteip();
void *setcs(), *setds(), *setes(), *setss(), *setfs(), *setgs();
void *setcf(), *setpf(), *setaf(), *setzf(), *setsf(),
   *settf(), *setif(), *setdf(), *setof(), *setiopl(),
   *setnt(), *setrf(), *setvm(), *setac();

// Exit
void *stop_program();

// To set the audit breakpoint
void *setbrkpt();

// To extract a file or sector from the emulated system to the real disk
void *extractfile();
void *extractHDMBR();
void *extractHDPBS();
void *extractFDBS();
void *extractsec();

// To check wether we're in a boot virus triaging session
void *bootTriage();
void *replaceFloppyA();

#endif
