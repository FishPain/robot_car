cmake_minimum_required(VERSION 3.12)

if (NOT PICO_NO_HARDWARE)

    if (NOT FREERTOS_KERNEL_PATH AND NOT DEFINED ENV{FREERTOS_KERNEL_PATH})
        message("\n\n")
        message("Skipping Pico W FreeRTOS examples as FREERTOS_KERNEL_PATH not defined")
        message("\n\n")
    else()
        include(FreeRTOS_Kernel_import.cmake)
        add_subdirectory(main)
        add_subdirectory(controller)
        add_subdirectory(dashboard)
    endif()
endif()


