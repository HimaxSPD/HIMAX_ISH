/*
TODO: Add copyright notice
*/

// ************
// * INCLUDES *
// ************

// Include: SC API header file for definitions of SC APIs and callback
// Also used for various struct definitions
#include "sc_api.h"

#include "sensor_descriptor.h"
#include "sc_udriver_api.h"

// *********************
// * MACRO DEFINITIONS *
// *********************

#define SUCCESS 0

#define HIMAX_BIOMETRIC_HUMAN_DETECTION_HIMAX_DEBUG_STR "HIMAX_BIOMETRIC_HUMAN_DETECTION_HIMAX"

// I2C addresses of the sensor
#define HIMAX_BIOMETRIC_HUMAN_DETECTION_HIMAX_SENSOR_ADDRESS_1 0x62

#define HIMAX_BIOMETRIC_HUMAN_DETECTION_HIMAX_VALIDATE_ID_REG 0x01

#define HIMAX_BIOMETRIC_HUMAN_DETECTION_HIMAX_VALIDATE_ID_EXPECTED_VALUE 0x01

#define HIMAX_BIOMETRIC_HUMAN_DETECTION_HIMAX_SUPPORTED_FREQUENCIES {0}

#define HIMAX_BIOMETRIC_HUMAN_DETECTION_HIMAX_SUPPORTED_SCALES {0}


#define HIMAX_BIOMETRIC_HUMAN_DETECTION_HIMAX_GPIO_INPUT_GPIO3 3

#define HIMAX_HM01B0_MIN_RI 80

#define HIMAX_EVENT_PEOPLE_APPROACH 0
#define HIMAX_EVENT_PEOPLE_LEAVE 1
#define HIMAX_EVENT_PEEK_ALERT 2
#define HIMAX_EVENT_PEEK_DISMISS 3
#define HIMAX_EVENT_WATCH_SCREEN 4
#define HIMAX_EVENT_NOT_WATCH_SCREEN 5
// *********************
// * STRUCT DEFINITION *
// *********************
typedef struct _himax_biometric_human_detection_himax_private_data
{
	uint8_t gpio_support;
	uint8_t human_deteted;
	uint8_t not_alone;
	uint8_t not_facing;
}himax_biometric_human_detection_himax_private_data;


// ********************************
// * FUNCTION FORWARD DECLARATION *
// ********************************

static sc_status himax_biometric_human_detection_himax_read_data(udriver_context*, sc_num_samples*, void*);
static sc_status himax_biometric_human_detection_himax_self_test(udriver_context*, sc_test_level, sc_self_test_result*);
static sc_status himax_biometric_human_detection_himax_set_report_interval(udriver_context*,sc_sensor_report_interval*);
static void himax_biometric_human_detection_himax_get_descriptor(sc_sensor_descriptor *descriptor);
static sc_status himax_biometric_human_detection_himax_test_sensitivity (IN sc_sensitivity_test_input* , INOUT sc_sensitivity_test_output* );
static sc_status himax_biometric_human_detection_himax_gpio_notify(udriver_context*,uint8_t);
static sc_status himax_biometric_human_detection_himax_gpio_config(udriver_context* ctx);
static sc_status himax_biometric_human_detection_himax_ioctl(udriver_context* ctx, sc_ioctl_id ioctl_id, uint16_t input_buffer_length, uint16_t *output_buffer_length, uint8_t *input_buf, uint8_t *output_buf);
static bool mh_algo_human_presence();
