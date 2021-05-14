// Error numbers:
#define ENOTSETUP 1 // pi_microsleep_hard has yet to be setup
#define ESETUP    2 // pi_microsleep_hard has already been setup
#define ENOPIVER  3 // Could not get PI board revision

// GPIO operation function prototypes:
int setup_microsleep_hard(void);
int microsleep_hard(unsigned int usec);