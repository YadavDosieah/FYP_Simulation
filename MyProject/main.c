#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "ch.h"
#include "chprintf.h"
#include "hal.h"
#include "shell.h"

#include "aseba_vm/aseba_node.h"
#include "aseba_vm/skel_user.h"
#include "aseba_vm/aseba_can_interface.h"
#include "aseba_vm/aseba_bridge.h"
#include "audio/audio_thread.h"
#include "audio/play_melody.h"
#include "audio/play_sound_file.h"
#include "audio/microphone.h"
#include "camera/camera.h"
#include "epuck1x/Asercom.h"
#include "epuck1x/Asercom2.h"
#include "epuck1x/a_d/advance_ad_scan/e_acc.h"
#include "epuck1x/motor_led/advance_one_timer/e_led.h"
#include "epuck1x/utility/utility.h"
#include "sensors/battery_level.h"
#include "sensors/ground.h"
#include "sensors/imu.h"
#include "sensors/mpu9250.h"
#include "sensors/proximity.h"
#include "sensors/VL53L0X/VL53L0X.h"
#include "button.h"
#include "cmd.h"
#include "config_flash_storage.h"
#include "exti.h"
#include "fat.h"
#include "i2c_bus.h"
#include "ir_remote.h"
#include "leds.h"
#include <main.h>
#include "memory_protection.h"
#include "motors.h"
#include "sdio.h"
#include "selector.h"
#include "serial_comm.h"
#include "spi_comm.h"
#include "usbcfg.h"
#include "communication.h"
#include "uc_usage.h"
#include "camera/dcmi_camera.h"

#define SHELL_WA_SIZE   THD_WORKING_AREA_SIZE(2048)

#define Max(a,b) (a>b?a:b)
#define Min(a,b) (a<b?a:b)

void RandomLEDs(int);
void SetSpeed(int v_l, int v_r);
void PixelDebug(int i, int r, int g, int b);
void LED_Toggle_Ring(void);
void LED_Toggle_Body(void);

const int LED_Max = 10;
const int LED_Min = 0;
int vl0 = 0, vr0 = 0, vl1 = 0, vr1 = 0, vl2 = 0, vr2 = 0;
#define cx 318
#define cy 200
#define cwidth 8
#define cheight 40
const subsampling_t Sx = SUBSAMPLING_X1, Sy = SUBSAMPLING_X2;
#define Sys 2

messagebus_t bus;
MUTEX_DECL(bus_lock);
CONDVAR_DECL(bus_condvar);

parameter_namespace_t parameter_root, aseba_ns;

static THD_WORKING_AREA(selector_thd_wa, 2048);

static bool load_config(void)
{
    extern uint32_t _config_start;

    return config_load(&parameter_root, &_config_start);
}

/*
   IR Receiver:
   Red: 0x37
   Green: 0x36
   Yellow: 0x32
   Blue: 0x34
   Power: 0xC
   OK: 0x35
   Cycle: 0x22
*/

static THD_FUNCTION(selector_thd, arg)
{
    (void) arg;
    chRegSetThreadName(__FUNCTION__);
    systime_t time;
    messagebus_topic_t *imu_topic = messagebus_find_topic_blocking(&bus, "/imu");
    imu_msg_t imu_values;
    uint8_t stop_loop = 0;
    uint8_t magneto_state = 0;
    uint8_t demo15_state = 0;
    double heading = 0.0;
    float mag_values[3];
    uint8_t temp_rx = 0;
	calibrate_acc();
	calibrate_gyro();
	calibrate_ir();
    // My definitions
    int Mode = 0;
    int iP; // iPixel
    // const int BuffLen = 25; // Hard coded!
    int w_c_r = 0; // Red pixel counter
    int w_c_g = 0;
    double R, G, B, Cmax, Cmin, Delta;
    double H = 0, S = 0, V = 0;
    uint8_t *img_buff_ptr;
    uint16_t r = 0, g = 0, b = 0;
    uint8_t hw_test_state = 0;
    char str[100];
    int str_length;

    while(stop_loop == 0)
    {
    	time = chVTGetSystemTime();
		switch(Mode)
		{
			case 0: // Init hardware.
				set_rgb_led(0, 0, 0, 0);
				set_rgb_led(1, 0, 0, 0);
				set_rgb_led(2, 0, 0, 0);
				set_rgb_led(3, 0, 0, 0);
				// chThdSleepUntilWindowed(time, time + MS2ST(50)); // Refresh @ 20 Hz.
				// if (SDU1.config->usbp->state != USB_ACTIVE) { // Skip printing if port not opened.
				//     continue;
				// }

				// chprintf((BaseSequentialStream *)&SDU1, ">> Init phase...\n >> Selector: %d\r\n", get_selector());
				// cam_advanced_config(FORMAT_COLOR, 310, 236, 20, 8, SUBSAMPLING_X4, SUBSAMPLING_X4);
				cam_advanced_config(FORMAT_COLOR, cx, cy, cwidth, cheight, Sx, Sy);
				cam_set_ae(0);
				cam_set_exposure(512, 0); // Fix the exposure to have a stable framerate.
				const int BuffLen = cwidth * cheight/Sys * 2;

				// cam_advanced_config(FORMAT_COLOR, 310, 230, 20, 20, SUBSAMPLING_X1, SUBSAMPLING_X1);
				dcmi_disable_double_buffering();
				dcmi_set_capture_mode(CAPTURE_ONE_SHOT);
				dcmi_prepare();
				//
				// spi_comm_suspend();
				// dcmi_capture_start();
				// wait_image_ready();
				// spi_comm_resume();
				// img_buff_ptr = cam_get_last_image_ptr();
				// int BuffLen = (int) strlen(img_buff_ptr);
				// chprintf((BaseSequentialStream *)&SDU1, " >> Buffer length: %d\r\n", BuffLen);
				// size_t len = strlen(img_buff_ptr);
				// int BuffLen =  static_cast<int>(len);
				// chprintf((BaseSequentialStream *)&SDU1, ">> Debug Mode:\n >> Buffer length: %lu\r\n", (unsigned long) strlen(img_buff_ptr));
				// playMelody(WE_ARE_THE_CHAMPIONS, ML_SIMPLE_PLAY, NULL);
//				LED_Toggle_Ring();
//				if (ir_remote_get_data() == 0x37) // Red: Spatial Coverage
//				{
//					vl0 = 720; vr0 = 413; vl1 = -998; vr1 = -912;
//					LED_Toggle_Body();
//				}
				// if (ir_remote_get_data() == 0x36) // Green: Decision Making
				// {
				//     vl0 = 760; vr0 = 999; vl1 = 998; vr1 = 685, vl2 = 980; vr2 = 996;
				//     LED_Toggle_Body();
				// }
//				if (ir_remote_get_data() == 0x34) // Blue: Aggregation
//				{
//					vl0 = -600; vr0 = -900; vl1 = 900; vr1 = -900;
//					LED_Toggle_Body();
//				}
//				if (ir_remote_get_data() == 0x32) // Yellow: Debug
//				{
//					vl0 = 0; vr0 = 0; vl1 = 0; vr1 = 0;
//					LED_Toggle_Body();
//				}
//				if (ir_remote_get_data() == 0x35)
//					Mode = 1;
					// set_body_led(0);
				Mode = 1;
				break;
			case 1: // Test.
				// chThdSleepUntilWindowed(time, time + MS2ST(50)); // Refresh @ 20 Hz.

				//set_body_led(0);
//				LED_Toggle_Ring();
				spi_comm_suspend();
				dcmi_capture_start();
				wait_image_ready();
				spi_comm_resume();
				img_buff_ptr = cam_get_last_image_ptr();
				// Image Processing!
				// chprintf((BaseSequentialStream *)&SDU1, "IR RECEIVER: 0x%x\r\n", ir_remote_get_data());
				int w_c_total = cwidth * cheight/Sys;
				int w_c_confidency = w_c_total/10*3;
			    w_c_r = 0;
				w_c_g = 0;
//				chprintf((BaseSequentialStream *)&SDU1, "New Image\n");
//				chprintf((BaseSequentialStream *)&SDU1, "Confidence = %.3d\r\n", w_c_confidency);
				for (iP = 0; iP<BuffLen; iP+=2)
				{
					r = (int)img_buff_ptr[iP]&0xF8;
					g = (int)(img_buff_ptr[iP]&0x07)<<5 | (img_buff_ptr[iP + 1]&0xE0)>>3;
					b = (int)(img_buff_ptr[iP + 1]&0x1F)<<3;

					R = r/255.0; G = g/255.0; B = b/255.0;
					Cmax = Max(Max(R, G), Max(G, B));
					Cmin = Min(Min(R, G), Min(G, B));
					Delta = Cmax - Cmin;
					// Compute H
					if (Cmax == Cmin) // if cmax and cmax are equal then h = 0
						H = 0.0;
					else if (Cmax == R)   // if cmax equal r then compute h
						H = fmod(60 * ((G - B) / Delta) + 360, 360);
					else if (Cmax == G)   // if cmax equal g then compute h
						H = fmod(60 * ((B - R) / Delta) + 120, 360);
					else if (Cmax == B)   // if cmax equal b then compute h
						H = fmod(60 * ((R - G) / Delta) + 240, 360);
					// Compute S
					if (Cmax == 0)
						S = 0.0;
					else
						S = (Delta / Cmax) * 100;
					// Compute v
					V = Cmax * 100;
//				    chprintf((BaseSequentialStream *)&SDU1, "(%3d, %3d, %3d) -> (%.3f, %.3f, %.3f)\r\n", r, g, b, H,S,V);
					if (H > 330 || H < 10)
					{
						w_c_r++;
					}
					else if(H > 170 && H < 220)
					{
						w_c_g++;
					}
					// if (V >= 50)
				   // r_c = RGB2HSV(r, g, b);
					// if ((r < 180) && (g < 180) && (b < 140))
						// w_c++;
					// PixelDebug(iP,H,S,V);
				}
//				chprintf((BaseSequentialStream *)&SDU1, "w_r,W_g = %.3d, %.3d\r\n", w_c_r,w_c_g);
				str_length = sprintf(str, "w_r,W_g = %.3d, %.3d\r\n", w_c_r,w_c_g);
				e_send_uart1_char(str, str_length);
				if (w_c_r > w_c_confidency)
				{
					set_rgb_led(0, 1, 0, 0);
					set_rgb_led(1, 1, 0, 0);
					set_rgb_led(2, 1, 0, 0);
					set_rgb_led(3, 1, 0, 0);
				}
				else if(w_c_g > w_c_confidency)
				{
					set_rgb_led(0, 0, 1, 0);
					set_rgb_led(1, 0, 1, 0);
					set_rgb_led(2, 0, 1, 0);
					set_rgb_led(3, 0, 1, 0);
				}
				else
				{
					set_rgb_led(0, 0, 0, 0);
					set_rgb_led(1, 0, 0, 0);
					set_rgb_led(2, 0, 0, 0);
					set_rgb_led(3, 0, 0, 0);
				}
//
//					SetSpeed(vl0, vr0);
//					// chprintf((BaseSequentialStream *)&SDU1, "Nothing! %d/%d\r\n", w_c, w_c_total);
//				}
//				else
//				{
//					SetSpeed(vl1, vr1);
//					// chprintf((BaseSequentialStream *)&SDU1, "Robot! %d/%d\r\n", w_c, w_c_total);
//				}
//				w_c = 0;
//
//				if (ir_remote_get_data() == 0xC || ir_remote_get_data() == 0x22)
//				{
//					SetSpeed(0,0);
//					Mode = 0;
//				}
				chThdSleepMilliseconds(100);
				break;
		}
    }
}

int main(void)
{

    halInit();
    chSysInit();
    mpu_init();

    /** Inits the Inter Process Communication bus. */
    messagebus_init(&bus, &bus_lock, &bus_condvar);
    parameter_namespace_declare(&parameter_root, NULL, NULL);

    // Init the peripherals.
	clear_leds();
	set_body_led(0);
	set_front_led(0);

    usb_start();
	dcmi_start();
	cam_start();
	motors_init();
	proximity_start();
	battery_level_start();
	dac_start();
	exti_start();
	imu_start();
	ir_remote_start();
	spi_comm_start();
	VL53L0X_start();

    serial_start();
	mic_start(NULL);
	sdio_start();
	playMelodyStart();
	playSoundFileStart();
	ground_start();

	// Initialise Aseba system, declaring parameters
    parameter_namespace_declare(&aseba_ns, &parameter_root, "aseba");
    aseba_declare_parameters(&aseba_ns);

    /* Load parameter tree from flash. */
    load_config();

    /* Start AsebaCAN. Must be after config was loaded because the CAN id
     * cannot be changed at runtime. */
    aseba_vm_init();
    aseba_can_start(&vmState);

    chThdCreateStatic(selector_thd_wa, sizeof(selector_thd_wa), NORMALPRIO, selector_thd, NULL);

    /* Infinite loop. */
    while (1) {
        chThdSleepMilliseconds(1000);
    }
}

#define STACK_CHK_GUARD 0xe2dee396
uintptr_t __stack_chk_guard = STACK_CHK_GUARD;

void __stack_chk_fail(void)
{
    chSysHalt("Stack smashing detected");
}

void RandomLEDs(int light)
{
    int l1 = 0, l2 = 0, l3 = 0;
    if (light != 0)
    {
        l1 = (rand() % (LED_Max - LED_Min + 1)) + LED_Min;
        l2 = (rand() % (LED_Max - LED_Min + 1)) + LED_Min;
        l3 = (rand() % (LED_Max - LED_Min + 1)) + LED_Min;
    }
    set_rgb_led(0, l1, l2, 0);
    set_rgb_led(1, 0, l2, l3);
    set_rgb_led(2, l1, 0 , l3);
    set_rgb_led(3, l1, l2, l3);
}

void SetSpeed(int v_l, int v_r)
{
    left_motor_set_speed(v_l);
    right_motor_set_speed(v_r);
}

void LED_Toggle_Ring(void)
{
    set_body_led(0);
    RandomLEDs(1);
    chThdSleepMilliseconds(500);
    clear_leds();
    chThdSleepMilliseconds(500);
}

void LED_Toggle_Body(void)
{
    clear_leds();
    set_body_led(1);
    chThdSleepMilliseconds(500);
    set_body_led(0);
    chThdSleepMilliseconds(500);
}
/*
int RGB2HSV(int r, int g, int b)
{
    double R, G, B, Cmax, Cmin, Delta;
    double H = 0, S = 0, V = 0;
    int Red = 0;
    R = r/255.0; G = g/255.0; B = b/255.0;
    Cmax = Max(Max(R, G), Max(G, B));
    Cmin = Min(Min(R, G), Min(G, B));
    Delta = Cmax - Cmin;
    // Compute H
    if (Cmax == Cmin) // if cmax and cmax are equal then h = 0
        H = 0.0;
    else if (Cmax == R)   // if cmax equal r then compute h
        H = fmod(60 * ((G - B) / Delta) + 360, 360);
    else if (Cmax == G)   // if cmax equal g then compute h
        H = fmod(60 * ((B - R) / Delta) + 120, 360);
    else if (Cmax == B)   // if cmax equal b then compute h
        H = fmod(60 * ((R - G) / Delta) + 240, 360);
    // Compute S
    if (Cmax == 0)
        S = 0.0;
    else
        S = (Delta / Cmax) * 100;
    // Compute v
    V = Cmax * 100;
    if (H > 300 || H < 10)
        Red = 1;
    // chprintf((BaseSequentialStream *)&SDU1, "(%3d, %3d, %3d) -> (%.3f, %.3f, %.3f)\r\n", r, g, b, H,S,V);
    return Red;
}
*/
void PixelDebug(int i, int r, int g, int b)
{
    chprintf((BaseSequentialStream *)&SDU1, "%d: (%3d, %3d, %3d)\r\n", i, r, g, b);
}

