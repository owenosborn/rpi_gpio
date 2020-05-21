#include "m_pd.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <wiringPi.h>

static t_class *rpi_gpio_shiftin_class;

typedef struct _rpi_gpio_shiftin {
    t_object x_obj;
    t_outlet *sr_value, *sr_index;
    int sr_pload_pin, sr_clock_pin, sr_data_pin, sr_num_bits;
} t_rpi_gpio_shiftin;

static void rpi_gpio_shiftin_bang(t_rpi_gpio_shiftin *x) {
    uint32_t bitVal;

    // so far best way to do the bit banging reliably is to
    // repeat the calls to reduce output clock frequency, like ad hoc 'nop' instructions
    // delay functions no good for such small times

    // load
    digitalWrite(x->sr_pload_pin, LOW);
    digitalWrite(x->sr_pload_pin, LOW);
    digitalWrite(x->sr_pload_pin, LOW);
    digitalWrite(x->sr_pload_pin, LOW);
    digitalWrite(x->sr_pload_pin, LOW);
    digitalWrite(x->sr_pload_pin, LOW);
    digitalWrite(x->sr_pload_pin, LOW);
    digitalWrite(x->sr_pload_pin, LOW);
    
    digitalWrite(x->sr_pload_pin, HIGH);
    digitalWrite(x->sr_pload_pin, HIGH);
    digitalWrite(x->sr_pload_pin, HIGH);
    digitalWrite(x->sr_pload_pin, HIGH);
    digitalWrite(x->sr_pload_pin, HIGH);
    digitalWrite(x->sr_pload_pin, HIGH);
    digitalWrite(x->sr_pload_pin, HIGH);
    digitalWrite(x->sr_pload_pin, HIGH);
    
    // shiftin
   for(int i = 0; i < x->sr_num_bits; i++)
    {
        bitVal = digitalRead(x->sr_data_pin);

        outlet_float(x->sr_value, bitVal);
        outlet_float(x->sr_index, i);


        digitalWrite(x->sr_clock_pin, HIGH);
        digitalWrite(x->sr_clock_pin, HIGH);
        digitalWrite(x->sr_clock_pin, HIGH);
        digitalWrite(x->sr_clock_pin, HIGH);
        digitalWrite(x->sr_clock_pin, HIGH);
        digitalWrite(x->sr_clock_pin, HIGH);
        digitalWrite(x->sr_clock_pin, HIGH);
        digitalWrite(x->sr_clock_pin, HIGH);
        
        digitalWrite(x->sr_clock_pin, LOW);
        digitalWrite(x->sr_clock_pin, LOW);
        digitalWrite(x->sr_clock_pin, LOW);
        digitalWrite(x->sr_clock_pin, LOW);
        digitalWrite(x->sr_clock_pin, LOW);
        digitalWrite(x->sr_clock_pin, LOW);
        digitalWrite(x->sr_clock_pin, LOW);
        digitalWrite(x->sr_clock_pin, LOW);
    }
    
}

static void *rpi_gpio_shiftin_new(t_floatarg f1, t_floatarg f2, t_floatarg f3, t_floatarg f4) {
    t_rpi_gpio_shiftin *x = (t_rpi_gpio_shiftin *)pd_new(rpi_gpio_shiftin_class);
    x->sr_index = outlet_new(&x->x_obj, gensym("float"));
    x->sr_value = outlet_new(&x->x_obj, gensym("float"));
 
    x->sr_pload_pin = x->sr_clock_pin = x->sr_data_pin = 0; // hmmm is this safe?
    x->sr_pload_pin = f1; /// really need to vet these numbers, make sure they are appropriate GPIOs 
	x->sr_clock_pin = f2;
	x->sr_data_pin  = f3;
	x->sr_num_bits  = f4;
	
	post("sr pload pin:%d, sr clock pin:%d, sr data pin:%d, number bits:%d", x->sr_pload_pin, x->sr_clock_pin, x->sr_data_pin, x->sr_num_bits); 
	
    if (geteuid () != 0){
    	pd_error(x, "might need to be root for gpio access...");
    	//exit (1) ;
  	}

  	if (wiringPiSetupGpio() == -1) {
    	pd_error(x, "something wrong, maybe not root?...");
    	//exit (1) ;
    }

//pinMode (x->x_gpio_pin, OUTPUT) ;

    // GPIO for shift registers
    pinMode(x->sr_pload_pin, OUTPUT);
    pinMode(x->sr_clock_pin, OUTPUT);
    pinMode(x->sr_data_pin, INPUT);
    digitalWrite(x->sr_clock_pin, LOW);
    digitalWrite(x->sr_pload_pin, HIGH);

    return (x);
}

void rpi_gpio_shiftin_setup(void) {
    rpi_gpio_shiftin_class = class_new(gensym("rpi_gpio_shiftin"), (t_newmethod)rpi_gpio_shiftin_new, 0, sizeof(t_rpi_gpio_shiftin), 0, A_DEFFLOAT, A_DEFFLOAT, A_DEFFLOAT, A_DEFFLOAT, 0);
    class_addbang(rpi_gpio_shiftin_class, rpi_gpio_shiftin_bang);
}
