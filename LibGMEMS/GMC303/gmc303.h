/*
 *
 ****************************************************************************
 * Copyright (C) 2018 GlobalMEMS, Inc. <www.globalmems.com>
 * All rights reserved.
 *
 * File : gmc303.h
 *
 * Date : 2018/04/11
 *
 * Usage: Sensor Driver file for GMC303 sensor
 *
 ****************************************************************************
 *
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 **************************************************************************/

/*! @file gmc303.h
 *  @brief  GMC303 Sensor Driver Header File
 *  @author Joseph FC Tseng
 */

#ifndef __GMC303_H__
#define __GMC303_H__

#include "bus_support.h"
#include "type_support.h"

#define GMC303_7BIT_I2C_ADDR		0x0C
#define GMC303_8BIT_I2C_ADDR		((GMC303_7BIT_I2C_ADDR)<<1)
#define GMC303_RAW_DATA_SENSITIVITY     (10.0/6)  //raw data one code = 0.6 uT

#define GMC303_REG_CMPID 	  0x00
#define GMC303_REG_DEVID	  0x01
#define GMC303_REG_INFO1 	  0x02
#define GMC303_REG_INFO2 	  0x03
#define GMC303_REG_ST1		  0x10
#define GMC303_REG_HXL		  0x11
#define GMC303_REG_HXH	 	  0x12
#define GMC303_REG_HYL		  0x13
#define GMC303_REG_HYH	 	  0x14
#define GMC303_REG_HZL		  0x15
#define GMC303_REG_HZH	 	  0x16
#define GMC303_REG_TMPS		  0x17
#define GMC303_REG_ST2		  0x18
#define GMC303_REG_CNTL1	  0x30
#define GMC303_REG_CNTL2 	  0x31
#define GMC303_REG_CNTL3	  0x32
#define GMC303_REG_TS1		  0x33
#define GMC303_REG_ASAX	 	  0x60
#define GMC303_REG_ASAY	 	  0x61
#define GMC303_REG_ASAZ	 	  0x62

/* PID */
#define GMC303_PID__REG                GMC303_REG_CMPID
/* Soft Rest bit */
#define GMC303_RST__REG		       GMC303_REG_CNTL3
#define GMC303_RST__MSK		       0x01
#define GMC303_RST__POS		       0
/* Operation mode bits */
#define GMC303_OP_MODE__REG	       GMC303_REG_CNTL2
#define GMC303_OP_MODE__MSK	       0x1F
#define GMC303_OP_MODE__POS	       0
/* Data read start register */
#define GMC303_DATA_READ_START__REG    GMC303_REG_HXL
/*ASA XYZ start register */
#define GMC303_ASA_XYZ_START__REG      GMC303_REG_ASAX


typedef enum {GMC303_OP_MODE_POWER_DOWN = 0x00,
	      GMC303_OP_MODE_SINGLE_MEASUREMENT = 0x01,
	      GMC303_OP_MODE_CM_10HZ = 0x02,
	      GMC303_OP_MODE_CM_20HZ = 0x04,
	      GMC303_OP_MODE_CM_50HZ = 0x06,
	      GMC303_OP_MODE_CM_100HZ = 0x08,
	      GMC303_OP_MODE_SELF_TEST = 0x10,
	      GMC303_OP_MODE_FUSE_ROM_ACCESS = 0x1F} GMC303_OP_MODE_T;

#define GMC303_GET_BITSLICE(regvar, bitname)	\
  ((regvar & bitname##__MSK) >> bitname##__POS)

#define GMC303_SET_BITSLICE(regvar, bitname, val)			\
  ((regvar & ~bitname##__MSK) | ((val<<bitname##__POS)&bitname##__MSK))


/*!
 * @brief Read multiple data from the starting regsiter address
 *
 * @param u8Addr Starting register address
 * @param pu8Data The data array of values read
 * @param u8Len Number of bytes to read
 *
 * @return Result from the burst read function
 * @retval >= 0 Success, number of bytes read
 * @retval -127 Error null bus
 * @retval -1   Bus communication error
 *
 */
s8 gmc303_burst_read(u8 u8Addr, u8* pu8Data, u8 u8Len);

/*!
 * @brief Write multiple data to the starting regsiter address
 *
 * @param u8Addr Starting register address
 * @param pu8Data The data array of values to write
 * @param u8Len Number of bytes to write
 *
 * @return Result from the burst write function
 * @retval >= 0 Success, number of bytes write
 * @retval -127 Error null bus
 * @retval -1   Communication error
 *
 */
s8 gmc303_burst_write(u8 u8Addr, u8* pu8Data, u8 u8Len);


/*!
 * @brief GMC303 initialize communication bus
 *
 * @param pbus Pointer to the I2C/SPI read/write bus support struct
 *
 * @return Result from bus communication function
 * @retval 0 Success
 * @retval -1 Bus communication error
 * @retval -127 Error null bus
 *
 */
s8 gmc303_bus_init(bus_support_t* pbus);

/*!
 * @brief GMC303 soft reset
 *
 * @param None
 *
 * @return Result from bus communication function
 * @retval -1 Bus communication error
 * @retval -127 Error null bus
 *
 */
s8 gmc303_soft_reset(void);

/*!
 * @brief GMC303 initialization
 *        1. Soft reset
 *        2. Turn on offset temperature compensation
 *        3. Set to continuous mode
 *        4. Turn on low pass filter
 *
 * @param None
 *
 * @return Result from bus communication function
 * @retval -1 Bus communication error
 * @retval -127 Error null bus
 *
 */
s8 gmc303_initialization(void);

/*!
 * @brief GMC303 read data XYZ
 *
 * @param pxyzt Data buffer to store the values
 *
 * @return Result from bus communication function
 * @retval -1 Bus communication error
 * @retval -127 Error null bus
 *
 */
s8 gmc303_read_data_xyz(raw_data_xyzt_t* pxyzt);

/*!
 * @brief GMC303 set operation mode.
 *
 * @param opMode: operation mode
 *
 * @return Result from bus communication function
 * @retval -1 Bus communication error
 * @retval -127 Error null bus
 *
 */
s8 gmc303_set_operation_mode(GMC303_OP_MODE_T opMode);

/* @brief GMC303 get XYZ sensitivity adjustment value
 *				Read ASAX/Y/Z and calculate the sensitivity adjustment value to return
 *
 * @param pAdjustVal Data buffer to store the sensitivity adjustment value
 * 				pAdjustVal->u.x = 1 + ASAX / 128
 *        pAdjustVal->u.y = 1 + ASAY / 128
 *        pAdjustVal->u.z = 1 + ASAZ / 128
 *
 * @return Result from bus communication function
 * @retval -1 Bus communication error
 * @retval -127 Error null bus
 *
 */
s8 gmc303_get_sensitivity_adjust_val(float_xyzt_t* pAdjustVal);

#endif // __GMC303_H__
