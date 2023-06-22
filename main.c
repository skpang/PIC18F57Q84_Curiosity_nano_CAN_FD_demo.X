 /*
 * MAIN Generated Driver File
 * 
 * @file main.c
 * 
 * @defgroup main MAIN
 * 
 * @brief This is the generated driver implementation file for the MAIN driver.
 *
 * @version MAIN Driver Version 1.0.0
*/

/*
© [2023] Microchip Technology Inc. and its subsidiaries.

    Subject to your compliance with these terms, you may use Microchip 
    software and any derivatives exclusively with Microchip products. 
    You are responsible for complying with 3rd party license terms  
    applicable to your use of 3rd party software (including open source  
    software) that may accompany Microchip software. SOFTWARE IS ?AS IS.? 
    NO WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS 
    SOFTWARE, INCLUDING ANY IMPLIED WARRANTIES OF NON-INFRINGEMENT,  
    MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT 
    WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, 
    INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY 
    KIND WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF 
    MICROCHIP HAS BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE 
    FORESEEABLE. TO THE FULLEST EXTENT ALLOWED BY LAW, MICROCHIP?S 
    TOTAL LIABILITY ON ALL CLAIMS RELATED TO THE SOFTWARE WILL NOT 
    EXCEED AMOUNT OF FEES, IF ANY, YOU PAID DIRECTLY TO MICROCHIP FOR 
    THIS SOFTWARE.
*/

/*
 
 * Curiosity Nano PIC18F57Q84 CAN FD Board
 * skpang.co.uk 06/23
 
 Nominal bit rate : 500kbps
 Data bit rate : 2000kbps
 
 */

#include "mcc_generated_files/system/system.h"
#include "mcc_generated_files/can/can1.h"
#include "mcc_generated_files/can/can_types.h"
/*
    Main application
*/
uint16_t tick;
uint16_t count=0;
uint8_t Transmit_Data[64];
   
static void can_tx(void)
{
    struct CAN_MSG_OBJ Transmission;
      
    Transmit_Data[0]= count;
    Transmit_Data[1]= (0xff00 & count) >> 8;
    count ++;

    Transmission.field.brs=CAN_BRS_MODE; //CAN_NON_BRS_MODE;//CAN_BRS_MODE;
    Transmission.field.dlc=DLC_64; //DLC_8;//;
    Transmission.field.formatType=CAN_FD_FORMAT; //CAN_2_0_FORMAT;
    Transmission.field.frameType=CAN_FRAME_DATA;
    Transmission.field.idType=CAN_FRAME_STD;
    Transmission.msgId=0x100;
    Transmission.data=Transmit_Data;
    
    if(CAN_TX_FIFO_AVAILABLE == (CAN1_TransmitFIFOStatusGet(CAN1_TXQ) & CAN_TX_FIFO_AVAILABLE))
    {
         CAN1_Transmit(CAN1_TXQ, &Transmission);
    }
    
}

static uint8_t CAN1_DlcToDataBytesGet(const enum CAN_DLC dlc)
{
    static const uint8_t dlcByteSize[] = {0U, 1U, 2U, 3U, 4U, 5U, 6U, 7U, 8U, 12U, 16U, 20U, 24U, 32U, 48U, 64U};
    return dlcByteSize[dlc];
}

static void can_event(void)
{
    struct CAN_MSG_OBJ msg;
    uint8_t i,len;
    uint32_t id;
    
    if(CAN1_ReceivedMessageCountGet())
    {
        CAN1_Receive(&msg);
        id = msg.msgId;
        len = CAN1_DlcToDataBytesGet(msg.field.dlc);
       
        printf("ID: %x %x    Len:%d Data: ", (uint16_t)((id & 0xffff0000) >>16),(uint16_t)id,len);
        
        for(i=0;i<len;i++)
        {
            printf("%x ",msg.data[i]);
        
        }
        printf("\n");
       
    }

}

void Timer0_CustomHandler(void){

    tick++;
}


int main(void)
{
    uint8_t i;
    SYSTEM_Initialize();
    tick=0;
    
    for(i=0;i<64;i++) Transmit_Data[i] =0 ;
    
    
    // If using interrupts in PIC18 High/Low Priority Mode you need to enable the Global High and Low Interrupts 
    // If using interrupts in PIC Mid-Range Compatibility Mode you need to enable the Global Interrupts 
    // Use the following macros to: 

    // Enable the Global High Interrupts 
    INTERRUPT_GlobalInterruptHighEnable(); 

    // Disable the Global High Interrupts 
    //INTERRUPT_GlobalInterruptHighDisable(); 

    // Enable the Global Low Interrupts 
    INTERRUPT_GlobalInterruptLowEnable(); 

    // Disable the Global Low Interrupts 
    //INTERRUPT_GlobalInterruptLowDisable(); 
    Timer0_OverflowCallbackRegister(Timer0_CustomHandler);
     
    printf("\nPIC18F57Q84 Curiosity Nano CAN FD demo skpang.co.uk 06/23\n");
    
    while(1)
    {
        can_event();
      
        if(tick >100)
        {
               tick = 0;
               can_tx();
               LED_Toggle();
        }
    }    
    
}