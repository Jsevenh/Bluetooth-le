#pragma once

// zephyr main dependancies
#include<zephyr/kernel.h>
#include<zephyr/device.h>
#include<zephyr/sys/printk.h>
#include<zephyr/logging/log.h>
#include<zephyr/drivers/gpio.h>
#include<zephyr/devicetree.h>

bool gpio_init();