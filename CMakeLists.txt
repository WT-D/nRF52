cmake_minimum_required(VERSION 3.20.0)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})

project(ess)

target_sources(app PRIVATE src/main.c src/ens160.c src/sht40.c src/oled.c src/gui.c src/gatt.c)
