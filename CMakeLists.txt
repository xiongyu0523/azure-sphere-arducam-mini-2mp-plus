#  Copyright (c) Microsoft Corporation. All rights reserved.
#  Licensed under the MIT License.

cmake_minimum_required(VERSION 3.10)

project(azure-sphere-arducam-mini-2mp-plus C)

azsphere_configure_tools(TOOLS_REVISION "20.04")
azsphere_configure_api(TARGET_API_SET "5") 
 
add_executable(${PROJECT_NAME} 
                main.c 
                arducam_driver/ArduCAM.c 
                arducam_driver/ll.c 
                arducam_driver/ov2640_regs.c 
                delay.c)

# Set the defines for the build
target_compile_definitions(${PROJECT_NAME} PUBLIC AzureSphere_CA7)
target_compile_definitions(${PROJECT_NAME} PUBLIC CFG_MODE_JPEG)

target_link_libraries(${PROJECT_NAME} applibs gcc_s c curl)

azsphere_target_hardware_definition(${PROJECT_NAME} TARGET_DIRECTORY "Hardware/mt3620_rdb" TARGET_DEFINITION "sample_hardware.json")

azsphere_target_add_image_package(${PROJECT_NAME} RESOURCE_FILES "certs/BaltimoreCyberTrustRoot.pem")
