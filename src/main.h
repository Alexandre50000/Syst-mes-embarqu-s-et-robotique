#ifndef MAIN_H
#define MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

#include <camera/dcmi_camera.h>
#include <msgbus/messagebus.h>
#include <parameter/parameter.h>


//constants for the differents parts of the project
#define IMAGE_BUFFER_SIZE		640
#define SLOPE_WIDTH 5
#define MIN_LINE_WIDTH 40
#define CONV_PX_CM 1560.0f

/** Robot wide IPC bus. */
extern messagebus_t bus;

extern parameter_namespace_t parameter_root;

void SendUint8ToComputer(uint8_t* data, uint16_t size);

#ifdef __cplusplus
}
#endif

#endif
