#include "m_pd.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <wiringPi.h>
#include <wiringPiSPI.h>

#define SPI_CH 1

static t_class *rpi_gpio_mcp3008_class;

typedef struct _rpi_gpio_mcp3008 {
    t_object x_obj;
    t_outlet *adc_0, *adc_1, *adc_2, *adc_3, *adc_4, *adc_5, *adc_6, *adc_7;
    int cs_pin;
} t_rpi_gpio_mcp3008;

static uint32_t adc_read(uint8_t adcnum) { 
    uint8_t commandout = 0;

    // read a channel from the MCP3008 ADC
    commandout = adcnum & 0x7;  // only 0-7
    commandout |= 0x18;     // start bit + single-ended bit

    uint8_t spibuf[3];

    spibuf[0] = commandout;
    spibuf[1] = 0;
    spibuf[2] = 0;

    wiringPiSPIDataRW(SPI_CH, spibuf, 3);    

    return ((spibuf[1] << 8) | (spibuf[2])) >> 4;
    
}

static void rpi_gpio_mcp3008_bang(t_rpi_gpio_mcp3008 *x) {
        outlet_float(x->adc_0, adc_read(0));
        outlet_float(x->adc_1, adc_read(1));
        outlet_float(x->adc_2, adc_read(2));
        outlet_float(x->adc_3, adc_read(3));
        outlet_float(x->adc_4, adc_read(4));
        outlet_float(x->adc_5, adc_read(5));
        outlet_float(x->adc_6, adc_read(6));
        outlet_float(x->adc_7, adc_read(7));
}

static void *rpi_gpio_mcp3008_new(t_floatarg f1) {
    t_rpi_gpio_mcp3008 *x = (t_rpi_gpio_mcp3008 *)pd_new(rpi_gpio_mcp3008_class);
    x->adc_0 = outlet_new(&x->x_obj, gensym("float"));
    x->adc_1 = outlet_new(&x->x_obj, gensym("float"));
    x->adc_2 = outlet_new(&x->x_obj, gensym("float"));
    x->adc_3 = outlet_new(&x->x_obj, gensym("float"));
    x->adc_4 = outlet_new(&x->x_obj, gensym("float"));
    x->adc_5 = outlet_new(&x->x_obj, gensym("float"));
    x->adc_6 = outlet_new(&x->x_obj, gensym("float"));
    x->adc_7 = outlet_new(&x->x_obj, gensym("float"));
 
    x->cs_pin = 0; // hmmm is this safe?
    x->cs_pin = f1; /// really need to vet these numbers, make sure they are appropriate GPIOs 
	
	post("cs pin: %d", x->cs_pin); 
	
    if (geteuid () != 0){
    	pd_error(x, "might need to be root for gpio access...");
    	//exit (1) ;
  	}

  	if (wiringPiSetupGpio() == -1) {
    	pd_error(x, "something wrong, maybe not root?...");
    	//exit (1) ;
    }

    wiringPiSPISetup(SPI_CH, 4*1000*1000);  // for adc
    
    return (x);
}

void rpi_gpio_mcp3008_setup(void) {
    rpi_gpio_mcp3008_class = class_new(gensym("rpi_gpio_mcp3008"), (t_newmethod)rpi_gpio_mcp3008_new, 0, sizeof(t_rpi_gpio_mcp3008), 0, A_DEFFLOAT, 0);
    class_addbang(rpi_gpio_mcp3008_class, rpi_gpio_mcp3008_bang);
}
