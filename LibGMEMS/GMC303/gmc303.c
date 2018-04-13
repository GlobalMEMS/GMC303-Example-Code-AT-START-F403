/*
 *
 ****************************************************************************
 * Copyright (C) 2018 GlobalMEMS, Inc. <www.globalmems.com>
 * All rights reserved.
 *
 * File : gmc303.c
 *
 * Date : 2018/04/11
 *
 * Usage: GMC303 sensor driver file
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

/*! @file gmc303.c
 *  @brief  GMC303 Sensor Driver File
 *  @author Joseph FC Tseng
 */

#include "stdio.h"
#include "gmc303.h"

static bus_support_t* pBus_support = 0;

/*!
 * @brief Read multiple data from the starting regsiter address
 *
 * @param u8Addr Starting register address
 * @param pu8Data The data array of values read
 * @param u8Len Number of bytes to read
 *
 * @return Result from the burst read function
 * @retval >= 0 Success
 * @retval -127 Error null bus
 * @retval -1   Communication error
 *
 */
s8 gmc303_burst_read(u8 u8Addr, u8* pu8Data, u8 u8Len){

  s8 comRslt = -1;
  if(pBus_support == NULL){
    return -127;
  }
  else{
    comRslt = pBus_support->bus_read(pBus_support->u8DevAddr, pu8Data, u8Addr, u8Len);
    if(comRslt == 0) //success, return # of bytes read
      comRslt = u8Len;
    else //return the error code
      comRslt = -comRslt;
  }

  return comRslt;
}


/*!
 * @brief Write multiple data to the starting regsiter address
 *
 * @param u8Addr Starting register address
 * @param pu8Data The data array of values to write
 * @param u8Len Number of bytes to write
 *
 * @return Result from the burst write function
 * @retval >= 0 Success
 * @retval -127 Error null bus
 * @retval -1   Communication error
 *
 */
s8 gmc303_burst_write(u8 u8Addr, u8* pu8Data, u8 u8Len){

  s8 comRslt = -1;
  if(pBus_support == NULL){
    return -127;
  }
  else{
    comRslt = pBus_support->bus_write(pBus_support->u8DevAddr, pu8Data, u8Addr, u8Len);
    if(comRslt == 0) //success, return # of bytes write
      comRslt = u8Len;
    else //return the error code
      comRslt = -comRslt;
  }

  return comRslt;
}

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
s8 gmc303_bus_init(bus_support_t* pbus){

  s8 comRslt = -1;
  u8 u8Data;

  //assign the I2C/SPI bus
  if(pbus == NULL)
    return -127;
  else
    pBus_support = pbus;

  //Read chip ID
  comRslt = gmc303_burst_read(GMC303_REG_DEVID, &u8Data, 1);

  return comRslt;
}

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
s8 gmc303_soft_reset(void){

  s8 comRslt = -1;
  u8 u8Data = 0;

  u8Data = GMC303_SET_BITSLICE(u8Data, GMC303_RST, 1);

  //Set the RST bit
  comRslt = gmc303_burst_write(GMC303_RST__REG, &u8Data, 1);

  return comRslt;
}

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
s8 gmc303_set_operation_mode(GMC303_OP_MODE_T opMode){

  s8 comRslt = -1;
  u8 u8Data = 0;

  //The OP mode bits
  u8Data = GMC303_SET_BITSLICE(u8Data, GMC303_OP_MODE, opMode);

  //Write operation mode register to change mode
  comRslt = gmc303_burst_write(GMC303_OP_MODE__REG, &u8Data, 1);

  return comRslt;
}

/*!
 * @brief GMC303 read data XYZ
 *
 * @param pxyz Data buffer to store the values
 *
 * @return Result from bus communication function
 * @retval -1 Bus communication error
 * @retval -127 Error null bus
 *
 */
s8 gmc303_read_data_xyz(raw_data_xyzt_t* pxyz){

  s8 comRslt = -1;
  u8 u8Data[6];
  s16 s16Tmp, i;

  comRslt = gmc303_burst_read(GMC303_DATA_READ_START__REG, u8Data, 6);

  if(comRslt < 0) goto EXIT;

  for(i = 0; i < 3; ++i){
    s16Tmp = (u8Data[2*i + 1] << 8) | (u8Data[2*i]);
    pxyz->v[i] = s16Tmp;
  }

 EXIT:
  return comRslt;
}

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
s8 gmc303_get_sensitivity_adjust_val(float_xyzt_t* pAdjustVal){

  s8 comRslt = -1;
  u8 u8Data[3];
  s32 i;

  //Read 3 bytes starting from 60h
  comRslt = gmc303_burst_read(GMC303_ASA_XYZ_START__REG, u8Data, 3);

  if(comRslt < 0) goto EXIT;

  for(i = 0; i < 3; ++i)
    pAdjustVal->v[i] = 1.0f + (u8Data[i] / 128.0f);

 EXIT:
  return comRslt;

}
