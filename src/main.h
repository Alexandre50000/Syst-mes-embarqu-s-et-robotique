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
#define NOSOUND         0   // When no new command was perceived
 
#define NORM			(16000/1024) // Used to normalise freq values to hertz



/** Robot wide IPC bus. */
extern messagebus_t bus;

extern parameter_namespace_t parameter_root;

/**Mailbox for commands*/
extern messagebus_topic_t Command_topic;
    
#ifdef __cplusplus
}
#endif

#endif
