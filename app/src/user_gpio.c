#include"../inc/primary_deps.h"
#include"../bt/bt.h"

// gpio pointe to device tree node
// external usage of this in bt.c
struct gpio_dt_spec  blue=GPIO_DT_SPEC_GET(DT_ALIAS(ble),gpios);
//external usage in bt.c app/bt/bt.c
struct gpio_dt_spec button=GPIO_DT_SPEC_GET(DT_ALIAS(buttony),gpios);
LOG_MODULE_DECLARE(bt,LOG_LEVEL_DBG);
 struct gpio_callback cb;
 // external variable from app/bt/bt.c
 extern int button_state;
void button_pressed(const struct device *port,
					struct gpio_callback *cb,
					gpio_port_pins_t pins){
             LOG_DBG("running interrupt \n");
    send_button_state_indicate(button_state);
    return;

 }
bool gpio_init(){

    if(!gpio_is_ready_dt(&blue)){
        LOG_ERR(" gpio %u is not ready for use",blue.pin);
        return false;
    }
    if(!gpio_is_ready_dt(&button))
    {
        LOG_ERR(" gpio_ pin %u connected to yellow button",button.pin);
        return false;
    }
    gpio_pin_configure_dt(&blue,GPIO_OUTPUT_ACTIVE);
    gpio_pin_configure_dt(&button,GPIO_INPUT);

    for(uint8_t i;i<10;++i){
    gpio_pin_toggle_dt(&blue);
    k_msleep(100);
    gpio_pin_toggle_dt(&blue);
    k_msleep(100);
    }
    
  
  gpio_pin_interrupt_configure_dt(&button,GPIO_INT_EDGE_BOTH);
  gpio_init_callback(&cb,button_pressed,BIT(button.pin));
  gpio_add_callback_dt(&button,&cb);
  
  return true;
    
}


