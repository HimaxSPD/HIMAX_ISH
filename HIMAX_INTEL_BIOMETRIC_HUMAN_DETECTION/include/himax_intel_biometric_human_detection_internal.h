/*
TODO: Add copyright notice
*/



#ifndef HIMAX_INTEL_BIOMETRIC_HUMAN_DETECTION_INTERNAL_H
#define HIMAX_INTEL_BIOMETRIC_HUMAN_DETECTION_INTERNAL_H

#include "himax_intel_biometric_human_detection_defs.h"

//************************************************************************************************
// Debug prints
//************************************************************************************************
// This set of macros is made available to help you categorize debugging messages.
// Each time you print a debug message, prefix it with one of the macros, which will allow the trace tool to filter it out by both source and importance.
// The source of all messages coming from this sensor is by default LOG_COMP_SENSOR_APP, but you can change it to another value in the LOG_COMPONENTS enum
// found in D:\Intel_ISS_FDK\Tools\EclipsePlugin\FDKTools\versions\CNL\X.X.X.X\include.
// There are four levels of importance in descending order:

#define HIMAX_INTEL_BIOMETRIC_HUMAN_DETECTION_ID		LOG_COMP_SENSOR_APP
#define HIMAX_INTEL_BIOMETRIC_HUMAN_DETECTION_PRINT_MESSAGE		"HIMAX_INTEL_BIOMETRIC_HUMAN_DETECTION: "

#define HIMAX_INTEL_BIOMETRIC_HUMAN_DETECTION_ERROR(...)		syslog (LOG_ERR | HIMAX_INTEL_BIOMETRIC_HUMAN_DETECTION_ID, HIMAX_INTEL_BIOMETRIC_HUMAN_DETECTION_PRINT_MESSAGE __VA_ARGS__)

#define HIMAX_INTEL_BIOMETRIC_HUMAN_DETECTION_INFO(...)		syslog (LOG_INFO | HIMAX_INTEL_BIOMETRIC_HUMAN_DETECTION_ID, HIMAX_INTEL_BIOMETRIC_HUMAN_DETECTION_PRINT_MESSAGE __VA_ARGS__)

#define HIMAX_INTEL_BIOMETRIC_HUMAN_DETECTION_DEBUG(...)		syslog (LOG_DEBUG | HIMAX_INTEL_BIOMETRIC_HUMAN_DETECTION_ID, HIMAX_INTEL_BIOMETRIC_HUMAN_DETECTION_PRINT_MESSAGE __VA_ARGS__)
	
//#define HIMAX_INTEL_BIOMETRIC_HUMAN_DETECTION_INTERNAL_DBG
#ifdef HIMAX_INTEL_BIOMETRIC_HUMAN_DETECTION_INTERNAL_DBG
#define HIMAX_INTEL_BIOMETRIC_HUMAN_DETECTION_INTERNAL_DEBUG(...)		syslog (LOG_INFO | HIMAX_INTEL_BIOMETRIC_HUMAN_DETECTION_ID, HIMAX_INTEL_BIOMETRIC_HUMAN_DETECTION_PRINT_MESSAGE __VA_ARGS__)
#else
#define HIMAX_INTEL_BIOMETRIC_HUMAN_DETECTION_INTERNAL_DEBUG(...) 
#endif


//************************************************************************************************
// Private context of the algorithm (optional)
//************************************************************************************************
typedef struct
{
	// sc_sdt_info_entry calibration_info;
	// himax_intel_biometric_human_detection_public_properties public_properties;
	// Add other algorithm parameters that should be stored.

} himax_intel_biometric_human_detection_private_context;


#endif //HIMAX_INTEL_BIOMETRIC_HUMAN_DETECTION_INTERNAL_H


