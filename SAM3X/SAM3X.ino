
#include <ARDK.h>
//
//using namespace ARDK::GPIO;
//using namespace ARDK::Board;


// the setup function runs once when you press reset or power the board

//void testPIOCMem() {
//  PIOBCtrl *piob = PIOBCtrl::instance();
//
//  Serial.print("WPMR   :");
//  Serial.print((uintptr_t)&piob->PIO_WPMR, HEX);
//  Serial.print("==");
//  Serial.print((uintptr_t)&PIOB->PIO_WPMR, HEX);
//  Serial.print(" | ");
//  Serial.print((uintptr_t)&piob->PIO_WPMR - (uintptr_t)piob, HEX);
//  Serial.print("==");
//  Serial.println((uintptr_t)&PIOB->PIO_WPMR - (uintptr_t)PIOB, HEX);
//
//  Serial.print("WPSR :");
//  Serial.print((uintptr_t)&piob->PIO_WPSR, HEX);
//  Serial.print("==");
//  Serial.print((uintptr_t)&PIOB->PIO_WPSR, HEX);
//  Serial.print(" | ");
//  Serial.print((uintptr_t)&piob->PIO_WPSR - (uintptr_t)piob, HEX);
//  Serial.print("==");
//  Serial.println((uintptr_t)&PIOB->PIO_WPSR - (uintptr_t)PIOB, HEX);
//
//  Serial.print("FRLHSR   :");
//  Serial.print((uintptr_t)&piob->PIO_FRLHSR, HEX);
//  Serial.print("==");
//  Serial.print((uintptr_t)&PIOB->PIO_FRLHSR, HEX);
//  Serial.print(" | ");
//  Serial.print((uintptr_t)&piob->PIO_FRLHSR - (uintptr_t)piob, HEX);
//  Serial.print("==");
//  Serial.println((uintptr_t)&PIOB->PIO_FRLHSR - (uintptr_t)PIOB, HEX);
//
//  Serial.print("LOCKSR   :");
//  Serial.print((uintptr_t)&piob->PIO_LOCKSR, HEX);
//  Serial.print("==");
//  Serial.print((uintptr_t)&PIOB->PIO_LOCKSR, HEX);
//  Serial.print(" | ");
//  Serial.print((uintptr_t)&piob->PIO_LOCKSR - (uintptr_t)piob, HEX);
//  Serial.print("==");
//  Serial.println((uintptr_t)&PIOB->PIO_LOCKSR - (uintptr_t)PIOB, HEX);
//
//
//  Serial.println("-------");
//}
//
//void testPMCMem() {
//  SAM3XPMC *mpmc = SAM3XPMC::instance();
//  Serial.println("-------");
//  Serial.print("PMC_SCER   "); Serial.println((uintptr_t)&mpmc->PMC_SCER - (uintptr_t)mpmc, HEX);
//  Serial.print("PMC_SCDR   "); Serial.println((uintptr_t)&mpmc->PMC_SCDR - (uintptr_t)mpmc, HEX);
//  Serial.print("PMC_SCSR   "); Serial.println((uintptr_t)&mpmc->PMC_SCSR - (uintptr_t)mpmc, HEX);
//  Serial.print("PMC_PCER0  "); Serial.println((uintptr_t)&mpmc->PMC_PCER0 - (uintptr_t)mpmc, HEX);
//  Serial.print("PMC_PCDR0  "); Serial.println((uintptr_t)&mpmc->PMC_PCDR0 - (uintptr_t)mpmc, HEX);
//  Serial.print("PMC_PCSR0  "); Serial.println((uintptr_t)&mpmc->PMC_PCSR0 - (uintptr_t)mpmc, HEX);
//  Serial.print("CKGR_UCKR  "); Serial.println((uintptr_t)&mpmc->CKGR_UCKR - (uintptr_t)mpmc, HEX);
//  Serial.print("CKGR_MOR   "); Serial.println((uintptr_t)&mpmc->CKGR_MOR - (uintptr_t)mpmc, HEX);
//  Serial.print("CKGR_MCFR  "); Serial.println((uintptr_t)&mpmc->CKGR_MCFR - (uintptr_t)mpmc, HEX);
//  Serial.print("CKGR_PLLAR "); Serial.println((uintptr_t)&mpmc->CKGR_PLLAR - (uintptr_t)mpmc, HEX);
//  Serial.print("PMC_MCKR   "); Serial.println((uintptr_t)&mpmc->PMC_MCKR - (uintptr_t)mpmc, HEX);
//  Serial.print("PMC_USB    "); Serial.println((uintptr_t)&mpmc->PMC_USB - (uintptr_t)mpmc, HEX);
//  Serial.print("PMC_PCK0   "); Serial.println((uintptr_t)&mpmc->PMC_PCK0 - (uintptr_t)mpmc, HEX);
//  Serial.print("PMC_PCK1   "); Serial.println((uintptr_t)&mpmc->PMC_PCK1 - (uintptr_t)mpmc, HEX);
//  Serial.print("PMC_PCK2   "); Serial.println((uintptr_t)&mpmc->PMC_PCK2 - (uintptr_t)mpmc, HEX);
//  Serial.print("PMC_IER    "); Serial.println((uintptr_t)&mpmc->PMC_IER - (uintptr_t)mpmc, HEX);
//  Serial.print("PMC_IDR    "); Serial.println((uintptr_t)&mpmc->PMC_IDR - (uintptr_t)mpmc, HEX);
//  Serial.print("PMC_SR     "); Serial.println((uintptr_t)&mpmc->PMC_SR - (uintptr_t)mpmc, HEX);
//  Serial.print("PMC_IMR    "); Serial.println((uintptr_t)&mpmc->PMC_IMR - (uintptr_t)mpmc, HEX);
//  Serial.print("PMC_FSMR   "); Serial.println((uintptr_t)&mpmc->PMC_FSMR - (uintptr_t)mpmc, HEX);
//  Serial.print("PMC_FSPR   "); Serial.println((uintptr_t)&mpmc->PMC_FSPR - (uintptr_t)mpmc, HEX);
//  Serial.print("PMC_FOCR   "); Serial.println((uintptr_t)&mpmc->PMC_FOCR - (uintptr_t)mpmc, HEX);
//  Serial.print("PMC_WPMR   "); Serial.println((uintptr_t)&mpmc->PMC_WPMR - (uintptr_t)mpmc, HEX);
//  Serial.print("PMC_WPSR   "); Serial.println((uintptr_t)&mpmc->PMC_WPSR - (uintptr_t)mpmc, HEX);
//  Serial.print("PMC_PCER1  "); Serial.println((uintptr_t)&mpmc->PMC_PCER1 - (uintptr_t)mpmc, HEX);
//  Serial.print("PMC_PCDR1  "); Serial.println((uintptr_t)&mpmc->PMC_PCDR1 - (uintptr_t)mpmc, HEX);
//  Serial.print("PMC_PCSR1  "); Serial.println((uintptr_t)&mpmc->PMC_PCSR1 - (uintptr_t)mpmc, HEX);
//  Serial.print("PMC_PCR    "); Serial.println((uintptr_t)&mpmc->PMC_PCR - (uintptr_t)mpmc, HEX);
//}

//  typedef BD02 TestPin;

void setup() {
  Serial.begin(9600);
//  TestPin::mode(OutputPushPull);
//  pinMode(13, OUTPUT);
}

void loop() {
//  uint8_t v = 0;
//  PIOBCtrl *pioc = PIOBCtrl::instance();
  while (1) {
//    TestPin::toggle();
//    delay(500);
    //    TestPin::write(v);
//    TestPin::low();
//    TestPin::high();
    //    v = 1 - v;
    //    digitalWrite(13, 0);
    //    digitalWrite(13, 1);
  }
}

