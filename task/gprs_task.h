/*
 * gprs_task.h
 *
 *  Created on: 2018Äê3ÔÂ9ÈÕ
 *      Author: heyunfei
 */

#ifndef GPRS_TASK_H_
#define GPRS_TASK_H_

#include "SIM800C.h"

void gprs_task(void *param);

extern sim800c_buf_t g_sim800c_rx_buf;

#endif /* GPRS_TASK_H_ */
