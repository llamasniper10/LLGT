/**
  MSSP1_I2C Generated Driver File

  @Company
    Microchip Technology Inc.

  @File Name
    mssp1_i2c.c

  @Summary
    This is the generated source file for the MSSP1_I2C driver using PIC24 / dsPIC33 / PIC32MM MCUs

  @Description
    This source file provides APIs for driver for MSSP1_I2C.
    Generation Information :
        Product Revision  :  PIC24 / dsPIC33 / PIC32MM MCUs - 1.167.0
        Device            :  PIC24F08KM202    
    The generated drivers are tested against the following:
        Compiler          :  XC16 v1.50
        MPLAB 	          :  MPLAB X v5.35
*/

/*
    (c) 2020 Microchip Technology Inc. and its subsidiaries. You may use this
    software and any derivatives exclusively with Microchip products.

    THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
    EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
    WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
    PARTICULAR PURPOSE, OR ITS INTERACTION WITH MICROCHIP PRODUCTS, COMBINATION
    WITH ANY OTHER PRODUCTS, OR USE IN ANY APPLICATION.

    IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE,
    INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND
    WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS
    BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE
    FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN
    ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
    THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.

    MICROCHIP PROVIDES THIS SOFTWARE CONDITIONALLY UPON YOUR ACCEPTANCE OF THESE
    TERMS.
*/

/**
  Section: Included Files
*/

#include "mssp1_i2c.h"

/**
  I2C Driver Queue Status Type

  @Summary
    Defines the type used for the transaction queue status.

  @Description
    This defines type used to keep track of the queue status.
 */

typedef union
{
    struct
    {
            uint8_t full:1;
            uint8_t empty:1;
            uint8_t reserved:6;
    }s;
    uint8_t status;
}I2C_TR_QUEUE_STATUS;

/**
  I2C Driver Queue Entry Type

  @Summary
    Defines the object used for an entry in the i2c queue items.

  @Description
    This defines the object in the i2c queue. Each entry is a composed
    of a list of TRBs, the number of the TRBs and the status of the
    currently processed TRB.
 */
typedef struct
{
    uint8_t                             count;          // a count of trb's in the trb list
    MSSP1_I2C_TRANSACTION_REQUEST_BLOCK *ptrb_list;     // pointer to the trb list
    MSSP1_I2C_MESSAGE_STATUS            *pTrFlag;       // set with the error of the last trb sent.
                                                        // if all trb's are sent successfully,
                                                        // then this is MSSP1_I2C_MESSAGE_COMPLETE
} I2C_TR_QUEUE_ENTRY;

/**
  I2C Master Driver Object Type

  @Summary
    Defines the object that manages the i2c master.

  @Description
    This defines the object that manages the sending and receiving of
    i2c master transactions.
  */

typedef struct
{
    /* Read/Write Queue */
    I2C_TR_QUEUE_ENTRY          *pTrTail;       // tail of the queue
    I2C_TR_QUEUE_ENTRY          *pTrHead;       // head of the queue
    I2C_TR_QUEUE_STATUS         trStatus;       // status of the last transaction
    uint8_t                     i2cDoneFlag;    // flag to indicate the current
                                                // transaction is done
    uint8_t                     i2cErrors;      // keeps track of errors


} I2C_OBJECT ;

/**
  I2C Master Driver State Enumeration

  @Summary
    Defines the different states of the i2c master.

  @Description
    This defines the different states that the i2c master
    used to process transactions on the i2c bus.
*/

typedef enum
{
    S_MASTER_IDLE,
    S_MASTER_RESTART,
    S_MASTER_SEND_ADDR,
    S_MASTER_SEND_DATA,
    S_MASTER_SEND_STOP,
    S_MASTER_ACK_ADDR,
    S_MASTER_RCV_DATA,
    S_MASTER_RCV_STOP,
    S_MASTER_ACK_RCV_DATA,
    S_MASTER_NOACK_STOP,
    S_MASTER_SEND_ADDR_10BIT_LSB,
    S_MASTER_10BIT_RESTART,
    
} I2C_MASTER_STATES;

/**
 Section: Macro Definitions
*/

/* defined for MSSP1_I2C */

#ifndef MSSP1_I2C_CONFIG_TR_QUEUE_LENGTH
        #define MSSP1_I2C_CONFIG_TR_QUEUE_LENGTH 1
#endif

#define MSSP1_I2C_TRANSMIT_REG                       SSP1BUF                 // Defines the transmit register used to send data.
#define MSSP1_I2C_RECEIVE_REG                        SSP1BUF                 // Defines the receive register used to receive data.

// The following control bits are used in the I2C state machine to manage
// the I2C module and determine next states.
#define MSSP1_I2C_WRITE_COLLISION_STATUS_BIT         SSP1CON1bits.WCOL       // Defines the write collision status bit.
#define MSSP1_I2C_MODE_SELECT_BITS                   SSP1CON1bits.SSPM       // I2C Master Mode control bit.
#define MSSP1_I2C_MASTER_ENABLE_CONTROL_BITS         SSP1CON1bits.SSPEN      // I2C port enable control bit.

#define MSSP1_I2C_START_CONDITION_ENABLE_BIT         SSP1CON2bits.SEN        // I2C START control bit.
#define MSSP1_I2C_REPEAT_START_CONDITION_ENABLE_BIT  SSP1CON2bits.RSEN       // I2C Repeated START control bit.
#define MSSP1_I2C_RECEIVE_ENABLE_BIT                 SSP1CON2bits.RCEN       // I2C Receive enable control bit.
#define MSSP1_I2C_STOP_CONDITION_ENABLE_BIT          SSP1CON2bits.PEN        // I2C STOP control bit.
#define MSSP1_I2C_ACKNOWLEDGE_ENABLE_BIT             SSP1CON2bits.ACKEN      // I2C ACK start control bit.
#define MSSP1_I2C_ACKNOWLEDGE_DATA_BIT               SSP1CON2bits.ACKDT      // I2C ACK data control bit.
#define MSSP1_I2C_ACKNOWLEDGE_STATUS_BIT             SSP1CON2bits.ACKSTAT    // I2C ACK status bit.

/**
 Section: Local Functions
*/

static void MSSP1_I2C_FunctionComplete(void);
static void MSSP1_I2C_Stop(MSSP1_I2C_MESSAGE_STATUS completion_code);

/**
 Section: Local Variables
*/

static I2C_TR_QUEUE_ENTRY                  mssp1_i2c_tr_queue[MSSP1_I2C_CONFIG_TR_QUEUE_LENGTH];
static I2C_OBJECT                          mssp1_i2c_object;
static I2C_MASTER_STATES                   mssp1_i2c_state = S_MASTER_IDLE;
static uint8_t                             mssp1_i2c_trb_count = 0;

static MSSP1_I2C_TRANSACTION_REQUEST_BLOCK *p_mssp1_i2c_trb_current = NULL;
static I2C_TR_QUEUE_ENTRY                  *p_mssp1_i2c_current = NULL;


/**
  Section: Driver Interface
*/


void MSSP1_I2C_Initialize(void)
{
    mssp1_i2c_object.pTrHead = mssp1_i2c_tr_queue;
    mssp1_i2c_object.pTrTail = mssp1_i2c_tr_queue;
    mssp1_i2c_object.trStatus.s.empty = true;
    mssp1_i2c_object.trStatus.s.full = false;

    mssp1_i2c_object.i2cErrors = 0;

    // SMP High Speed; CKE Idle to Active; 
    SSP1STAT = 0x00;
    // SSPEN enabled; WCOL no_collision; CKP Clock Stretch; SSPM FOSC/(2 * (BRG_Value_I2C + 1)); SSPOV no_overflow; 
    SSP1CON1 = 0x28;
    // SBCDE disabled; BOEN disabled; SCIE disabled; PCIE disabled; DHEN disabled; SDAHT 100ns; AHEN disabled; 
    SSP1CON3 = 0x00;
    // Baud Rate Generator Value: SSPADD 3;   
    // Calculated Frequency: 250000 Hz; 
    SSP1ADD = 0x03;

    
}




        
uint8_t MSSP1_I2C_ErrorCountGet(void)
{
    uint8_t ret;

    ret = mssp1_i2c_object.i2cErrors;
    return ret;
}

void MSSP1_I2C_MasterTasks ( void )
{
  
    static uint8_t  *pi2c_buf_ptr;
    static uint16_t i2c_address = 0;
    static uint8_t  i2c_bytes_left = 0;
    static uint8_t  i2c_10bit_address_restart = 0;


    // Check first if there was a collision.
    // If we have a Write Collision, reset and go to idle state */
    if(MSSP1_I2C_WRITE_COLLISION_STATUS_BIT)
    {
        // clear the Write collision
        MSSP1_I2C_WRITE_COLLISION_STATUS_BIT = 0;
        mssp1_i2c_state = S_MASTER_IDLE;
        *(p_mssp1_i2c_current->pTrFlag) = MSSP1_I2C_MESSAGE_FAIL;

        // reset the buffer pointer
        p_mssp1_i2c_current = NULL;

        return;
    }

    /* Handle the correct i2c state */
    switch(mssp1_i2c_state)
    {
        case S_MASTER_IDLE:    /* In reset state, waiting for data to send */

            if(mssp1_i2c_object.trStatus.s.empty != true)
            {
                // grab the item pointed by the head
                p_mssp1_i2c_current     = mssp1_i2c_object.pTrHead;
                mssp1_i2c_trb_count     = mssp1_i2c_object.pTrHead->count;
                p_mssp1_i2c_trb_current = mssp1_i2c_object.pTrHead->ptrb_list;

                mssp1_i2c_object.pTrHead++;

                // check if the end of the array is reached
                if(mssp1_i2c_object.pTrHead == (mssp1_i2c_tr_queue + MSSP1_I2C_CONFIG_TR_QUEUE_LENGTH))
                {
                    // adjust to restart at the beginning of the array
                    mssp1_i2c_object.pTrHead = mssp1_i2c_tr_queue;
                }

                // since we moved one item to be processed, we know
                // it is not full, so set the full status to false
                mssp1_i2c_object.trStatus.s.full = false;

                // check if the queue is empty
                if(mssp1_i2c_object.pTrHead == mssp1_i2c_object.pTrTail)
                {
                    // it is empty so set the empty status to true
                    mssp1_i2c_object.trStatus.s.empty = true;
                }

                // send the start condition
                MSSP1_I2C_START_CONDITION_ENABLE_BIT = 1;
                
                // start the i2c request
                mssp1_i2c_state = S_MASTER_SEND_ADDR;
            }

            break;

        case S_MASTER_RESTART:

            /* check for pending i2c Request */

            // ... trigger a REPEATED START
            MSSP1_I2C_REPEAT_START_CONDITION_ENABLE_BIT = 1;

            // start the i2c request
            mssp1_i2c_state = S_MASTER_SEND_ADDR;

            break;

        case S_MASTER_SEND_ADDR_10BIT_LSB:

            if(MSSP1_I2C_ACKNOWLEDGE_STATUS_BIT)
            {
                mssp1_i2c_object.i2cErrors++;
                MSSP1_I2C_Stop(MSSP1_I2C_MESSAGE_ADDRESS_NO_ACK);
            }
            else
            {
                // Remove bit 0 as R/W is never sent here
                MSSP1_I2C_TRANSMIT_REG = (i2c_address >> 1) & 0x00FF;

                // determine the next state, check R/W
                if(i2c_address & 0x01)
                {
                    // if this is a read we must repeat start
                    // the bus to perform a read
                    mssp1_i2c_state = S_MASTER_10BIT_RESTART;
                }
                else
                {
                    // this is a write continue writing data
                    mssp1_i2c_state = S_MASTER_SEND_DATA;
                }
            }

            break;

        case S_MASTER_10BIT_RESTART:

            if(MSSP1_I2C_ACKNOWLEDGE_STATUS_BIT)
            {
                mssp1_i2c_object.i2cErrors++;
                MSSP1_I2C_Stop(MSSP1_I2C_MESSAGE_ADDRESS_NO_ACK);
            }
            else
            {
                // ACK Status is good
                // restart the bus
                MSSP1_I2C_REPEAT_START_CONDITION_ENABLE_BIT = 1;

                // fudge the address so S_MASTER_SEND_ADDR works correctly
                // we only do this on a 10-bit address resend
                i2c_address = 0x00F0 | ((i2c_address >> 8) & 0x0006);

                // set the R/W flag
                i2c_address |= 0x0001;

                // set the address restart flag so we do not change the address
                i2c_10bit_address_restart = 1;

                // Resend the address as a read
                mssp1_i2c_state = S_MASTER_SEND_ADDR;
            }

            break;

        case S_MASTER_SEND_ADDR:

            /* Start has been sent, send the address byte */

            /* Note: 
                On a 10-bit address resend (done only during a 10-bit
                device read), the original i2c_address was modified in
                S_MASTER_10BIT_RESTART state. So the check if this is
                a 10-bit address will fail and a normal 7-bit address
                is sent with the R/W bit set to read. The flag
                i2c_10bit_address_restart prevents the  address to
                be re-written.
             */
            if(i2c_10bit_address_restart != 1)
            {
                // extract the information for this message
                i2c_address    = p_mssp1_i2c_trb_current->address;
                pi2c_buf_ptr   = p_mssp1_i2c_trb_current->pbuffer;
                i2c_bytes_left = p_mssp1_i2c_trb_current->length;
            }
            else
            {
                // reset the flag so the next access is ok
                i2c_10bit_address_restart = 0;
            }

            // check for 10-bit address
            if(i2c_address > 0x00FF)
            {
                // we have a 10 bit address
                // send bits<9:8>
                // mask bit 0 as this is always a write
                MSSP1_I2C_TRANSMIT_REG = 0xF0 | ((i2c_address >> 8) & 0x0006);
                mssp1_i2c_state = S_MASTER_SEND_ADDR_10BIT_LSB;
            }
            else
            {
                // Transmit the address
                MSSP1_I2C_TRANSMIT_REG = i2c_address;
                if(i2c_address & 0x01)
                {
                    // Next state is to wait for address to be acked
                    mssp1_i2c_state = S_MASTER_ACK_ADDR;
                }
                else
                {
                    // Next state is transmit
                    mssp1_i2c_state = S_MASTER_SEND_DATA;
                }
            }
            break;

        case S_MASTER_SEND_DATA:

            // Make sure the previous byte was acknowledged
            if(MSSP1_I2C_ACKNOWLEDGE_STATUS_BIT)
            {
                // Transmission was not acknowledged
                mssp1_i2c_object.i2cErrors++;

                // Reset the Ack flag
                MSSP1_I2C_ACKNOWLEDGE_STATUS_BIT = 0;

                // Send a stop flag and go back to idle
                MSSP1_I2C_Stop(MSSP1_I2C_DATA_NO_ACK);

            }
            else
            {
                // Did we send them all ?
                if(i2c_bytes_left-- == 0U)
                {
                    // yup sent them all!

                    // update the trb pointer
                    p_mssp1_i2c_trb_current++;

                    // are we done with this string of requests?
                    if(--mssp1_i2c_trb_count == 0)
                    {
                        MSSP1_I2C_Stop(MSSP1_I2C_MESSAGE_COMPLETE);
                    }
                    else
                    {
                        // no!, there are more TRB to be sent.
                        //MSSP1_I2C_START_CONDITION_ENABLE_BIT = 1;

                        // In some cases, the slave may require
                        // a restart instead of a start. So use this one
                        // instead.
                        MSSP1_I2C_REPEAT_START_CONDITION_ENABLE_BIT = 1;

                        // start the i2c request
                        mssp1_i2c_state = S_MASTER_SEND_ADDR;

                    }
                }
                else
                {
                    // Grab the next data to transmit
                    MSSP1_I2C_TRANSMIT_REG = *pi2c_buf_ptr++;
                }
            }
            break;

        case S_MASTER_ACK_ADDR:

            /* Make sure the previous byte was acknowledged */
            if(MSSP1_I2C_ACKNOWLEDGE_STATUS_BIT)
            {

                // Transmission was not acknowledged
                mssp1_i2c_object.i2cErrors++;

                // Send a stop flag and go back to idle
                MSSP1_I2C_Stop(MSSP1_I2C_MESSAGE_ADDRESS_NO_ACK);

                // Reset the Ack flag
                MSSP1_I2C_ACKNOWLEDGE_STATUS_BIT = 0;
            }
            else
            {
                MSSP1_I2C_RECEIVE_ENABLE_BIT = 1;
                mssp1_i2c_state = S_MASTER_ACK_RCV_DATA;
            }
            break;

        case S_MASTER_RCV_DATA:

            /* Acknowledge is completed.  Time for more data */

            // Next thing is to ack the data
            mssp1_i2c_state = S_MASTER_ACK_RCV_DATA;

            // Set up to receive a byte of data
            MSSP1_I2C_RECEIVE_ENABLE_BIT = 1;

            break;

        case S_MASTER_ACK_RCV_DATA:

            // Grab the byte of data received and acknowledge it
            *pi2c_buf_ptr++ = MSSP1_I2C_RECEIVE_REG;

            // Check if we received them all?
            if(--i2c_bytes_left)
            {

                /* No, there's more to receive */

                // No, bit 7 is clear.  Data is ok
                // Set the flag to acknowledge the data
                MSSP1_I2C_ACKNOWLEDGE_DATA_BIT = 0;

                // Wait for the acknowledge to complete, then get more
                mssp1_i2c_state = S_MASTER_RCV_DATA;
            }
            else
            {

                // Yes, it's the last byte.  Don't ack it
                // Flag that we will nak the data
                MSSP1_I2C_ACKNOWLEDGE_DATA_BIT = 1;

                MSSP1_I2C_FunctionComplete();
            }

            // Initiate the acknowledge
            MSSP1_I2C_ACKNOWLEDGE_ENABLE_BIT = 1;
            break;

        case S_MASTER_RCV_STOP:                
        case S_MASTER_SEND_STOP:

            // Send the stop flag
            MSSP1_I2C_Stop(MSSP1_I2C_MESSAGE_COMPLETE);
            break;

        default:

            // This case should not happen, if it does then
            // terminate the transfer
            mssp1_i2c_object.i2cErrors++;
            MSSP1_I2C_Stop(MSSP1_I2C_LOST_STATE);
            break;

    }
}

static void MSSP1_I2C_FunctionComplete(void)
{

    // update the trb pointer
    p_mssp1_i2c_trb_current++;

    // are we done with this string of requests?
    if(--mssp1_i2c_trb_count == 0)
    {
        mssp1_i2c_state = S_MASTER_SEND_STOP;
    }
    else
    {
        mssp1_i2c_state = S_MASTER_RESTART;
    }

}

static void MSSP1_I2C_Stop(MSSP1_I2C_MESSAGE_STATUS completion_code)
{
    // then send a stop
    MSSP1_I2C_STOP_CONDITION_ENABLE_BIT = 1;

    // make sure the flag pointer is not NULL
    if (p_mssp1_i2c_current->pTrFlag != NULL)
    {
        // update the flag with the completion code
        *(p_mssp1_i2c_current->pTrFlag) = completion_code;
    }

    // Done, back to idle
    mssp1_i2c_state = S_MASTER_IDLE;
    
}

void MSSP1_I2C_MasterWrite(
                                uint8_t *pdata,
                                uint8_t length,
                                uint16_t address,
                                MSSP1_I2C_MESSAGE_STATUS *pstatus)
{
    static MSSP1_I2C_TRANSACTION_REQUEST_BLOCK   trBlock;

    // check if there is space in the queue
    if (mssp1_i2c_object.trStatus.s.full != true)
    {
        MSSP1_I2C_MasterWriteTRBBuild(&trBlock, pdata, length, address);
        MSSP1_I2C_MasterTRBInsert(1, &trBlock, pstatus);
    }
    else
    {
        *pstatus = MSSP1_I2C_MESSAGE_FAIL;
    }

}


                                
void MSSP1_I2C_MasterRead(
                                uint8_t *pdata,
                                uint8_t length,
                                uint16_t address,
                                MSSP1_I2C_MESSAGE_STATUS *pstatus)
{
    static MSSP1_I2C_TRANSACTION_REQUEST_BLOCK   trBlock;


    // check if there is space in the queue
    if (mssp1_i2c_object.trStatus.s.full != true)
    {
        MSSP1_I2C_MasterReadTRBBuild(&trBlock, pdata, length, address);
        MSSP1_I2C_MasterTRBInsert(1, &trBlock, pstatus);
    }
    else
    {
        *pstatus = MSSP1_I2C_MESSAGE_FAIL;
    }

}
                                
       

void MSSP1_I2C_MasterTRBInsert(
                                uint8_t count,
                                MSSP1_I2C_TRANSACTION_REQUEST_BLOCK *ptrb_list,
                                MSSP1_I2C_MESSAGE_STATUS *pflag)
{

    // check if there is space in the queue
    if (mssp1_i2c_object.trStatus.s.full != true)
    {
        *pflag = MSSP1_I2C_MESSAGE_PENDING;

        mssp1_i2c_object.pTrTail->ptrb_list = ptrb_list;
        mssp1_i2c_object.pTrTail->count     = count;
        mssp1_i2c_object.pTrTail->pTrFlag   = pflag;
        mssp1_i2c_object.pTrTail++;

        // check if the end of the array is reached
        if (mssp1_i2c_object.pTrTail == (mssp1_i2c_tr_queue + MSSP1_I2C_CONFIG_TR_QUEUE_LENGTH))
        {
            // adjust to restart at the beginning of the array
            mssp1_i2c_object.pTrTail = mssp1_i2c_tr_queue;
        }

        // since we added one item to be processed, we know
        // it is not empty, so set the empty status to false
        mssp1_i2c_object.trStatus.s.empty = false;

        // check if full
        if (mssp1_i2c_object.pTrHead == mssp1_i2c_object.pTrTail)
        {
            // it is full, set the full status to true
            mssp1_i2c_object.trStatus.s.full = true;
        }

        // for interrupt based
        if (mssp1_i2c_state == S_MASTER_IDLE)
        { 
            // force the task to run since we know that the queue has
            // something that needs to be sent
        }           
        
    }
    else
    {
        *pflag = MSSP1_I2C_MESSAGE_FAIL;
    }

}

         

void MSSP1_I2C_MasterReadTRBBuild(
                                MSSP1_I2C_TRANSACTION_REQUEST_BLOCK *ptrb,
                                uint8_t *pdata,
                                uint8_t length,
                                uint16_t address)
{
    ptrb->address  = address << 1;
    // make this a read
    ptrb->address |= 0x01;
    ptrb->length   = length;
    ptrb->pbuffer  = pdata;
}



void MSSP1_I2C_MasterWriteTRBBuild(
                                MSSP1_I2C_TRANSACTION_REQUEST_BLOCK *ptrb,
                                uint8_t *pdata,
                                uint8_t length,
                                uint16_t address)
{
    ptrb->address = address << 1;
    ptrb->length  = length;
    ptrb->pbuffer = pdata;
}


                                
bool MSSP1_I2C_MasterQueueIsEmpty(void)
{
    return(mssp1_i2c_object.trStatus.s.empty);
}



bool MSSP1_I2C_MasterQueueIsFull(void)
{
    return(mssp1_i2c_object.trStatus.s.full);
}        


        
/**
 End of File
*/
