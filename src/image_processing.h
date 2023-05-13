#ifndef PROCESS_IMAGE_H
#define PROCESS_IMAGE_H

// Specify the 2 consecutive lines used for tracking the black line
// The line number starts from 0 and ending to PO8030_MAX_HEIGHT - 1. Consult camera/po8030.h
// But as 2 lines will be used, the value of the first line can be higher than PO8030_MAX_HEIGHT - 2
#define USED_LINE 200   // Must be inside [0..478], according to the above explanations

uint16_t extract_line_width(uint8_t *buffer);
uint16_t get_line_position(void);
uint8_t get_found(void);
float get_distance_cm(void);
void process_image_start(void);
void select_color_detection(uint16_t choice_detect_color);

#endif /* PROCESS_IMAGE_H */