/*
	RobotBoard firmware project
	
	Power switch / watchdog module: drives the charge pumps to control platfrom and
	application power switch on the battery module

	(c) 2017-2018 Pulu Robotics and other contributors
	Maintainer: Antti Alhonen <antti.alhonen@iki.fi>

	This program is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License version 2, as 
	published by the Free Software Foundation.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	GNU General Public License version 2 is supplied in file LICENSING.
*/

#pragma once
#include <stdint.h>


/*
Call these two functions in two critical parts of the program, between 2 and 10 kHz, between 35% and 65% duty cycle.

If these are not called alternatively, main power switch NFET gate charge depletes and power turns off.
*/

void pwrswitch_init();

void alive_platform_0();
void alive_platform_1();