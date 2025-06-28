add_executable(dds
    main.c
    blink.c
    oled.c
    adc.c
    ads7868.c
    multi_core.c
    key.c
    rgb.c
    segment_scan.c
    angle.c
    dac.c
    )

# pull in common dependencies
target_link_libraries(dds pico_stdlib pico_multicore hardware_adc hardware_pwm hardware_i2c)

if (PICO_CYW43_SUPPORTED)
    target_link_libraries(dds pico_cyw43_arch_none)
endif()

# create map/bin/hex file etc.
pico_add_extra_outputs(dds)

# add url via pico_set_program_url
example_auto_set_url(dds)
