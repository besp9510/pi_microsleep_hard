// Error numbers:
#define ESETUP    1 // pi_microsleep_hard has already been setup
#define ENOPIVER  2 // Could not get PI board revision

// GPIO operation function prototypes:
int setup_microsleep_hard(void);
int microsleep_hard(unsigned int usec);