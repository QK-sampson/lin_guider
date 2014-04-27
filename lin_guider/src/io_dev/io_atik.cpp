/*
 * io_nexstar.cpp
 *
 *  Created on: 01.10.2013
 *      Author: Rumen G.Bogdanovski
 *
 * This file is part of Lin_guider.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "io_atik.h"
#include "maindef.h"
#include "utils.h"
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>

namespace io_drv {

//----------------------------------------
//
// Derived.... atik
//
//----------------------------------------
cio_driver_atik::cio_driver_atik() {
	device_type = DT_ATIK;
	set_bit_map_template( device_bit_map_template[DT_ATIK-1] );
}


cio_driver_atik::~cio_driver_atik() {

}


int cio_driver_atik::open_device( void ) {

	if (DBG_VERBOSITY) log_i("%s", __FUNCTION__);
	
	int res = atik_open();
	if (res) return res;

	return 0;
}


int cio_driver_atik::close_device( void ) {
	return atik_close();
}


void cio_driver_atik::write_data( unsigned int dByte ) {
	int direction = NO_DIR;
	bool inverse_ra = false;
	bool inverse_dec = false;
	int ra_inc_pos = 0;
	int ra_dec_pos = 0;
	int dec_inc_pos = 0;
	int dec_dec_pos = 0;
	
	log_i("byte = %d", dByte);

	for( int i = 0;i < 8;i++ )
	{
		 if( bit_actions[i] == RA_INC_DIR ) {
			 ra_inc_pos = i;
			 log_i("==> RA_INC_DIR");
		 }
		 if( bit_actions[i] == RA_DEC_DIR ) {
		 	 ra_dec_pos = i;
		 	 log_i("==> RA_DEC_DIR");
		 }
		 if( bit_actions[i] == DEC_INC_DIR ) {
		 	 dec_inc_pos = i;
		 	 log_i("==> DE_INC_DIR");
		 }
		 if( bit_actions[i] == DEC_DEC_DIR ) {
		 	 dec_dec_pos = i;
		 	 log_i("==> DE_DEC_DIR");
		 }
	}

	inverse_ra = ra_inc_pos > ra_dec_pos;
	inverse_dec = dec_inc_pos > dec_dec_pos;

	u_char mapped = bit_map_encoder[ (u_char)dByte ];

	if( DBG_VERBOSITY )
		log_i("mapped = %d", (int)mapped);

	for( int i = 0;i < 8;i++ )
	{
		u_char mask = (1 << i);
		if( (mapped & mask) && bit_actions[i] != NO_DIR )
		{
			switch( bit_actions[i] )
			{
			case RA_INC_DIR:
				direction |= (inverse_ra ? GUIDE_WEST : GUIDE_EAST);
				log_i("RA_INC_DIR dir = %d", direction);
				break;
			case RA_DEC_DIR:
				direction |= (inverse_ra ? GUIDE_EAST : GUIDE_WEST);
				log_i("RA_DEC_DIR dir = %d", direction);
				break;
			case DEC_INC_DIR:
				direction |= (inverse_dec ? GUIDE_SOUTH : GUIDE_NORTH);
				log_i("DEC_INC_DIR dir = %d", direction);
				break;
			case DEC_DEC_DIR:
				direction |= (inverse_dec ? GUIDE_NORTH : GUIDE_SOUTH);
				break;
			default:
				;
			}
		}
	}

	if( !initialized )
		return;

	log_i("DIR= %d\n", direction);
	pthread_mutex_lock(&m_mutex);
	m_camera->setGuideRelays(direction);
	pthread_mutex_unlock(&m_mutex);
}


u_char cio_driver_atik::read_byte( void ) {

	if (DBG_VERBOSITY) log_i("%s", __FUNCTION__);
	return 0;
}

}
