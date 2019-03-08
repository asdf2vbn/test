/*
 * mqtt_task.c
 *
 *  Created on: 2018年6月8日
 *      Author: ruby
 */


#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include "FreeRTOS.h"
#include "task.h"

#include "mqtt_task.h"
#include "cloud.h"
#include "parameter.h"
#include "common.h"



/*******************************************************************************
* 函数名:
* 参    数:
* 返回值:
* 描    述:
* 作    者:
********************************************************************************/
void mqtt_task(void *param)
{

	while(true)
	{
		if(g_sys_param.current_produce == YITO_RAIN
							|| YITO_CRACK == g_sys_param.current_produce || YITO_HYGRO==g_sys_param.current_produce)
		{
			if(g_cloud_para.is_connect_cloud == true)
			{
				if(g_cloud_para.tickCount  < g_sys_param.upload_freq - 2)
				{
					//receive_cloud_order();
				}
				else
				{
					//stop_rece_cloud_order();
				}
			}//end if
		}//end if

		vTaskDelay(200);

	}
}
