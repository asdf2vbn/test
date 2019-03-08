#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include "FreeRTOS.h"
#include "task.h"
#include "fsl_usart.h"
#include "gprs_task.h"
#include "SIM800C.h"
#include "hal_usart.h"
#include "LPC54113.h"
#include "fsl_gpio.h"
#include "cloud.h"
#include "hal_gpio.h"
#include "bynav.h"
#include "huace.h"
#include "fsl_wwdt.h"

#include "parameter.h"
#include "common.h"

void gprs_task(void *param)
{

	set_sim800c_connect(false);  //默认未非链接

	//cloud_main();

	//cloud_test1();
	while(true)
	{
		//通信方式为 gprs
		if(g_sys_param.commu_style == COMMU_GPRS)
		{

			if(g_sys_param.current_produce == YITO_RAIN
					|| YITO_CRACK == g_sys_param.current_produce || YITO_HYGRO==g_sys_param.current_produce)
			{
				cloud_work();

			}

			else if(g_sys_param.current_produce == GNSS_BYNAV_BASE)
			{

				//bynav_work();

				//bynav_single_work();   //北云单独工作

				bynav_work_with_mqtt();

				//bynav_work_with_tcp();

			}else if(g_sys_param.current_produce == GNSS_HUACE_BASE)
			{
				//test_huace();

				//huace_single_work();

				huace_work_with_mqtt();

				//huace_work_with_tcp();

				//huace_work_with_tcp_test();
			}

		}

		vTaskDelay(500);

	}


}


void gprs_task_back(void *param)
{
	bool     bstatus          = false;
	sim800c_con_e      status = SIM800c_NO_CON;

	set_sim800c_connect(false);  //默认未非链接
	GPIO_WritePinOutput(GPIO,1, 10, 0);

	while(true)
	{
		status = get_sim800c_con();
		if(status == SIM800C_STOP_CON) //停止连接动作
		{
			GPIO_WritePinOutput(GPIO,1, 10, 0);
			set_sim800c_connect(false);
		}
		else if(status == SIM800C_START_CON) //建立连接动作
		{
			GPIO_WritePinOutput(GPIO,1, 10, 1);
			SIM800C_Exit_DTU();     //取消透传模式
			SIM800C_Wait_CREG();    //等待注册完毕
			do
			{
				bstatus = SIM800C_TCPConnect_Server();  //建立透传模式的连接
			}while(!bstatus);

			//连接成功  nyh add
			set_sim800c_connect(true);

		}

		if(get_sim800c_connect())
		{
			UART2_SendByte(0x32);
			vTaskDelay(1000);
		}



#if 0
		if(g_sim800c_rx_buf.bufLen > 0)
		{
			UART_send_bytes(USART4, g_sim800c_rx_buf.buf, g_sim800c_rx_buf.bufLen);
			SIM800C_clear_buffer(&g_sim800c_rx_buf);
		}
#endif

	}

}


