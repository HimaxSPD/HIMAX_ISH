/*************************************************************************************************
 * IMPORTANT NOTE: THIS FILE IS AUTOMATICALLY GENERATED. DO NOT EDIT THIS FILE.
 * Any edits to this file may be overwritten or cause unexpected behavior
 * **********************************************************************************************/


#ifndef HIMAX_INTEL_BIOMETRIC_HUMAN_DETECTION_H
#define HIMAX_INTEL_BIOMETRIC_HUMAN_DETECTION_H


/*************************************************************************************************
 * Algorithm report buffer declaration
 * **********************************************************************************************/
struct _himax_intel_biometric_human_detection_report {
	sc_timestamp timestamp;
	uint32_t	distance;
	uint8_t	presence_confidence;
	uint8_t	presence;

} __attribute__ ((packed));
typedef struct _himax_intel_biometric_human_detection_report himax_intel_biometric_human_detection_report;


/*************************************************************************************************
 * HID descriptor definitions
 * **********************************************************************************************/

/* NOTE: HID descriptor must match the report buffer data fields (excluding timestamp).
 * Each data item should be added with its corresponding SC_USAGE_SENSOR_DATA_ value.
 * A data item with no native HID value should be added with value of
 * SC_USAGE_SENSOR_DATA_CUSTOM_VALUE_<i> */
#define SC_USAGE_SENSOR_DATA_HIMAX_INTEL_BIOMETRIC_HUMAN_DETECTION_DISTANCE		SC_USAGE_SENSOR_DATA_CUSTOM_VALUE_1
#define SC_USAGE_SENSOR_DATA_HIMAX_INTEL_BIOMETRIC_HUMAN_DETECTION_PRESENCE_CONFIDENCE		SC_USAGE_SENSOR_DATA_CUSTOM_VALUE_2
#define SC_USAGE_SENSOR_DATA_HIMAX_INTEL_BIOMETRIC_HUMAN_DETECTION_PRESENCE		SC_USAGE_SENSOR_DATA_CUSTOM_VALUE_3


/*************************************************************************************************
 * Vendor-defined properties that can be get/set by other sensors
 * **********************************************************************************************/
typedef struct {
	
} himax_intel_biometric_human_detection_public_properties;

/*************************************************************************************************
 * Init Function
 * **********************************************************************************************/
void HIMAX_INTEL_BIOMETRIC_HUMAN_DETECTION_init(IN sc_sdt_sensor_data *);

#endif /* HIMAX_INTEL_BIOMETRIC_HUMAN_DETECTION_H */

