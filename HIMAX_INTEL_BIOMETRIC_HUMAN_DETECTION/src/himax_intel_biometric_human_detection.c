/*
TODO: Add copyright notice
*/



//************************************************************************************************
// Debug prints
//************************************************************************************************
#include "himax_intel_biometric_human_detection_internal.h"

// Include the mathematical library if required
#include "math_lib.h"

// Include reporter's header
//#include "HIMAX_ALGO.h"


//add other include files here

//************************************************************************************************
// himax_intel_biometric_human_detection_init callback
//************************************************************************************************
void HIMAX_INTEL_BIOMETRIC_HUMAN_DETECTION_init(IN sc_sdt_sensor_data* sdt)
{
	algo_context *context;
	himax_intel_biometric_human_detection_private_context *private_context;
	
	//sensor_create is an automatically generated sensor initialization function. It configures the sensor and its reporters.
	//Any edits you make to the function will be erased.
	himax_intel_biometric_human_detection_create(&context, (void **)&private_context, sdt, sizeof(himax_intel_biometric_human_detection_private_context));

	//Any code you wish to perform during sensor initialization, such as setting the values of algorithm parameters
	//or configuring GPIO interrupts, should be added here.
}

//************************************************************************************************
// himax_intel_biometric_human_detection_process_data callback
//************************************************************************************************
sc_status himax_intel_biometric_human_detection_process_data(IN algo_context *context,
								   INOUT void *private_data,
								   IN uint8_t samples_num,
								   IN reporter_sample_info *samples)
{
	//HIMAX_INTEL_BIOMETRIC_HUMAN_DETECTION_ERROR("report data");
	sc_status res = SC_STATUS_SUCCESS;
	// TODO: Implement this function
	himax_intel_biometric_human_detection_report out_sample = {0};
	human_presence_phy_data_t *data1 = (human_presence_phy_data_t *)samples[HIMAX_INTEL_BIOMETRIC_HUMAN_DETECTION_HIMAX_ALGO_INDEX].data_sample;
	out_sample.timestamp = data1->timestamp;
	out_sample.presence = data1->present;
	if(data1->present)
	{
		out_sample.distance = 500;
		out_sample.presence_confidence = 100;
	}
	else
	{
		out_sample.distance = 100;
		out_sample.presence_confidence = 1;
	}

	//human_presence_phy_data_t *data1 = (human_presence_phy_data_t *)samples[HIMAX_BIOMETRIC_PRESENCE_HIMAX_ALGO_INDEX].data_sample;
	//out_sample.timestamp = data1->timestamp;
	//himax_intel_biometric_human_detection_report out_sample = {0};
    //himax_intel_biometric_human_detection_private_context *priv_context = 
    //    (himax_intel_biometric_human_detection_private_context *)private_data;
    
	// TODO: Retrieve each of your reporters¡¦ data samples from the *samples array.
	// Each of your reporters has provided a data sample, under samples[index_number].data_sample.
	// You will need to properly cast each data sample in order to use it, as follows:
	// TODO: INSERT_TYPE *data1 = (INSERT_TYPE *)samples[HIMAX_INTEL_BIOMETRIC_HUMAN_DETECTION_HIMAX_ALGO_INDEX].data_sample;
	// Each reporter also indicates, under samples[index_number].flags, the following information:
	//     * Use if(samples[index_number].flags & SC_REPORTER_SAMPLE_INFO_NEW_DATA) to determine if the reporter has provided a new data sample since the last time this function was called.
	//     * Use if(samples[index_number].flags & SC_REPORTER_SAMPLE_INFO_VALID) to determine if the data provided by the reporter is old but nonetheless valid.
	// You can use these flags to adjust your algorithm to handle any desired combination of new, old, and missing data. See the Developer¡¦s Guide in the documentation for details and examples.

	// writing to the SDT for saving values after power off. 

	// This operation should not be performed frequently (no more than once a day) so as to prevent flash wear-out. 
	// Note: This is a long operation that is performed in the background, so the return value does not reflect the success of the operation.
	// algo_set_sdt_info_entry(context, SC_SDT_CALI_INFO, (void *)priv_context->sdt_info_entry);
	
    // post a report buffer if you have new data. Set the timestamp  of your data accordingly
    // e.g.: 
	algo_post_data(context, (void *)(&out_sample), 0);

	return res;
}

//************************************************************************************************
// himax_intel_biometric_human_detection_clear_context callback
//************************************************************************************************
void himax_intel_biometric_human_detection_clear_context(IN algo_context *context,
							   INOUT void *private_data)
{
    //himax_intel_biometric_human_detection_private_context *priv_context = 
    //    (himax_intel_biometric_human_detection_private_context *)private_data;

    HIMAX_INTEL_BIOMETRIC_HUMAN_DETECTION_DEBUG("clear context\n");

    // Clear all relevant fields in priv_context that are state-related
    // e.g., timestamp, algorithm state
	
	// ATTENTION: clear_context() is called whenever the sensor is activated.
    // If you have fields that should be initialized only during ISS reset (e.g., calibration data from SDT)
    // do not initialize them here.
}

//************************************************************************************************
// himax_intel_biometric_human_detection_sensitivity_test callback
//************************************************************************************************
void himax_intel_biometric_human_detection_sensitivity_test(IN algo_context *context,
                                  IN void *private_data, 
                                  IN sc_sensitivity_test_input *input, 
                                  OUT sc_sensitivity_test_output *output)
{
	HIMAX_INTEL_BIOMETRIC_HUMAN_DETECTION_DEBUG("sensitivity test\n");
    himax_intel_biometric_human_detection_report *new_sample =
        (himax_intel_biometric_human_detection_report *)input->new_sample;

    himax_intel_biometric_human_detection_report *old_sample =
        (himax_intel_biometric_human_detection_report *)input->old_sample;
    /*
     * sensitivity = 0 . output it.
     * sensitivity != 0 . check all field of output structure.
     * */



    if(input->value) {
    	uint8_t data_ready = new_sample->presence != old_sample->presence;
		data_ready = data_ready || abs((int32_t)(new_sample->distance - old_sample->distance)) >= input->value;
		data_ready = data_ready || new_sample->presence_confidence != old_sample->presence_confidence;
		output->threshold_reached = data_ready;
    	/* himax
    	uint8_t data_ready = new_sample->distance != old_sample->distance;
    	data_ready |= new_sample->presence != old_sample->presence;
    	data_ready |= new_sample->presence_confidence != old_sample->presence_confidence;
    	output->threshold_reached = data_ready;*/
    } else {
    	output->threshold_reached = 1;
    }

    // Set the condition in which the samples differ and should be passed to the sensor
    // e.g.: output->threshold_reached = (abs(old->counter - new->counter) >= input->value);
}

//************************************************************************************************
// himax_intel_biometric_human_detection_notify_event callback
//
// Consider implementing this callback if:
// - You would like to handle events that are not supported by default.
// - You would like to handle events differently from the default handling.
//
//************************************************************************************************
sc_status himax_intel_biometric_human_detection_notify_event ( INOUT algo_context *context,
			INOUT void *private_data,
			IN sc_event_notification_event_id event_id,
			IN sc_event_info event_info )
{
	// TODO: Implement this function
	sc_status res = SC_STATUS_SUCCESS;
	HIMAX_INTEL_BIOMETRIC_HUMAN_DETECTION_DEBUG("notify_event\n");

	switch(event_id)
		{
			//Add to the switch case the events that the algorithm is handling
			//e.g.:
			case SC_NOTIFY_HOST_OS_STATE_CHANGE_CS:
			{
				if(event_info.sc_notify_sensor_sys_state.system_state_statuses & SYS_STATE_CONNECTED_STANDBY)
				{
					HIMAX_INTEL_BIOMETRIC_HUMAN_DETECTION_ERROR("ENTER modern standby");
				}
				else
				{
					HIMAX_INTEL_BIOMETRIC_HUMAN_DETECTION_ERROR("EXIT modern standby");
				}
			}
			break;
			default:
			break;
		}

	return res;
}

//************************************************************************************************
// himax_intel_biometric_human_detection_ioctl callback
//************************************************************************************************
sc_status himax_intel_biometric_human_detection_ioctl ( IN algo_context *context,
			IN sc_ioctl_id ioctl_id,
			IN uint16_t input_buffer_length,
			INOUT uint16_t *output_buffer_length,
			IN uint8_t *input_buf,
			OUT uint8_t *output_buf )
{
	// TODO: Implement this function
	HIMAX_INTEL_BIOMETRIC_HUMAN_DETECTION_DEBUG("ioctl\n");
	sc_status res = SC_STATUS_SUCCESS;

	// Put your code here

	return res;
}
#if 0
sc_status himax_intel_biometric_human_detection_get_config( IN algo_context *context,
		IN void *private_data,
		INOUT sc_sensor_item_value *item_value )
{
	sc_status res = SC_STATUS_SUCCESS;
	switch (item_value->usage) {
	/* get the max distance range */
		//case SC_SENSOR_ITEM_USAGE_RANGE_MAXIMUM: {
	case 1: {
			*(uint16_t *)item_value->value = 1000; // unit is mm
		}
		break;
	/* get the min distance range */
		//case SC_SENSOR_ITEM_USAGE_RANGE_MINIMUM: {
	case 2: {
			*(uint16_t *)item_value->value =0; // unit is mm
		}
		break;
		default:
			break;
	}
	return res;

}

sc_status human_detection_set_config( IN algo_context *context,
		INOUT void *private_data,
		INOUT sc_sensor_item_value *item_value )
{
	sc_status res = SC_STATUS_SUCCESS;
	switch (item_value->usage) {
		case SC_SENSOR_ITEM_USAGE_VENDOR_DEFINED_CONFIG: {
			if (item_value->data_size == sizeof(threshold_low_set_config))
			{
				threshold_low_set_config config = *((threshold_low_set_config *)(item_value->value));
				if (config.client_info.sub_usage_id == SUB_USAGE_HUD_THRESHOLD_LOW) {
					if ((config.client_info.client_id & 0xFFFF) == SC_SENSOR_TYPE_BIOMETRIC_PRESENCE &&
							(config.client_info.client_id & 0x1000000) == 0 /* Not a custom sensor */) {
						// the property value can be read from config.threshold_low
					}
				}
			}
		}
			break;
		default:
			break;
	}
	return res;
}

#endif






























/************************************************************************************************
 * himax_intel_biometric_human_detection_get_config callback
 *
 * Consider implementing this callback if:
 * - You would like to implement a get/set configuration interface
 * - You would like to define and implement a new usage with a get/set configuration
 ************************************************************************************************/
sc_status himax_intel_biometric_human_detection_get_config(IN algo_context *context,
			IN void *private_data,
			INOUT sc_sensor_item_value *item_value)
{
	/* TODO: Implement this function */
	sc_status res = SC_STATUS_SUCCESS;
	HIMAX_INTEL_BIOMETRIC_HUMAN_DETECTION_DEBUG("get_config %d \n", item_value->usage);

	switch (item_value->usage) {
	/*Add the supported usages to the switch case
		e.g.:
		case SC_SENSOR_ITEM_USAGE_VENDOR_DEFINED_CONFIG:
		{
			Copy the configuration buffer from private_data->public_properties to item_value->value
			himax_intel_biometric_human_detection_private_context *priv_context = (himax_intel_biometric_human_detection_private_context *)private_data;
			*((himax_intel_biometric_human_detection_public_properties *)item_value->value) = priv_context->public_properties;
		}
			break;*/

	/* get the max distance range */
	//case SC_SENSOR_ITEM_USAGE_RANGE_MAXIMUM:
	case SC_USAGE_SENSOR_PROPERTY_RANGE_MAXIMUM:
	{
		*(uint16_t *)item_value->value = 2000; // unit is mm
	}
		break;
	/* get the min distance range */
	//case SC_SENSOR_ITEM_USAGE_RANGE_MINIMUM:
	case SC_USAGE_SENSOR_PROPERTY_RANGE_MINIMUM:
	{
		*(uint16_t *)item_value->value =20; // unit is mm
	}
		break;
	default:
		break;
	}
	return res;

}

/*************************************************************************************************
 * himax_intel_biometric_human_detection_set_config callback
 *
 * Consider implementing this callback if:
 * - You have a reporter whose configuration you manage manually
 * - You would like to implement a get/set configuration interface
 ************************************************************************************************/
sc_status himax_intel_biometric_human_detection_set_config(IN algo_context *context,
			INOUT void *private_data,
			INOUT sc_sensor_item_value *item_value)
{
	/* TODO: Implement this function */
	sc_status res = SC_STATUS_SUCCESS;
	/*himax_intel_biometric_human_detection_private_context *priv_context = (himax_intel_biometric_human_detection_private_context *)private_data;*/
	HIMAX_INTEL_BIOMETRIC_HUMAN_DETECTION_DEBUG("set_config\n");

	switch (item_value->usage) {
	/*Add supported usages to the switch case:
	e.g.:
	case SC_SENSOR_ITEM_USAGE_VENDOR_DEFINED_CONFIG:
	{
		Put your code here.
		E.g. Copy the configuration buffer to public_properties, found under private_context
		priv_context->public_properties = *((himax_intel_biometric_human_detection_public_properties *)item_value->value);
	}
		break;*/
	default:
		break;
	}

	return res;
}









