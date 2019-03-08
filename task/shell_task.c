/*************************************************************************************
* 版    权: 2017-2027, BDS Micro Chipset
* 文件名：shell_task.c
* 描    述: 实现串口调试任务
* 作    者：heyunfei 2017.12.8
* 历史记录: 新建
*************************************************************************************/
#include <stdio.h>
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "parameter.h"
#include "common.h"

#include "hal_gpio.h"
#include "gnss.h"
#include "compass.h"
#include "shell.h"

#include "unix_time.h"
#include "hygrometer.h" //温湿度传感器
#include "crack.h"  //裂缝计
#include "rain.h"  //雨量计
#include "cloud.h"
#include "fsl_wwdt.h"
#include "process.h"
#include "huace.h"

#include "slope.h"
#include "hal_i2c.h"



 uint8_t pdata[1];

void shell_task(void *pvParameters)
{

	while(true)
	{
		//传感器采集命令不能发太快,厂商反馈要间隔5秒发一定命令,这样才能稳定的响应
		static uint8_t frequency_gap = 0;

		parse_shell_data();  //解析shell命令


		 if(g_sys_param.commu_style  ==  COMMU_LORA)
		 {
			 device_data_handle();  //主动读取传感器数据
		 }


		 //UART0_Printf("g_sys_param.lora_try_count:%d\r\n", g_sys_param.lora_try_count);//for test

		frequency_gap++;

		if(frequency_gap >=6)
		{

			parse_gnss_data(); //解析GNSS数据


			frequency_gap= 0;

			if(g_sys_param.current_produce == YITO_RAIN)
			{
				//set_rain_time();
				read_rain_data();    //读取雨量计
				parse_rain_data();  //解析雨量计

				g_sys_param.read_sensor_count++;

				if(g_sys_param.read_sensor_count >= YITO_READ_SENSOR_MAX)
				{
					init_sensor_rain_data();
				}

			}else if(g_sys_param.current_produce == YITO_CRACK)
			{
				parse_crack_data(); //解析裂缝计,2018.3.30
				read_crack_data();  //读取裂缝计传感器数据  3.30

				g_sys_param.read_sensor_count++;

				if(g_sys_param.read_sensor_count >= YITO_READ_SENSOR_MAX)
				{
					init_sensor_crack_data();
				}
			}else if(g_sys_param.current_produce == YITO_HYGRO)
			{

				parse_hygro_data(); //解析温湿度计,2018.3.29
				read_hygro_data();  //读取温湿度传感器数据  3.29

				g_sys_param.read_sensor_count++;

				if(g_sys_param.read_sensor_count >= YITO_READ_SENSOR_MAX)
				{
					init_sensor_hygro_data();
				}

			}else if(g_sys_param.current_produce == YITO_SLOPE)
			{
				g_sys_param.read_sensor_count++;
				parse_slope_data();
				read_slope_data();
			}
		}


		//read_compass_data();
		//do_compass_cali();
		//sendCompassToDebugUart();
		//sendGNSSToDebugUart();
		//sendLoraToDebugUart();

		//Led_Flash(BOARD_LED4_PIN);
		//Led_Flash(BOARD_LED3_PIN);
		//Led_Flash(BOARD_LED2_PIN);
		Led_Flash(BOARD_LED1_PIN);

		//Led_Flash(BOARD_LED4_PIN);

		//printf("%d\n",get_unix_time(0x18,0x04,0x27));


		//shell_uart_send_byte(0x32);//for test

		//shell_uart_send_buffer("abcd",4);  //for test

		//gbrs_uart_send_byte(0x32);//for test
		//compass_uart_send_byte(0x3a); //for test


		//nb_uart_send_byte(0x35);//for test

		//printf("%d\n", sizeof(g_sys_param));//for test



#if 0
		if(true){
			int ret =0;
			char buf[8] ={10,11,12,13,14,15,16,17};

			char buf1[8] ={0,0};

			ret = I2C_WriteBuff(buf, 7, 8);//for test
			printf("w ret:%d\n",ret);

			I2C_ReadBuff(buf1, 7 , 8);//for test
			printf("r ret:%d\n",ret);

			printf("%d,%d,%d,%d,%d,%d,%d,%d\n",
					buf1[0],buf1[1],buf1[2],buf1[3],buf1[4],buf1[5],buf1[6],buf1[7]);

		}
#endif


		if(g_sys_param.gprs_tunnel_lose >=2 && g_sys_param.lora_tunnel_lose >=2)
		{
			UART0_Printf("\r\n----stop watch dog-----\r\n");//for test

		}else
		{
			WWDT_Refresh(WWDT);  //喂狗一次
		}


		vTaskDelay(1000);
	}

	vTaskSuspend(NULL);

}


