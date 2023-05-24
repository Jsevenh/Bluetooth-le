 cmake_minimum_required(VERSION 3.26.3)

 set(BOARD esp32)

 find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
 
 project(bt)

 target_sources(app PRIVATE src/main.c  bt/bt.c  src/user_gpio.c)