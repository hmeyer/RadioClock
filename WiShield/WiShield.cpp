
/******************************************************************************

  Filename:		WiShield.cpp
  Description:	WiShield library file for the WiShield 1.0

 ******************************************************************************

  TCP/IP stack and driver for the WiShield 1.0 wireless devices

  Copyright(c) 2009 Async Labs Inc. All rights reserved.

  This program is free software; you can redistribute it and/or modify it
  under the terms of version 2 of the GNU General Public License as
  published by the Free Software Foundation.

  This program is distributed in the hope that it will be useful, but WITHOUT
  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
  FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
  more details.

  You should have received a copy of the GNU General Public License along with
  this program; if not, write to the Free Software Foundation, Inc., 59
  Temple Place - Suite 330, Boston, MA  02111-1307, USA.

  Contact Information:
  <asynclabs@asynclabs.com>

   Author               Date        Comment
  ---------------------------------------------------------------
   AsyncLabs			05/01/2009	Initial version
   AsyncLabs			05/29/2009	Adding support for new library
   Rugged Circuits LLC		01/08/2012	Modified to work with Arduino 1.0

 *****************************************************************************/

extern "C" {
  #include "witypes.h"
  #include "global-conf.h"
  #include "network.h"
  #include "g2100.h"
  #include "spi.h"
  void stack_init(void);
  void stack_process(void);
}

#include "WiShield.h"
#include <avr/interrupt.h>
#include "display_timer.h"

Wifi wifi;

template <class T> const T& min (const T& a, const T& b) {
	  return !(b<a)?a:b;     // or: return !comp(b,a)?a:b; for version (2)
}

ISR(INT0_vect) {
	zg_isr();
}

inline void enableINT0(int mode) {
      EICRA = (EICRA & ~((1 << ISC00) | (1 << ISC01))) | (mode << ISC00);
      EIMSK |= (1 << INT0);
}


inline void WiFi_initPre() {
	zg_init();

#ifdef USE_DIG0_INTR
//	attachInterrupt(0, zg_isr, LOW);
	enableINT0(0);
#endif

#ifdef USE_DIG8_INTR
	// set digital pin 8 on Arduino
	// as ZG interrupt pin
	PCICR |= (1<<PCIE0);
	PCMSK0 |= (1<<PCINT0);
#endif

}
inline bool WiFi_initLoop() {
	if (zg_get_conn_state() != 1) {
		zg_drv_process();
		return true;
	}
	return false;
}
inline void WiFi_initPost() {
	stack_init();
}

void Wifi::reconnect(void) {
	m_connected = false;
}

void Wifi::restartStack(void) {
	stack_init();
}

PT_THREAD( Wifi::run(struct pt *pt, struct timer *timer)  ){
	PT_BEGIN(pt);
	m_connected = false;
	WiFi_initPre();
	PT_WAIT_WHILE(pt, WiFi_initLoop());
	WiFi_initPost();
	m_connected = true;
	
	while(m_connected) {
		stack_process();
		zg_drv_process();
		timer_set(timer, 2);
		PT_WAIT_UNTIL(pt, timer_expired(timer));
		//PT_WAIT_UNTIL(pt, 1);
		cycles++;
		//PT_YIELD(pt);
	}
	PT_END(pt);
}

#if defined USE_DIG8_INTR && !defined APP_WISERVER
// PCINT0 interrupt vector
ISR(PCINT0_vect){
	zg_isr();
}


#endif

