// Include C standard libraries:
#include <stdint.h> // C Standard integer types
#include <errno.h>  // C Standard for error conditions

// Include C POSIX libraries:
#include <sys/mman.h> // Memory management library

// Include header files:
#include "pi_microsleep_hard.h" // Macro definitions
#include "bcm.h"                // Physical address definitions
#include "map_peripheral.h"     // Map peripherals into virtual memory
#include "get_pi_version.h"     // Determines PI versions

// Virtual address of GPIO peripheral base:
volatile uintptr_t *arm_timer_virt_addr;

// Configured?
static int config_flag = 0;

// PWM controller register map:
struct arm_timer_reg_map {
    uint32_t load;    // Load
    uint32_t value;   // Value (read-only)
    uint32_t control; // Control
    uint32_t irqcntl; // IRQ Clear/Ack (write-only)
    uint32_t rawirq;  // Raw IRQ (read-only)
    uint32_t mskirq;  // Masked IRQ (read-only)
    uint32_t reload;  // Reload
    uint32_t prediv;  // Pre-divider
    uint32_t freecnt; // Free running counter
};

static volatile struct arm_timer_reg_map *arm_timer_reg; // Arm timer reg map

// Configure for GPIO by mapping peripheral into virtual address space
int setup_microsleep_hard(void) {
    int pi_version;

    int bcm_peri_base_phys_addr;
    int arm_timer_phys_addr;

    // Error out if microsleep_hard has already been setup. We cannot remap
    // registers into virtual memory again!
    if (config_flag) {
        return -ESETUP;
    }

    // Get PI version by parsing /proc/cpu_info:
    pi_version = get_pi_version__();

    // Set BCM base addresses according to the found PI version:
    if ((pi_version == 0) || (pi_version == 1)) {
        // Set BCM base addresses:
        bcm_peri_base_phys_addr = BCM2835_PERI_BASE_PHYS_ADDR;
    } else if ((pi_version == 2) || (pi_version == 3)) {
        // Set BCM base addresses:
        bcm_peri_base_phys_addr = BCM2837_PERI_BASE_PHYS_ADDR;
    } else if (pi_version == 4) {
        // Set BCM base addresses:
        bcm_peri_base_phys_addr = BCM2711_PERI_BASE_PHYS_ADDR;
    } else {
        return -ENOPIVER;
    }

    // Add in offset to find registers of value:
    // (Points to LOAD register)
    arm_timer_phys_addr = (bcm_peri_base_phys_addr + BCM_ARM_TIMER_BASE_OFFSET);

    // Map physical address into virtual address space to manipulate registers:
    arm_timer_virt_addr = map_peripheral__(arm_timer_phys_addr);

    if ((void *) arm_timer_virt_addr == MAP_FAILED) {
        return (int) MAP_FAILED;
    }

    // Set flag so only one mapping is done:
    config_flag = 1;

    // Point register map structure to virtual address of
    // ARM timer register base:
    arm_timer_reg = (struct arm_timer_reg_map*) ((char*)arm_timer_virt_addr + BCM_ARM_TIMER_REG_OFFSET);

    // Set the timer to free-running at speed = 1MHz.
    // 0xF9: the timer divide is base clock / (divide+1) so
    // base clock is 250MHz / 250 = 1MHz
    arm_timer_reg->control = 0x280;
    arm_timer_reg->prediv = 0xF9;

    return 0;
}

int microsleep_hard(unsigned int usec) {
    // Error out if microsleep_hard has not been setup yet; can't access
    // registers if physical memory has yet to be mapped into the virtual
    // space:
    if (!(config_flag)) {
        return -ENOTSETUP;
    }

    // Set the time for the timer to count down: 
    arm_timer_reg->load = usec;

    // This will clear the interrupt pending bit:
    arm_timer_reg->irqcntl = 0x0;

    // Spin while interrupt pending bit is cleared; it will be set once the
    // timer elapses:
    while (arm_timer_reg->rawirq == 0);

    return 0;
}
