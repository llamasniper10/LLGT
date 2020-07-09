// inc
//-------------------------------------------------------------------------
#if defined(__XC16__)
#include <xc.h>
#elif defined(__C30__)
#if defined(__PIC24E__)
#include <p24Exxxx.h>
#elif defined(__PIC24F__) || defined(__PIC24FK__)
#include <p24Fxxxx.h>
#elif defined(__PIC24H__)
#include <p24Hxxxx.h>
#endif
#endif
#include "main.h"
#include "bmm150.h"
#include "bmm150_defs.h"
#include "../mcc_generated_files/mssp1_i2c.h"
#include "../mcc_generated_files/pin_manager.h"
#include "../mcc_generated_files/system.h"

#include <xc.h>

#define CYCLES_PER_MS ((uint16_t)(FCY * 0.001)) // TODO: Verify this calculation is correct with set up XTAL_FREQ

void delay_ms(uint16_t d);                      // prototype declaration for delay function
bmm150_dev BMM150_Initialize(int8_t &rslt);           // prototype declaration for BMM150 initialization
int8_t drdy_interrupt_configure(struct bmm150_dev *dev);
int8_t perform_self_tests(struct bmm150_dev *dev);
int8_t read_sensor_data(struct bmm150_dev *dev);
int8_t set_sensor_settings(struct bmm150_dev *dev);



int main(void)
{
    // Initializing the MCU
    // Includes: PIN_MANAGER, CLK, INTERRUPT,MSSP1_I2C
    SYSTEM_Initialize();
    
    // Initialize and fully configure BMM150 Sensor(s))
    // TODO: Iterate and initialize all sensors
    int8_t rslt = BMM150_OK;
    bmm150_dev dev;
    dev = BMM150_Initialize(rslt); // TODO: Check for OK status, if not, maybe retry?
    
    while (1)
    {
        // TODO: Iterate and read each sensors data (while checking DRDY bus)
        rslt = read_sensor_data(&dev); // TODO: Check for OK status, if not, maybe retry?
        IO_RA1_SetHigh();
    }
    return 1;
}



// Delay by given ms function using _XTAL_FREQ/2
void delay_ms(uint16_t d) // TODO: Verify this calculation is correct with set up XTAL_FREQ -- Is there a better way to do this through the Microchip Timer1 module?
{
    uint16_t ms = CYCLES_PER_MS * d;
    __delay32(ms);
}

// Initializes BMM150 Sensor
bmm150_dev BMM150_Initialize(int8_t &rslt){
    struct bmm150_dev dev;

    // Sensor interface over I2C
    dev.dev_id = BMM150_DEFAULT_I2C_ADDRESS;
    dev.intf = BMM150_I2C_INTF;
    dev.read = (bmm150_com_fptr_t)MSSP1_I2C_MasterRead;
    dev.write = (bmm150_com_fptr_t)MSSP1_I2C_MasterWrite;
    dev.delay_ms = (bmm150_delay_fptr_t)delay_ms;

    rslt = bmm150_init(&dev);
    
    set_sensor_settings(dev);   // TODO: Check for OK status, if not, maybe retry?
    perform_self_tests(&dev);   // TODO: Check for OK status, if not, maybe retry?
    // TODO: Set up data ready interrupt logic
    
    return dev;
}

// Set sensor settings (Normal mode)
int8_t set_sensor_settings(struct bmm150_dev *dev)
{
	int8_t rslt;

	/* Setting the power mode as normal */
	dev->settings.pwr_mode = BMM150_NORMAL_MODE;
	rslt = bmm150_set_op_mode(dev);
	
	/* Setting the preset mode as Low power mode 
	i.e. data rate = 10Hz XY-rep = 1 Z-rep = 2*/
	dev->settings.preset_mode = BMM150_PRESETMODE_LOWPOWER;
	rslt = bmm150_set_presetmode(dev);
	
	return rslt;	
}

int8_t read_sensor_data(struct bmm150_dev *dev)
{
	int8_t rslt;

	/* Mag data for X,Y,Z axis are stored inside the
	bmm150_dev structure in int16_t format */
	rslt = bmm150_read_mag_data(dev);

	/* Print the Mag data */
	printf("\n Magnetometer data \n");                  // TODO: I believe we will want to return these results through pulse counting here, so create & call P.C. function here
	printf("MAG X : %d \t MAG Y : %d \t MAG Z : %d \n"
		,dev->data.x, dev->data.y, dev->data.z);
	
	return rslt;
}

int8_t perform_self_tests(struct bmm150_dev *dev)
{
	int8_t rslt;

	/* Perform Normal Self test */
	rslt = bmm150_perform_self_test(BMM150_NORMAL_SELF_TEST, dev);
	printf("\n NORMAL SELF TEST RESULT :  %d",rslt);
	
	/* Validate normal self test result */
	if (rslt == BMM150_OK) {
		printf("\n Normal Self test passed ");
	} else {
		printf("\n Normal Self test failed ");
	}
	/* Perform Advanced Self test */
	rslt |= bmm150_perform_self_test(BMM150_ADVANCED_SELF_TEST, dev);
	printf("\n ADVANCED SELF TEST RESULT : %d",rslt);

	/* Validate Advanced self test result */
	if (rslt == BMM150_OK) {
		printf("\n Advanced Self test passed ");
	} else {
		printf("\n Advanced Self test failed ");
	}
	
	return rslt;
}

// Configure Data Ready
int8_t drdy_interrupt_configure(struct bmm150_dev *dev) // TODO: Make sure this logic is good (with our PCB setup)
{
	int8_t rslt;
	uint16_t desired_settings;

	/* Set the macros to enable DRDY pin */
	desired_settings = BMM150_DRDY_PIN_EN_SEL | BMM150_DRDY_POLARITY_SEL;
	/* Set the drdy_pin_en to enable the drdy interrupt  */
	dev->settings.int_settings.drdy_pin_en = BMM150_INT_ENABLE;
	/* Set the polarity as active high on the DRDY pin */
	dev->settings.int_settings.drdy_polarity = BMM150_ACTIVE_HIGH_POLARITY; 
	
	/* Set the configurations in the sensor */
	rslt = bmm150_set_sensor_settings(desired_settings, dev);
	
	return rslt;
}

// Data Ready Interrupt Handler
int8_t drdy_interrupt_handling(struct bmm150_dev *dev) // TODO: Make sure this logic is good (with our PCB setup)
{
	int8_t rslt;
	
	/* Read the interrupt status */
	rslt = bmm150_get_interrupt_status(dev);
	if (rslt == BMM150_OK) {
		if (dev->int_status & BMM150_DATA_READY_INT) {
			/* Interrupt asserted - Read mag data */
			rslt = bmm150_read_mag_data(dev);
			printf("\n MAG DATA ");
			printf("\n MAG X : %d MAG Y : %d MAG Z : %d "
				,dev->data.x,dev->data.y,dev->data.z);
		} else {
			/*Interrupt not asserted */
			printf("\n Data is not ready yet");
		}
	}
	
	return rslt;
}