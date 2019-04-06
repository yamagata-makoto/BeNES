#ifndef SEALAPU_H_DEFINED
#define SEALAPU_H_DEFINED

#ifdef __cplusplus
extern "C" {
#endif

void SEALAPUreset();
unsigned char SEALAPUread(unsigned address);
void SEALAPUwrite(unsigned address, unsigned char data);
void SEALAPUstatusChange();
void SEALAPUstop();

#ifdef __cplusplus
}
#endif

#endif
