/* generated vector header file - do not edit */
#ifndef VECTOR_DATA_H
#define VECTOR_DATA_H
/* Number of interrupts allocated */
#ifndef VECTOR_DATA_IRQ_COUNT
#define VECTOR_DATA_IRQ_COUNT    (1)
#endif
/* ISR prototypes */
void fcu_frdyi_isr(void);

/* Vector table allocations */
#define VECTOR_NUMBER_FCU_FRDYI ((IRQn_Type) 2) /* FCU FRDYI (Flash ready interrupt) */
#define FCU_FRDYI_IRQn          ((IRQn_Type) 2) /* FCU FRDYI (Flash ready interrupt) */
#endif /* VECTOR_DATA_H */
