
#include "../bt/bt.h"
// declare log modules
LOG_MODULE_DECLARE(bt, LOG_LEVEL_DBG);
///  create advertising data  structure
static struct bt_data adv_data[] = {
    BT_DATA_BYTES(BT_DATA_FLAGS, (BT_LE_AD_GENERAL | BT_LE_AD_NO_BREDR)),
    BT_DATA(BT_DATA_NAME_COMPLETE, CONFIG_BT_DEVICE_NAME, sizeof(CONFIG_BT_DEVICE_NAME) - 1)};
// response package
static const unsigned char url_esp[] = {0x17, '/', '/', 'w', 'w', 'w', 'e', 's', 'p', 'r', 'e', 's', 'i', 'f',
                                        '.', 'c', 'o', 'm'};
// response package structure
static struct bt_data rsp_pkt[] = {
    BT_DATA(BT_DATA_URI, url_esp, sizeof(url_esp)),
};
// configure advertising parameters
static struct bt_le_adv_param *adv_param = BT_LE_ADV_PARAM((BT_LE_ADV_OPT_CONNECTABLE | BT_LE_ADV_OPT_USE_IDENTITY), 800, 801, NULL);

// connection parameters
struct bt_conn_cb connection_callbacks = {
    .connected = on_connected,
    .disconnected = on_disconnect
    //.le_phy_updated=on_le_phy_updated

};

// bluetooth connection parameter
struct bt_conn *my_conn = NULL;
void on_connected(struct bt_conn *conn, uint8_t err)
{
    if (err)
    {
        LOG_ERR(" error encountered during connection\n");
    }
    LOG_INF("connection successful\n");
    // bluetooth connection infomation
    struct bt_conn_info info;
    err = bt_conn_get_info(conn, &info);
    if (err)
    {
        LOG_ERR("bt_conn_get_info() returned %d", err);
        return;
    }
    double connection_interval = info.le.interval * 1.25; // in ms
    uint16_t supervisor_timeout = info.le.timeout * 10;   // ms

    LOG_INF("Connection parameters: interval %.2f ms, latency %i intervals,timeout %d ms",
            connection_interval, info.le.latency, supervisor_timeout);
    // update phy
    // printk("updating phy configurations\n");
    // update_phy(my_conn);
    my_conn = bt_conn_ref(conn);
}
void on_disconnect(struct bt_conn *conn, uint8_t reason)
{
    LOG_INF("Disconnected. Reason %d", reason);
    bt_conn_unref(my_conn);
}

 // this has been declared in gpio.c in app/src/gpio.c
extern struct gpio_dt_spec blue;
// declared in gpio.c app/src/gpio.c
extern struct gpio_dt_spec button;


// bluetooth read button callback

 static ssize_t write_led(struct bt_conn *conn,
                         const struct bt_gatt_attr *attr,
                         void *buf,
                         uint16_t len,
                         uint16_t offset)
{
    LOG_DBG("Attribute write handle: %u , conn %p",attr->handle,(void*)conn);
    if (len != 1U) {
		LOG_DBG("Write led: Incorrect data length");
		return BT_GATT_ERR(BT_ATT_ERR_INVALID_ATTRIBUTE_LEN);
	}

	if (offset != 0) {
		LOG_DBG("Write led: Incorrect data offset");
		return BT_GATT_ERR(BT_ATT_ERR_INVALID_OFFSET);
	}
    uint8_t val=*(uint8_t *)buf;
    if(val==0x1 || val==0x0){
        gpio_pin_set_dt(&blue,val?1:0);
    }
    else {
			LOG_DBG("Write led: Incorrect value");
			return BT_GATT_ERR(BT_ATT_ERR_VALUE_NOT_ALLOWED);
		}

     return len;
}
// referenced in user_gpio.h
int button_state;
// call back function for read button characteristic 
ssize_t read_button(struct bt_conn *conn,
                   const struct bt_gatt_attr *attr,
                    void *buf,
                     uint16_t len, 
                     uint16_t offset){

                    const char* value=attr->user_data;
                    LOG_DBG("Attribute read, handle: %u, conn: %p", attr->handle,
		              (void *)conn);
                     button_state=gpio_pin_get_dt(&button);
                     return bt_gatt_attr_read(conn,attr,buf,len,offset,value,sizeof(*value));
                  }
    //callback function for button characteristic configurator 
    bool indicate_enabled;

    // indicate eneabled call back 
 void button_ccc_cfg_changed(const struct bt_gatt_attr *attr,uint16_t value){
     indicate_enabled=(value==BT_GATT_CCC_INDICATE);
 }
 // function is called on acknowledge of an indication
 static void indicate_cb(struct bt_conn *conn,
			struct bt_gatt_indicate_params *params, uint8_t err)
{
	LOG_DBG("Indication %s\n", err != 0U ? "fail" : "success");
}
 
    BT_GATT_SERVICE_DEFINE(led_svc,
                           BT_GATT_PRIMARY_SERVICE(BT_UUID),
                           BT_GATT_CHARACTERISTIC(BT_BUTTON,
                                                  BT_GATT_CHRC_READ|BT_GATT_CHRC_INDICATE,
                                                  BT_GATT_PERM_READ,
                                                  read_button, NULL,
                                                  &button_state),
                         BT_GATT_CCC(button_ccc_cfg_changed,BT_GATT_PERM_READ|BT_GATT_PERM_WRITE),
                           BT_GATT_CHARACTERISTIC(BT_LED,
                                                  BT_GATT_CHRC_WRITE,
                                                  BT_GATT_PERM_WRITE,
                                                  NULL, write_led, NULL),
                           

    );
        struct bt_gatt_indicate_params ind_params;

    // indication function to be called in  button pressed interrupt
    int send_button_state_indicate(bool button_state){
      LOG_INF("state indication \n");
      if(!indicate_enabled){
        return -EACCES;
      }
      ind_params.attr=&led_svc.attrs[2];
      ind_params.func=indicate_cb;
      ind_params.destroy=NULL;
      ind_params.data=&button_state;
      ind_params.len=sizeof(button_state);
      LOG_DBG("finishing up\n");
      return bt_gatt_indicate(NULL, &ind_params);

 }

int bluetooth_init(void)
{

    int err = bt_enable(NULL);
    if (err)
    {
        LOG_ERR("bluetooth initialization failed %i \n", err);
        return 1;
    }
    // bluetooth phy configuration update

    // register callbacks
    bt_conn_cb_register(&connection_callbacks);
    // start advertising
    err = bt_le_adv_start(adv_param, adv_data, ARRAY_SIZE(adv_data), rsp_pkt, ARRAY_SIZE(rsp_pkt));
    if (err)
    {
        LOG_ERR(" Failed to start advertising\n");
        return 1;
    }
    return 0;
}