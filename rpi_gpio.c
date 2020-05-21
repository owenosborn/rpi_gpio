#include "m_pd.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <wiringPi.h>

static t_class *rpi_gpio_class;

typedef struct _rpi_gpio {
    t_object x_obj;
    t_outlet *x_out1;
    int x_gpio_pin, x_pin_mode, x_pin_val;
} t_rpi_gpio;

static void rpi_gpio_bang(t_rpi_gpio *x) {
    int value;
    if (x->x_pin_mode == 0)	pd_error(x, "gpio in OUTPUT MODE");
    else {
    	value = digitalRead (x->x_gpio_pin);
    	outlet_float(x->x_out1, value);
    }
}

static void rpi_gpio_float(t_rpi_gpio *x, t_float f) {
    int value;
    value = f;
    if (x->x_pin_mode == 1)	pd_error(x, "gpio in INPUT MODE");
    else {  
    	digitalWrite (x->x_gpio_pin, value) ;
    }
}

static void *rpi_gpio_new(t_floatarg f1, t_floatarg f2, t_floatarg f3) {
    t_rpi_gpio *x = (t_rpi_gpio *)pd_new(rpi_gpio_class);
    x->x_out1 = outlet_new(&x->x_obj, gensym("float"));
 
    x->x_pin_val = x->x_gpio_pin = x->x_pin_mode = 0;
    x->x_gpio_pin = f1; 
	x->x_pin_mode = f2;
	x->x_pin_val  = f3;
	
	post("pin:%d, pin:%d, pin:%d,", x->x_gpio_pin, x->x_pin_mode, x->x_pin_val); 
	
    if (geteuid () != 0){
    	pd_error(x, "might need to be root for gpio access...");
    	//exit (1) ;
  	}

  	if (wiringPiSetupGpio() == -1) {
    	pd_error(x, "something wrong, maybe not root?...");
    	//exit (1) ;
    }
	if (x->x_pin_mode == 0) {
    	pinMode (x->x_gpio_pin, OUTPUT) ;
    	digitalWrite (x->x_gpio_pin, x->x_pin_val) ;
    	post("pin %d output mode", x->x_gpio_pin);
    }
    else {
    	pinMode (x->x_gpio_pin, INPUT) ;
    	post("pin %d input mode", x->x_gpio_pin);
    }

    return (x);
}

void rpi_gpio_setup(void) {
    rpi_gpio_class = class_new(gensym("rpi_gpio"), 
		(t_newmethod)rpi_gpio_new, 
		0, sizeof(t_rpi_gpio), 
		0, A_DEFFLOAT, A_DEFFLOAT, A_DEFFLOAT, 0);
	class_addfloat(rpi_gpio_class, rpi_gpio_float);
    class_addbang(rpi_gpio_class, rpi_gpio_bang);
}
