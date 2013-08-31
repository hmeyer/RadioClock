
/******************************************************************************

  Filename:		WiShield.h
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

 *****************************************************************************/

#ifndef WISHIELD_H_
#define WISHIELD_H_

extern "C" {
	#include "config.h"
	#include "spi.h"
}

#include "pt.h"


class wifi {
	public:
	wifi():m_connected(false) {};
	PT_THREAD( run(struct pt *pt) );
	bool connected() { return m_connected; }
	private:
	bool m_connected;
};

extern wifi WiFi;

#endif /* WISHIELD_H_ */
