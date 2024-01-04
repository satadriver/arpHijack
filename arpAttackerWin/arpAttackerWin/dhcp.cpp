


/*
//#include "DHCP.h"



#define _DHCP_C

#define THIS_IS_DHCP



#define DHCP_TIMEOUT                    (TICK)(2L * TICK_SECOND)





#define DHCP_CLIENT_PORT                (68u)

#define DHCP_SERVER_PORT                (67u)



#define BOOT_REQUEST                    (1u)

#define BOOT_REPLY                      (2u)

#define HW_TYPE                         (1u)

#define LEN_OF_HW_TYPE                  (6u)



#define DHCP_MESSAGE_TYPE               (53u)

#define DHCP_MESSAGE_TYPE_LEN           (1u)



#define DHCP_UNKNOWN_MESSAGE            (0u)



#define DHCP_DISCOVER_MESSAGE           (1u)

#define DHCP_OFFER_MESSAGE              (2u)

#define DHCP_REQUEST_MESSAGE            (3u)

#define DHCP_DECLINE_MESSAGE            (4u)

#define DHCP_ACK_MESSAGE                (5u)

#define DHCP_NAK_MESSAGE                (6u)

#define DHCP_RELEASE_MESSAGE            (7u)



#define DHCP_SERVER_IDENTIFIER          (54u)

#define DHCP_SERVER_IDENTIFIER_LEN      (4u)



#define DHCP_PARAM_REQUEST_LIST         (55u)

#define DHCP_PARAM_REQUEST_LIST_LEN     (2u)

#define DHCP_PARAM_REQUEST_IP_ADDRESS       (50u)

#define DHCP_PARAM_REQUEST_IP_ADDRESS_LEN   (4u)

#define DHCP_SUBNET_MASK                (1u)

#define DHCP_ROUTER                     (3u)

#define DHCP_IP_LEASE_TIME              (51u)

#define DHCP_END_OPTION                 (255u)



#define HALF_HOUR                       (WORD)((WORD)60 * (WORD)30)



#define INVALID_UDP_SOCKET      (0xff)

#define INVALID_UDP_PORT        (0L)



#define SIZE_OF_DHCPMES   (548u)   //标准DHCP报文字节大小



DHCP_MES DHCPMes;

APP_CONFIG AppConfig;



SM_DHCP  smDHCPState = SM_DHCP_INIT;

static UDP_SOCKET DHCPSocket = INVALID_UDP_SOCKET;





DHCP_STATE DHCPState = { 0x00 };



static IP_ADDR DHCPServerID;

//static DWORD_VAL DHCPLeaseTime;



static IP_ADDR tempIPAddress;

static IP_ADDR tempGateway;

static IP_ADDR tempMask;



INT8U SizeOfOptionsUsed;    //DHCP报文options字段使用的字节

INT8U SizeOfDHCPMesUsed;	//DHCP报文全部字段使用字节长度





static INT8U _DHCPReceive(void);

static void _DHCPSend(INT8U messageType);



/*

*********************************************************************************************************

*                                            DHCPReset

*

* Description: 对DHCP进行重置，设置相应的参数

*

* Arguments  : void

*

* Returns    : void

*

* Notes		 : 当服务器发回NAC报文时调用此函数

*

**********************************************************************************************************

*/

void DHCPReset(void)

{

	// Do not reset DHCP if it was previously disabled.

	if (smDHCPState == SM_DHCP_DISABLED)

		return;



	if (DHCPSocket != INVALID_UDP_SOCKET)

		UDPClose(DHCPSocket);				 //？？？？？？？？？？？关闭套接字

	DHCPSocket = INVALID_UDP_SOCKET;



	smDHCPState = SM_DHCP_INIT;

	//    DHCPBindCount = 0;



	DHCPState.bits.bIsBound = FALSE;

}



/*

*********************************************************************************************************

*                                            DHCP_MesInit

*

* Description: DHCP报文初始化函数:对除了options字段外其他部分进行初始化，供DHCP_Send函数进行调用

*

* Arguments  : void

*

* Returns    : void

*

* Notes		 : 报文部分字段赋值为固定值

*

**********************************************************************************************************

*/

viod DHCP_MesInit(viod)

{

	INT8U i;



	DHCPMes.op = BOOT_REQUEST;

	DHCPMes.htype = HW_TYPE;

	DHCPMes.hlen = LEN_OF_HW_TYPE;

	DHCPMes.hops = 0;

	DHCPMes.xid[0] = 0x12;

	DHCPMes.xid[1] = 0x34;

	DHCPMes.xid[2] = 0x56;

	DHCPMes.xid[3] = 0x78;

	DHCPMes.secs[0] = 0;

	DHCPMes.secs[1] = 0;

	DHCPMes.flags[0] = 0x80;

	DHCPMes.flags[1] = 0;

	for (i = 0; i<4u; i++)

	{

		DHCPMes.ciaddr[i] = 0x00;

		DHCPMes.yiaddr[i] = 0x00;

		DHCPMes.siaddr[i] = 0x00;

		DHCPMes.giaddr[i] = 0x00;

	}

	DHCPMes.chaddr[0] = AppConfig.MyMACAddr.v[0];

	DHCPMes.chaddr[1] = AppConfig.MyMACAddr.v[1];

	DHCPMes.chaddr[2] = AppConfig.MyMACAddr.v[2];

	DHCPMes.chaddr[3] = AppConfig.MyMACAddr.v[3];

	DHCPMes.chaddr[4] = AppConfig.MyMACAddr.v[4];

	DHCPMes.chaddr[5] = AppConfig.MyMACAddr.v[5];

	for (i = 6; i<16u; i++)

	{

		DHCPMes.chaddr[i] = 0x00;

	}

	for (i = 0; i<64u; i++)

	{

		DHCPMes.sname[i] = 0x00;

	}

	for (i = 0; i<128u; i++)

	{

		DHCPMes.file[i] = 0x00;

	}

}

/*   end of DHCP_MesInit  */



/*

*********************************************************************************************************

*                                            DHCP_Task

*

* Description: DHCP主任务模块，获取IP、子网掩码、默认网关 ,并存放在全局变量结构体AppConfig中，是DHCP任务核心函数

*

* Arguments  : void

*

* Returns    : void

*

* Notes		 : 租期未考虑

*

**********************************************************************************************************

*/

void DHCP_Task(void)

{

	INT8U DHCPReValue;





	switch (smDHCPState)

	{

	case SM_DHCP_INIT:

		tempIPAddress.val = 0x0;

		//		   DHCP_MesInit();

		DHCPSocket = Socket_Create();    //？？？？？？？？？创建

		smDHCPState = SM_DHCP_BROADCAST;

		/* No break */

	case SM_DHCP_BROADCAST:



		if (DHCPState.bits.bIsBound)  smDHCPState = SM_DHCP_REQUEST;

		else if (UDPIsPutReady(DHCPSocket))	                 //???????????????????

		{

			DHCP_Send(DHCP_DISCOVER_MESSAGE);

			smDHCPState = SM_DHCP_DISCOVER;

		}

		break;

	case SM_DHCP_DISCOVER:

		if (UDPIsGetReady(DHCPSocket))	 //????????????????????????

		{

			if (DHCP_Receive() == DHCP_OFFER_MESSAGE)

			{

				smDHCPState = SM_DHCP_REQUEST;

			}

			else break;

		}

		else break;

	case SM_DHCP_REQUEST:

		if (UDPIsPutReady(DHCPSocket))		   //？？？？？？？套接字连接 成功

		{

			DHCP_Send(DHCP_REQUEST_MESSAGE);

			smDHCPState = SM_DHCP_BIND;

		}

		else break;

	case SM_DHCP_BIND:

		if (UDPIsGetReady(DHCPSocket))	 //??????????????????????

		{

			DHCPReValue = DHCP_Receive();

			if (DHCPReValue == DHCP_NAK_MESSAGE)

			{

				DHCP_Reset();

				return;

			}

			else if (DHCPReValue == DHCP_ACK_MESSAGE)

			{

				UDPClose(DHCPSocket);   //???????????

				DHCPSocket = INVALID_UDP_SOCKET;

				AppConfig.MyIPAddr = tempIPAddress;

				AppConfig.MyMask = tempMask;

				AppConfig.MyGateway = tempGateway;

				return;

			}

		}

		break;

	}/* end of switch*/

}/*end of DHCP_Task*/





 /*

 *********************************************************************************************************

 *                                            DHCP_Send

 *

 * Description: DHCP发送报文函数，根据传入的报文类型，产生相应的报文并发送

 *

 * Arguments  : INT8U messageType 报文类型	 DHCP_DISCOVER_MESSAGE或DHCP_REQUEST_MESSAGE

 *

 * Returns    : void

 *

 * Notes		 : 租期未考虑

 *

 **********************************************************************************************************

 */

void DHCP_Send(INT8U messageType)

{

	DHCP_MesInit();

	INT8U n = 0;

	//options字段开始标志

	DHCPMes.options[n++] = 99;

	DHCPMes.options[n++] = 130;

	DHCPMes.options[n++] = 83;

	DHCPMes.options[n++] = 99;



	//设置options 53字段（报文类型信息）

	DHCPMes.options[n++] = DHCP_MESSAGE_TYPE;

	DHCPMes.options[n++] = DHCP_MESSAGE_TYPE_LEN;

	DHCPMes.options[n++] = messageType;



	if (messageType == DHCP_DISCOVER_MESSAGE)

	{

		DHCPState.bits.bIsBound = FALSE;

	}



	if (messageType != DHCP_DISCOVER_MESSAGE&&tempIPAddress.Val != 0x0000u)

	{

		DHCPMes.options[n++] = DHCP_SERVER_IDENTIFIER;

		DHCPMes.options[n++] = DHCP_SERVER_IDENTIFIER_LEN;

		DHCPMes.options[n++] = DHCPServerID.v[0];

		DHCPMes.options[n++] = DHCPServerID.v[1];

		DHCPMes.options[n++] = DHCPServerID.v[2];

		DHCPMes.options[n++] = DHCPServerID.v[3];

	}



	//请求队列（子网掩码和默认网关）

	DHCPMes.options[n++] = DHCP_PARAM_REQUEST_LIST;

	DHCPMes.options[n++] = DHCP_PARAM_REQUEST_LIST_LEN;

	DHCPMes.options[n++] = DHCP_SUBNET_MASK;

	DHCPMes.options[n++] = DHCP_ROUTER;



	if (messageType == DHCP_REQUEST_MESSAGE)

	{

		DHCPMes.options[n++] = DHCP_PARAM_REQUEST_IP_ADDRESS;

		DHCPMes.options[n++] = DHCP_PARAM_REQUEST_IP_ADDRESS_LEN;



		DHCPMes.options[n++] = tempIPAddress.v[0];

		DHCPMes.options[n++] = tempIPAddress.v[1];

		DHCPMes.options[n++] = tempIPAddress.v[2];

		DHCPMes.options[n++] = tempIPAddress.v[3];



	}

	DHCPMes.options[n++] = DHCP_END_OPTION;   //options字段结束标志位

	SizeOfOptionsUsed = n + 1;

	SizeOfDHCPMesUsed = SizeOfOptionsUsed + 236;





	/*

	转换mbuf 数组结构，调用 socket 发送报文  代写。。。。。。。。。



	*/











}//end of DHCP_Send()



 /*********************************************************************

 DHCP PACKET FORMAT AS PER RFC 1541



 0                   1                   2                   3

 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1

 +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

 |     op (1)    |   htype (1)   |   hlen (1)    |   hops (1)    |

 +---------------+---------------+---------------+---------------+

 |                            xid (4)                            |

 +-------------------------------+-------------------------------+

 |           secs (2)            |           flags (2)           |

 +-------------------------------+-------------------------------+

 |                          ciaddr  (4)                          |

 +---------------------------------------------------------------+

 |                          yiaddr  (4)                          |

 +---------------------------------------------------------------+

 |                          siaddr  (4)                          |

 +---------------------------------------------------------------+

 |                          giaddr  (4)                          |

 +---------------------------------------------------------------+

 |                                                               |

 |                          chaddr  (16)                         |

 |                                                               |

 |                                                               |

 +---------------------------------------------------------------+

 |                                                               |

 |                          sname   (64)                         |

 +---------------------------------------------------------------+

 |                                                               |

 |                          file    (128)                        |

 +---------------------------------------------------------------+

 |                                                               |

 |                          options (312)                        |

 +---------------------------------------------------------------+



 ********************************************************************/

 /*

 *********************************************************************************************************

 *                                            DHCP_Receive

 *

 * Description: 将接收到的报文的有效信息进行提取，并存放于全局变量的结构体

 *

 * Arguments  : void

 *

 * Returns    : (INT8U) type 报文类型

 *

 * Notes		 : 租期未考虑

 *

 **********************************************************************************************************

 */

static INT8U DHCP_Receive(void)

{



	/*

	???????????/mbf数据数组 转化成DHCP报文(DHCPMes)```````````



	*/

	INT8U type, i, j;

	INT8U s = 0;

	BOOL lbDone;

	IP_ADDR tempServerID;



	type = DHCP_UNKNOWN_MESSAGE;

	//make sure that the message comes from server

	if (DHCPMes.op = BOOT_REPLY)

	{

		if (!DHCPState.bits.bOffereReceived)	//之前没有获得offer报文

		{

			tempIPAddress.Val = DHCPMes.yiaddr.Val;	   //将客户端被提供的IP存入teamIPAddress中

		}



		// Check to see if chaddr (Client Hardware Address) belongs to us.

		for (i = 0; i<6u; i++)

		{

			if (DHCPMes.chaddr.v[i] != AppConfig.MyMACAddr.v[i])

				goto UDPInvalid;

		}



		lbDone = FALSE;

		do

		{

			//Get the Option number

			switch (DHCPMes.options[s])

	case DHCP_MESSAGE_TYPE:

		s++;

		if (DHCPMes.options[s++] == 1u)

		{

			//    s++;

			type = DHCPMes.options[s++];

			if (DHCPState.bits.bOfferReceived && (type == DHCP_OFFER_MESSAGE)) //throw another offer when we already have one

			{

				goto UDPInvalid;

			}

		}

		else goto UDPInvalid;

		break;



	case DHCP_SUBNET_MASK:

		s++;

		if (DHCPMes.options[s++] == 4u)

		{

			if (!DHCPState.bits.bOfferReceived)	// Check to see if this is the first offer

			{

				for (i = 0; i<4; i++)

				{

					tempMask[i] = DHCPMes.options[s++];  //将子网掩码存入tempMask结构体中

				}

			}

		}

		else goto UDPInvalid;

		break;



	case DHCP_ROUTER:

		s++;

		j = SHCPMes.options[s++];

		if (j >= 4u)  //大小为4的整数倍

		{

			if (!DHCPState.bits.bOfferReceived)	 // Check to see if this is the first offer

			{

				for (i = 0; i<4; i++)

				{

					tempGateway[i] = DHCPMes.options[s++]; //将网关存入tempGateway结构体中

				}

			}

		}

		else

			goto UDPInvalid;

		j -= 4;

		while (j--)

			s++;	//放弃其他 Router地址，指针加1

		break;



	case DHCP_SERVER_IDENTIFIER:

		s++;

		if (DHCPMes.options[s++] == 4u)

		{

			for (i = 0; i<4; i++)

			{

				tempServerID[i] = DHCPMes.options[s++]; //将服务器地址存放在tempServerID结构体中

			}

		}

		else goto UDPInvalid;

		break;



	case DHCP_END_OPTION:

		lbDone = TRUE;

		break;



	default:	   // Ignore all unsupport tags.	        

		s++;

		j = DHCPMes.options[s++];	  //unsupport size

		while (j--)  s++;   		  //Ignore all unsupport data



		} while (!lbDone);	 //end of do





	}//end of if(DHCPMes.op=BOOT_REPLY)



	if (type == DHCP_OFFER_MESSAGE)

	{

		DHCPServerID.Val = tempServerID.Val;

		DHCPState.bits.bOfferReceived = TRUE;

	}

	else

	{

		if (DHCPServerID.Val != tempServerID.Val) //make sure that received server id matches with our previous one.

		{

			type = DHCP_UNKNOWN_MESSAGE;

		}

	}



	/*

	处理完毕   待写。。。UDPDiscard();

	*/

	return type;



UDPInvalid:

	/*

	处理完毕	待写。。。。。	UDPDiscard();

	*/

	return DHCP_UNKNOWN_MESSAGE;



} //end of DHCP_Receive








