#ifndef MAIN_H
#define MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

#include <camera/dcmi_camera.h>
#include <msgbus/messagebus.h>
#include <parameter/parameter.h>

/**Defines for other files to use*/

#define SOUND_1			32	//500Hz For base recognition
#define SOUND_2			36	//Around 560Hz Patrol Attack
#define SOUND_3			39	//Around 610HZ Patrol Defense
#define SOUND_4			42	//Around 650Hz Controlled Patrol
#define SOUND_5         48  // 750 Hz
#define NOSOUND         0   // When no new command was perceived

#define RED 		    1   // For camera color selection    
#define GREEN		    0
 
#define NORM			(16000/1024) // Used to normalise freq values to hertz


#define MAX_SPEED   1000 // [step/s]

//constants for the differents parts of the project
#define IMAGE_BUFFER_SIZE		640
#define WIDTH_SLOPE				5
#define MIN_LINE_WIDTH			40
#define PXTOCM					1570.0f //experimental value
#define GOAL_DISTANCE 			10.0f
#define MAX_DISTANCE 			25.0f

/** Robot wide IPC bus. */
extern messagebus_t bus;

extern parameter_namespace_t parameter_root;

#ifdef __cplusplus
}
#endif

#endif
