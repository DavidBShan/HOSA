#include "sys.h"  // Include the system header file, which likely contains necessary definitions for the microcontroller.

// Function to configure the NVIC (Nested Vectored Interrupt Controller)
void NVIC_Configuration(void)
{
    // Configure the NVIC priority group
    // This function sets the priority grouping of the NVIC.
    // In this case, it sets it to NVIC_PriorityGroup_2, which means 2 bits for priority and 2 bits for subpriority.
    // The specific values depend on the microcontroller and the NVIC implementation.
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
}
