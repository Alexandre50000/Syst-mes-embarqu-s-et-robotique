#define VITESSE_AVANCE_CONTROL   3
#define VITESSE_ROTATION_CONTROL 1

//semaphore
static BSEMAPHORE_DECL(control_out_sem, TRUE);

void control_init();