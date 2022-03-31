/*
TODO: Add copyright notice
*/

#include "himax_biometric_human_detection_himax.h"
#include "himax_biometric_human_detection_himax_MetaData.h"

/******************************************************************************
 Description:
 Initializes the physical sensor and registers its context into the sensor core FW.
 This function is responsible for:
 1)	Create the sensor in the sensor core FW and obtain its context.
 2)   initialize the sensor's private data (optional)get_descriptor
 3)	Validating the sensor presence 
 			a) validate the sensor i2c address or setup spi device
			b) call udriver_validate_id function to test sensor connectivity
 4)	register FIFO if supported
 5)	register GPIO if supported
 6)	Setting the sensor to its default configuration
 7)	Setting the sensor to READY state

 Input:
 HIMAX_BIOMETRIC_HUMAN_DETECTION_HIMAX_sdt -	the sensor sdt information
 ******************************************************************************/
void HIMAX_BIOMETRIC_HUMAN_DETECTION_HIMAX_init(sc_sdt_sensor_data *HIMAX_BIOMETRIC_HUMAN_DETECTION_HIMAX_sdt)
{
	//TODO: Implement this function
	sc_status ret = SC_STATUS_SUCCESS;
	//Step 1: Call udriver_create_sensor to register the sensor in the Sensor Core FW
	udriver_context *ctx;
	// default requirement for custom is 100Hz
	sc_sensor_report_interval min_ri = HZ_TO_SC_REPORT_INTERVAL(100);

	//This is a list of callback functions that may be called by the sensor core
	//All the callbacks in this list must be implemented.
	udriver_callback_list himax_biometric_human_detection_himax_callback_list = {
		.read_data_f = &himax_biometric_human_detection_himax_read_data,
		.self_test_f = &himax_biometric_human_detection_himax_self_test,
		.set_report_interval_f = &himax_biometric_human_detection_himax_set_report_interval,
		.test_sensitivity_f = &himax_biometric_human_detection_himax_test_sensitivity,
		.get_descriptor_f  = &himax_biometric_human_detection_himax_get_descriptor
	};

	ret = udriver_create_sensor(
		HIMAX_BIOMETRIC_HUMAN_DETECTION_HIMAX_sdt,
		min_ri,
		UDRIVER_ATTRIBUTE_I2C_REPEATED_START_DISABLED,//UDRIVER_ATTRIBUTE_NO_ATTRIBUTES
		&himax_biometric_human_detection_himax_callback_list,
		sizeof(himax_biometric_human_detection_himax_private_data),
		&ctx
	);

	//udriver_set_timer (ctx,1000);

	if (unlikely(ret))
	{
		UDRIVER_PRINT("%s %d: [%s] create context has failed\n",__func__,__LINE__,HIMAX_BIOMETRIC_HUMAN_DETECTION_HIMAX_DEBUG_STR);
		return;
	}

	//Step 2: initialize the sensor private data
	//TODO: if you have any private data of the sensor, it needs to be initialized here.
	himax_biometric_human_detection_himax_private_data *private_data =
			(himax_biometric_human_detection_himax_private_data *)udriver_get_private_data(ctx);
	private_data->gpio_support = 0;

	//leo add;
	private_data->not_facing = 0;
	private_data->human_deteted = 0;
	private_data->not_alone = 0;

	/*
	//Step 3: validate sensor presence (optional)
	//TODO: please implement a simple test to check that the sensor is connected and the i2c is working.
	ret = udriver_i2c_validate_id(ctx, HIMAX_BIOMETRIC_HUMAN_DETECTION_HIMAX_VALIDATE_ID_REG, HIMAX_BIOMETRIC_HUMAN_DETECTION_HIMAX_VALIDATE_ID_EXPECTED_VALUE);
	if (unlikely(ret))
	{
		UDRIVER_PRINT("%s %d: [%s] validate id has failed\n",__func__,__LINE__,HIMAX_BIOMETRIC_HUMAN_DETECTION_HIMAX_DEBUG_STR);
		return;
	}
	*/

	//Step 4: register GPIO
	//TODO: Individually configure each GPIO input, using udriver_get_gpio_info and udriver_gpio_config.
	ret = himax_biometric_human_detection_himax_gpio_config(ctx);

	if (unlikely(ret))
	{
		UDRIVER_PRINT("%s %d: [%s] notify gpio event has failed\n",__func__,__LINE__,HIMAX_BIOMETRIC_HUMAN_DETECTION_HIMAX_DEBUG_STR);
		return;
	}

	udriver_register_ioctl_callback(ctx,&himax_biometric_human_detection_himax_ioctl);//Leo add ioctl

	//Step 5: setting the sensor state to READY
	ret = udriver_update_sensor_state(ctx,SC_SENSOR_STATE_READY);
	if (unlikely(ret))
	{
		UDRIVER_PRINT("%s %d: [%s] create context has failed\n",__func__,__LINE__,HIMAX_BIOMETRIC_HUMAN_DETECTION_HIMAX_DEBUG_STR);
		return;
	}

}

static sc_status himax_biometric_human_detection_himax_gpio_config(udriver_context* ctx)
{
	sc_status ret = SC_STATUS_SUCCESS;
	sc_sdt_gpio_info *p_gpio_info = udriver_get_gpio_info(ctx);
	himax_biometric_human_detection_himax_private_data *private_data =
				(himax_biometric_human_detection_himax_private_data *)udriver_get_private_data(ctx);
	if (p_gpio_info && (p_gpio_info->gpio_entry != NULL) && (p_gpio_info->num_gpio_entries > 0))
	{
		uint8_t gpio_id;
		sc_gpio_mode gpio_mode;
		gpio_id = p_gpio_info->gpio_entry[0].gpio_id;
		//gpio_mode = p_gpio_info->gpio_entry[0].gpio_mode;
		gpio_mode = (p_gpio_info->gpio_entry[0].gpio_mode ==
						SC_GPIO_MODE_ACTIVE_HIGH_PUSH_PULL?SC_GPIO_MODE_RISING_EDGE:SC_GPIO_MODE_FALLING_EDGE);

		/*
		 * First disable the sensor's polling by switching to async mode, then enable the interrupt.
		 */
		sc_config_gpio_params gpio_cfg = {
										   SC_GPIO_REGISTER,
										   gpio_id,
										   gpio_mode,
										   SC_GPIO_DIRECTION_INPUT };

		ret = udriver_gpio_config(ctx, &gpio_cfg);
		if (unlikely(ret))
		{
			return ret;
		}
		//udriver_config_polling_mode(ctx, DISABLE_POLLING_MODE);

		ret = udriver_register_notify_gpio_event(
				ctx, &himax_biometric_human_detection_himax_gpio_notify);

		if (unlikely(ret))
		{
			return ret;
		}
		private_data->gpio_support = 1;
	}
	return ret;
}

/******************************************************************************
 Description:
 Read sensor output data from the sensor
 and convert the raw data into human-friendly values
 Input:
 		ctx 		- the context of the driver
 		sample_cnt	- the number of samples to read
		data_buf 	- the buffer to write to
 Return:
 			integer value corresponding to success (0) or failure (non-zero value)
 ******************************************************************************/
static sc_status himax_biometric_human_detection_himax_read_data(udriver_context* ctx, sc_num_samples *sample_cnt, void *data_buf)
{
	//TODO: Implement this function
	//extract the private data if you plan to use it in this function
	//himax_biometric_human_detection_himax_private_data *private_data = (himax_biometric_human_detection_himax_private_data *)udriver_get_private_data(ctx);
	sc_status ret = SC_STATUS_SUCCESS;
	//int output_length;
	//TODO: Initialize output_length with the number of output values the sensor has

	//uint16_t raw_data[*sample_cnt * output_length];
	//TODO: Declare data_out pointer to data_buf.
	//For example: accel_phy_data_t* data_out = (accel_phy_data_t*) data_buf;

	//The timestamp field does not need to be filled. It will be set by the sensor core.

	//Step 1: Read the raw data from the sensor into raw_data
	//ret = udriver_i2c_read (ctx, , , );
	//if (unlikely(ret))
	//{
		//return SC_STATUS_EPARAM;
	//}

	//TODO: Apply scaling
	return ret;
}

/******************************************************************************
 Description:
 implements tests that the sensor might use in order to check its functionality
 Input:
             ctx    - The driver's context.
             level  - The level of the test, where:
                    SC_TEST_LEVEL_1            : Tests that the sensor can be reached by accessing a known register.
                                                 Every sensor should implement this. If not implemented, the sensor should return SC_SELF_TEST_PASS.
                                                 Output: SC_SELF_TEST_PASS or SC_SELF_TEST_FAIL.
                    SC_TEST_LEVEL_2            : This test level, which formerly ensured that the calibration data is valid, should not be implemented here.
                                                 Sensors that require calibration data should verify the calibration using the calibration_verify callback function instead.
                    SC_TEST_LEVEL_3            : Used mostly in third-party extensibility. To be implemented if extensive tests other than those specified above are to be performed.
                                                 Output: SC_SELF_TEST_SUCCESS or SC_SELF_TEST_FAIL
             result  - where the result will be saved
 ******************************************************************************/
static sc_status himax_biometric_human_detection_himax_self_test (udriver_context *ctx, sc_test_level level, sc_self_test_result *result)
{
	//TODO: Implement this function
	//extract the private data if you plan to use it in this function
	//himax_biometric_human_detection_himax_private_data *private_data = (himax_biometric_human_detection_himax_private_data *)udriver_get_private_data(ctx);
	sc_status ret = SC_STATUS_SUCCESS;
	//TODO: add any additional test levels that you would like to test the sensor with according to the API
	switch(level)
	{
	case SC_TEST_LEVEL1:
		ret = udriver_i2c_validate_id(ctx, HIMAX_BIOMETRIC_HUMAN_DETECTION_HIMAX_VALIDATE_ID_REG, HIMAX_BIOMETRIC_HUMAN_DETECTION_HIMAX_VALIDATE_ID_EXPECTED_VALUE);
		if (ret == SC_STATUS_SUCCESS)
			*result = SC_SELF_TEST_PASS;
		else
			*result = SC_SELF_TEST_CONNECTIVITY_FAILURE;
		break;
	case SC_TEST_LEVEL3:
		//TODO: If the sensor supports the HW self-test, trigger it here.
		*result = SC_SELF_TEST_PASS;
		break;
	default:
		*result = SC_SELF_TEST_UNSUPPORTED;
		break;
	}

	return ret;
}

/******************************************************************************
 Description:
	configures the sensor in the best way that will support the requested report interval

 Input:
	ctx				- pointer to the sensor's context
	report_interval	- the new report interval to support.
					  when report interval = 0, sensor should shut down

 Return:
	integer value corresponding to success (0) or failure (non-zero value)
 ******************************************************************************/
static sc_status himax_biometric_human_detection_himax_set_report_interval (udriver_context *ctx, sc_sensor_report_interval* report_interval)
{
	//TODO: Implement this function
	//extract the private data if you plan to use it in this function
	//himax_biometric_human_detection_himax_private_data *private_data = (himax_biometric_human_detection_himax_private_data *)udriver_get_private_data(ctx);
	sc_status ret = SC_STATUS_SUCCESS;
	if (*report_interval == 0)
	{
		//TODO: set the sensor to power down mode
		return ret;
	}
	//TODO: Set the best match frequency your sensor supports and return it in the report_interval parameter.
	//This is mandatory for all sensors in order to know the exact frequency the sensor chose and better query sample for algorithms.
	return ret;
}

/******************************************************************************
 Description:
		get the sensor HID descriptor

 Input:
		descriptor 	- pointer to the descriptor to be filled 

 Return:
		integer value corresponding to success (0) or failure (non-zero value)
 ******************************************************************************/
static void himax_biometric_human_detection_himax_get_descriptor(sc_sensor_descriptor *descriptor)
{	
/*
		sc_sensor_item_description *item;
		(void)build_human_presence_descriptor(descriptor);
*/
		sensor_descriptor_init(descriptor, SC_SENSOR_TYPE_INTEL_BIOMETRIC_HUMAN_DETECTION, SC_DESC_FLAG_HID_CUSTOM_SENSOR);
		sensor_descriptor_add_sensitivity_item(descriptor,SC_USAGE_SENSOR_DATA_CUSTOM_VALUE_1, 0, true);

		sensor_descriptor_add_data_item_uint32(descriptor,SC_USAGE_SENSOR_DATA_CUSTOM_VALUE_1, 0);
		sensor_descriptor_add_data_item_uint8(descriptor,SC_USAGE_SENSOR_DATA_CUSTOM_VALUE_2, 0);
		sensor_descriptor_add_data_item_uint8(descriptor,SC_USAGE_SENSOR_DATA_CUSTOM_VALUE_3, 0);







	//TODO: you either chose not to use the default descriptor or your sensor type doesn't have a default descriptor
	//please build a descriptor for your sensor, and put it inside descriptor
	//TODO: please refer to sensor_descriptor.h file if you want to modify your descriptor in the future
}


/******************************************************************************
 Description:
		checks if the current sample crossed the sensitivity threshold
		this callback is mandatory, and should be passed to udriver_create_sensor
 Input:
		context - pointer to the sensor's context
		input   - the previous sample, the new sample, and the current sensitivity
			see sc_sensitivity_test_input documentation
 Output:
		output- truealse. did the new sample cross the sensitivity threshold?
			see sc_sensitivity_test_output documentation
 Return:
		sc_status value corresponding to success(0) or failure(non-zero value)
******************************************************************************/
static sc_status himax_biometric_human_detection_himax_test_sensitivity (IN sc_sensitivity_test_input* input, INOUT sc_sensitivity_test_output* output)
{
	//TODO: Implement this function
	//This sample code checks the sensitivity of a sensor that uses uint32_t data:
	//uint8_t res = 0;
	//phy_sensor_data_u32_t *old_data = (phy_sensor_data_u32_t *)input->old_sample;
	//phy_sensor_data_u32_t *new_data = (phy_sensor_data_u32_t *)input->new_sample;
	//sc_sensor_sensitivity threshold = input->value
	//res = (abs(old_data->data - new_data->data) >= threshold);
	//output->threshold_reached = (res != 0);
	//We use the generic test_sensitivity call instead of implementing it ourselves.
	//TODO: you either chose not to use the default descriptor or your sensor type doesn't have a default descriptor
	//please build a descriptor for your sensor, and put it inside descriptor

	//output->threshold_reached = true;
	//return SC_STATUS_SUCCESS;
	return udriver_boolean_data_test_sensitivity(input, output);
}


bool mh_algo_human_presence()
{

	//UDRIVER_PRINT("**mh_algo**\n");

	/*
	 * Condition of True : A| B
	 *  A:frontal face detection (any distance)
	 *  B:distance < 1 meter (any head posing)
	 * Condition of False
	 *  no presence for 2 seconds (as many frames as posible)
	 */

	#define FRONTAL 1
	#define NON_FRONTAL 2
	#define NON_PEOPLE 3
	#define COUNT_TOTAL 10


	int face_class_type = 1;
	int distance_value = 1;
	int count = 0;
	bool human_presence_flag = false;

	if (face_class_type == FRONTAL || distance_value < 1)
	{
		human_presence_flag = true;
	}
	else
	{
		for (count = 0 ; count < COUNT_TOTAL; count++)
		{
			human_presence_flag = false;
		}
	}


/*
	switch(face_class_type)
	{
		case FRONTAL:
			human_presence_flag = true;
			break;
		case NON_FRONTAL:
			if(distance_value < 1)
			{
				human_presence_flag = true;

			}
			else{
				human_presence_flag = false;
			}

			break;
		case NON_PEOPLE:
			human_presence_flag = false;
			break;
		default:
			break;
	}
*/
	return human_presence_flag;
}
/******************************************************************************
 Description:
		Notifies the sensor about a GPIO event.
		This is an optional callback, A uDriver will implement this callback if it
		is connected through GPIO
		This callback is set through the function udriver_register_notify_gpio_event().
 Input:
		ctx - pointer to the sensor's context
		gpio_id - indicates the gpio_id that was triggered
 Return:
		sc_status value corresponding to success(0) or failure(non-zero value)
******************************************************************************/
int count = 0;

sc_status himax_biometric_human_detection_himax_gpio_notify(udriver_context* ctx, uint8_t gpio_id)
{
	//TODO: Implement this function
	//extract the private data if you plan to use it in this function

	sc_sdt_gpio_info *gpio_info = udriver_get_gpio_info(ctx);
	sc_sdt_gpio_entry* gpio_entry = NULL;
	uint8_t buf_PD[80];
	uint16_t reason;
	uint8_t event = 0;
	uint8_t value = 0;

	himax_biometric_human_detection_himax_private_data *private_data = (himax_biometric_human_detection_himax_private_data *)udriver_get_private_data(ctx);
	UDRIVER_PRINT("enter function himax_biometric_human_detection_himax_gpio_notify! count : %d  \n",count);
	for(int i = 0; i < gpio_info->num_gpio_entries; i++)
	{
		if (gpio_info->gpio_entry[i].gpio_id == gpio_id)
		{
			gpio_entry = &gpio_info->gpio_entry[i];
			//UDRIVER_PRINT("gpio_mode: %d  \n",gpio_info->gpio_entry[i].gpio_mode);
			break;
		}
	}



	uint8_t buf_writer[6];
	sc_status ret = SC_STATUS_SUCCESS;
	buf_writer[0] = 0x00;
	buf_writer[1] = 0x02;
	buf_writer[2] = 0x00;
	buf_writer[3] = 0x00;
	buf_writer[4] = 0x99; //crc
	buf_writer[5] = 0xb6; //crc
/*
	buf_writer[0] = 0x02;
	buf_writer[1] = 0x00;
	buf_writer[2] = 0x00;
*/
	count = count + 1 ;


	//udriver_i2c_write(ctx, 0x00, (uint8_t*) &buf_writer, 4);
	//ret = i2c_write_raw(1, 0x62 , (uint8_t*) &buf_writer, 6 ,&reason , 0);
	//UDRIVER_PRINT("1 udriver_i2c_write 0x62 cmd 00 02 00 00! ret : %d \n", ret);

	//ret = i2c_read_raw(1, 0x62 ,(uint8_t*) &buf_PD ,71 ,&reason , 0);
	//UDRIVER_PRINT("1 ret : %d i2c data: %d %d %d %d %d %d type: %d %d\n", ret , buf_PD[0], buf_PD[1], buf_PD[2], buf_PD[3], buf_PD[4], buf_PD[5],
	//																buf_PD[9],buf_PD[10]);


	//UDRIVER_PRINT("gpio_entry->gpio_purpose : %d  \n",gpio_entry->gpio_purpose);

	human_presence_phy_data_t data_out;
	if (gpio_entry != NULL)
	{
		switch(gpio_entry->gpio_purpose)
		{
			case HIMAX_BIOMETRIC_HUMAN_DETECTION_HIMAX_GPIO_INPUT_GPIO3:
				//TODO: put your code here
				ret = i2c_read_raw(1, 0x62 ,(uint8_t*) &buf_PD ,71 ,&reason , 0);
				//ret = i2c_read_raw(1, 0x62 ,(uint8_t*) &buf_PD ,52 ,&reason , 0);
				UDRIVER_PRINT("1 ret : %d i2c data: %d %d %d %d %d %d type: %d %d\n", ret , buf_PD[0], buf_PD[1], buf_PD[2], buf_PD[3], buf_PD[4], buf_PD[5],
																					buf_PD[6],buf_PD[7]);

				UDRIVER_PRINT("ret : %d  data type: %d %d \n", ret , buf_PD[8], buf_PD[9]);
				mh_algo_human_presence();

				switch(buf_PD[8]){
					case HIMAX_EVENT_PEOPLE_APPROACH:
						data_out.present = HUMAN_PRESENCE_DETECTED;
						private_data->human_deteted = HUMAN_PRESENCE_DETECTED;
						event = HIMAX_EVENT_PEOPLE_APPROACH;
						break;
					case HIMAX_EVENT_PEOPLE_LEAVE:
						data_out.present = HUMAN_PRESENCE_NOT_DETECTED;
						private_data->human_deteted = HUMAN_PRESENCE_NOT_DETECTED;
						event =	HIMAX_EVENT_PEOPLE_LEAVE;
						break;
					case HIMAX_EVENT_PEEK_ALERT:
						data_out.present = HUMAN_PRESENCE_DETECTED;
						private_data->human_deteted = HUMAN_PRESENCE_DETECTED;
						event = HIMAX_EVENT_PEEK_ALERT;
						break;
					case HIMAX_EVENT_PEEK_DISMISS:
						data_out.present = HUMAN_PRESENCE_DETECTED;
						private_data->human_deteted = HUMAN_PRESENCE_DETECTED;
						event = HIMAX_EVENT_PEEK_DISMISS;
						break;
					case HIMAX_EVENT_WATCH_SCREEN:
						data_out.present = HUMAN_PRESENCE_DETECTED;
						private_data->human_deteted = HUMAN_PRESENCE_DETECTED;
						event = HIMAX_EVENT_WATCH_SCREEN;
						break;
					case HIMAX_EVENT_NOT_WATCH_SCREEN:
						data_out.present = HUMAN_PRESENCE_DETECTED;
						private_data->human_deteted = HUMAN_PRESENCE_DETECTED;
						event = HIMAX_EVENT_NOT_WATCH_SCREEN;
						break;

				}
				//return udriver_post_data(ctx, &data, 0);
				return udriver_post_data(ctx, &data_out, 0);
				//udriver_post_data(ctx,&data_out,SC_POST_DATA_FLAG_CANCEL_TIMESTAMP_OVERRIDE);
				UDRIVER_PRINT("**Himax-Event : %d Himax-Value : %d **\n", event, value);

				break;
		}
	}
	return ret;
}

/*Leo add*/
sc_status himax_biometric_human_detection_himax_ioctl(udriver_context* ctx, sc_ioctl_id ioctl_id, uint16_t input_buffer_length, uint16_t *output_buffer_length, uint8_t *input_buf, uint8_t *output_buf)
{

	UDRIVER_PRINT("himax_biometric_human_detection_himax_ioctl\n");
	sc_status ret = SC_STATUS_SUCCESS;
	//uint32_t i;
	uint16_t size;
	size = 3;
	*output_buffer_length = size;
	himax_biometric_human_detection_himax_private_data *private_data =
				(himax_biometric_human_detection_himax_private_data *)udriver_get_private_data(ctx);
	output_buf[0]=private_data->human_deteted;
	output_buf[1]=private_data->not_alone;
	output_buf[2]=private_data->not_facing;

	if (ioctl_id == SC_IOCTL_HOST_EXCHANGE_DATA)
	{
		UDRIVER_PRINT("IO control : %c\n",input_buf[0]);
	}

	return ret;
}
