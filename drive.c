#include <stdint.h>
#include <string.h>
#include "drive.h"
#include "misc.h"
#include "imu.h"
#include "bldc.h"
#include "own_std.h"
#include "sin_lut.h"
#include "math.h"
#include "../robotsoft/api_board_to_soft.h"
#include "../robotsoft/api_soft_to_board.h"
#include "audio.h"
#include "tof_ctrl.h" // for N_SENSORS

#define LEDS_ON

#define G_DC_MOTDET_TH 250 // 100 generates false noise detections about every 2-3 seconds
#define G_AC_MOTDET_TH (150*256) // threshold after DC offset correction

#define GYRO_X_BLANKING_TH (100*256)
#define GYRO_Y_BLANKING_TH (100*256)
#define GYRO_Z_BLANKING_TH (100*256)

#define G_DC_FILT 10 // Quadratic effect, 15 maximum.

static int moving = 1000;
static inline void robot_moves()
{
	moving = 1000;
}

static inline void robot_doesnt_move()
{
	if(moving > 0)
		moving--;
}

static inline int is_robot_moving()
{
	return moving;
}

hires_pos_t cur_pos;
hires_pos_t target_pos;

void new_target(hires_pos_t pos)
{
	target_pos = pos;
}

int obstacle_front_near, obstacle_back_near, obstacle_right_near, obstacle_left_near; // Written to in tof_process.c
int obstacle_front_far, obstacle_back_far, obstacle_right_far, obstacle_left_far; // Written to in tof_process.c

int motors_enabled = 0;

int32_t cur_id;
uint32_t micronavi_status;

int new_direction;
int backmode;

static int run;

int is_driving()
{
	return run;
}

static double max_ang_speed = 13.0; // steps per cycle
static double min_ang_speed = 3.0;
static double max_lin_speed = 45.0;
static double min_lin_speed = 5.0;

void set_top_speed_max(int old_style_value)
{
	max_ang_speed = (double)old_style_value / 5.0;
	max_lin_speed = (double)old_style_value / 2.0;

	if(max_ang_speed < min_ang_speed) max_ang_speed = min_ang_speed;
	if(max_lin_speed < min_lin_speed) max_lin_speed = min_lin_speed;
	if(max_ang_speed > 13.0) max_ang_speed = 13.0;
	if(max_lin_speed > 25.0) max_lin_speed = 25.0;
}

static int mode_xy;
static uint32_t ang_to_target;

volatile int lock_processing;

static int stop_indicators;

static int accurot;


void cmd_go_to(s2b_move_abs_t* m)
{
	lock_processing = 1;
	target_pos.x = (int64_t)m->x<<16;
	target_pos.y = (int64_t)m->y<<16;
	backmode = m->backmode;
	new_direction = 1;
	cur_id = m->id;
	micronavi_status = 0;

	accurot = 0;
	mode_xy = 1;
//	do_start = 1;

	lock_processing = 0;

	if(stop_indicators == 0)
		beep(75, 800, -600, 30);

}

#if 0
// Started implementing function taking straight distance and calculating x,y once
void straight_rel(int32_t mm)
{ 
	uint32_t ang = cur_pos.ang;

	if(mm < 0)
	{
		ang += (uint32_t)(ANG_180_DEG);
		mm *= -1;
	}

	int64_t x = (((int64_t)lut_cos_from_u32(ang) * (int64_t)mm)>>SIN_LUT_RESULT_SHIFT)<<16;
	int64_t y = (((int64_t)lut_sin_from_u32(ang) * (int64_t)mm)>>SIN_LUT_RESULT_SHIFT)<<16;

}
#endif

static int64_t store_lin_err;

int32_t get_remaining_lin()
{
	return store_lin_err>>16;
}

void straight_rel(int32_t mm)
{
	ang_to_target = cur_pos.ang;
	store_lin_err = (int64_t)mm<<16;
	mode_xy = 0;

	if(stop_indicators == 0)
		beep(75, 800, -600, 30);

	accurot = 1;

//	do_start = 1;
}

void rotate_rel(int32_t ang32)
{
	ang_to_target = cur_pos.ang + (uint32_t)ang32;
	store_lin_err = 0;
	mode_xy = 0;

	if(stop_indicators == 0)
		beep(75, 800, -600, 30);

	accurot = 1;

//	do_start = 1;
}

void rotate_and_straight_rel(int32_t ang32, int32_t mm, int accurate_rotation_first)
{
	ang_to_target = cur_pos.ang + (uint32_t)ang32;
	store_lin_err = (int64_t)mm<<16;
	mode_xy = 0;

	if(stop_indicators == 0)
		beep(75, 800, -600, 30);

	accurot = accurate_rotation_first;

//	do_start = 1;
}

void cmd_motors(int enabled_ms)
{
	motors_enabled = enabled_ms*4;
}

static s2b_corr_pos_t stored_corrpos;
static int corrpos_in_queue;

void cmd_corr_pos(s2b_corr_pos_t* cmd)
{
	memcpy(&stored_corrpos, cmd, sizeof(s2b_corr_pos_t));
	corrpos_in_queue = 1;
}

void execute_corr_pos()
{
	if(!corrpos_in_queue)
		return;

	int32_t da = stored_corrpos.da;
	int64_t dx = stored_corrpos.dx;
	int64_t dy = stored_corrpos.dy;

	if(dx > 2000LL || dx < -2000LL || dy > 2000LL || dy < -2000LL)
		error(167);

	// Divide by 2 by only shifting 15 places instead of 16.
	dx<<=15;
	dy<<=15;
	da/=2;

	cur_pos.x += dx;
	cur_pos.y += dy;
	cur_pos.ang += (uint32_t)da;
	target_pos.x += dx;
	target_pos.y += dy;

	corrpos_in_queue = 0;
}




static uint32_t ang_to_target;

static void stop()
{
	target_pos = cur_pos;
	new_direction = 0;
	run = 0;
	store_lin_err = 0;
//	do_start = 0;
	ang_to_target = cur_pos.ang;
	backmode = 2; // auto decision
}

void cmd_stop_movement()
{
	lock_processing = 1;
	stop();
	micronavi_status = 0;
	lock_processing = 0;
}

static int ignore_front, ignore_left, ignore_back, ignore_right;

void obstacle_avoidance_ignore(int dir, int ms)
{
	ms *= 4;
	if(dir==0)
		ignore_front = ms;
	else if(dir==1)
		ignore_left = ms;
	else if(dir==2)
		ignore_back = ms;
	else if(dir==3)
		ignore_right = ms;
}

static int err_cnt;
static void log_err()  __attribute__((section(".text_itcm")));
static void log_err()
{
	err_cnt+=1000;
	if(err_cnt > 3000)
		error(130);
}


extern int32_t latency_targets[N_SENSORS];
#define MS(x) ((x)*10)

void sensors_fwd(double speed)
{
	// Latencies: the criticals; the secondaries (sides), the rest (nonrelated)
	// Speed   0 -> 500ms, 1000ms, 2000ms
	// Speed  50 -> 400ms, 800ms,  2200ms
	// Speed 100 -> 300ms, 600ms,  2400ms
	// Speed 150 and over -> 200ms, 400ms,  2600ms
//	int target = 500.0 - speed*2.0;
//	if(target < 200) target = 200;
	int target = 600.0 - speed*2.0;
	if(target < 300) target = 300;

	int secondaries = target * 2;
	int nonrelated = 3000 - 2*target;

	latency_targets[9] = MS(target);
	latency_targets[0] = MS(target);
	latency_targets[1] = MS(target);

	latency_targets[2] = MS(secondaries);
	latency_targets[8] = MS(secondaries);

	latency_targets[3] = MS(nonrelated);
	latency_targets[4] = MS(nonrelated);
	latency_targets[5] = MS(nonrelated);
	latency_targets[6] = MS(nonrelated);
	latency_targets[7] = MS(nonrelated);
}

void sensors_bwd(double speed)
{
	// Latencies: the criticals; the secondaries (sides), the rest (nonrelated)
	// Speed   0 -> 500ms, 1000ms, 2000ms
	// Speed  50 -> 400ms, 800ms,  2200ms
	// Speed 100 -> 300ms, 600ms,  2400ms
	// Speed 150 and over -> 200ms, 400ms,  2600ms
//	int target = 500.0 - speed*2.0;
//	if(target < 200) target = 200;
	int target = 600.0 - speed*2.0;
	if(target < 300) target = 300;

	int secondaries = target * 2;
	int nonrelated = 3000 - 2*target;

	latency_targets[4] = MS(target);
	latency_targets[5] = MS(target);
	latency_targets[6] = MS(target);

	latency_targets[3] = MS(secondaries);
	latency_targets[7] = MS(secondaries);

	latency_targets[0] = MS(nonrelated);
	latency_targets[1] = MS(nonrelated);
	latency_targets[2] = MS(nonrelated);
	latency_targets[8] = MS(nonrelated);
	latency_targets[9] = MS(nonrelated);
}


void sensors_posang(double speed)
{
	// Latencies: the criticals; the secondaries (sides), the rest (nonrelated)
	// Speed   0 -> 500ms, 1000ms, 2000ms
	// Speed  10 -> 400ms, 800ms,  2200ms
	// Speed  20 -> 300ms, 600ms,  2400ms
	// Speed  30 and over -> 200ms, 400ms,  2600ms
//	int target = 500.0 - speed*10.0;
//	if(target < 200) target = 200;
	int target = 600.0 - speed*10.0;
	if(target < 300) target = 300;

	int secondaries = target * 2;
	int nonrelated = 3000 - 2*target;

	latency_targets[7] = MS(target);
	latency_targets[8] = MS(target);

	latency_targets[9] = MS(secondaries);
	latency_targets[4] = MS(secondaries);
	latency_targets[5] = MS(secondaries);

	latency_targets[0] = MS(nonrelated);
	latency_targets[1] = MS(nonrelated);
	latency_targets[2] = MS(nonrelated);
	latency_targets[3] = MS(nonrelated);
	latency_targets[6] = MS(nonrelated);
}

void sensors_negang(double speed)
{
	// Latencies: the criticals; the secondaries (sides), the rest (nonrelated)
	// Speed   0 -> 500ms, 1000ms, 2000ms
	// Speed  10 -> 400ms, 800ms,  2200ms
	// Speed  20 -> 300ms, 600ms,  2400ms
	// Speed  30 and over -> 200ms, 400ms,  2600ms
//	int target = 500.0 - speed*10.0;
//	if(target < 200) target = 200;
	int target = 600.0 - speed*10.0;
	if(target < 300) target = 300;

	int secondaries = target * 2;
	int nonrelated = 3000 - 2*target;

	latency_targets[2] = MS(target);
	latency_targets[3] = MS(target);

	latency_targets[1] = MS(secondaries);
	latency_targets[5] = MS(secondaries);
	latency_targets[6] = MS(secondaries);

	latency_targets[0] = MS(nonrelated);
	latency_targets[4] = MS(nonrelated);
	latency_targets[7] = MS(nonrelated);
	latency_targets[8] = MS(nonrelated);
	latency_targets[9] = MS(nonrelated);
}

void sensors_idle()
{
	for(int i=0; i<N_SENSORS; i++)
		latency_targets[i] = MS(3000);
}


void drive_handler() __attribute__((section(".text_itcm")));
void drive_handler()
{
	static int gyro_dc_x[6];
	static int gyro_dc_y[6];
	static int gyro_dc_z[6];

	int32_t g_pitch=0, g_roll=0, g_yaw=0;
	int32_t a_x=0, a_y=0, a_z=0; // in robot coordinate frame


	static int ac_th_det_on = 0;

	static const uint8_t x_is_pitch_y_is_roll[6] = {1,1,0,  0,0,0}; // Otherwise, x is roll, y is pitch
	static const int8_t pitch_mult[6] = {-1, 1, 1,   -1, 1,-1};
	static const int8_t roll_mult[6] =  {-1, 1,-1,    1,-1, 1};

	static const int8_t x_mult[6] =  {-1, 1,-1,    1,-1, 1};
	static const int8_t y_mult[6] =  { 1,-1,-1,    1,-1, 1};


	/*

	Roll:
	"Left wing" rising: positive g_roll, positive a_y

	Pitch:
	Nose rising: positive g_pitch, positive a_x

	Yaw:
	Counter-clockwise = positive

	a_z is -g when oriented normally

	*/


	for(int imu=0; imu<6; imu++)
	{
		if(imu_a[imu]->n < 4 || imu_a[imu]->n > 7 || imu_g[imu]->n < 4 || imu_g[imu]->n > 7)
		{
			log_err();
			// Just use the old values!
		}

		int32_t cur_gx=0, cur_gy=0, cur_gz=0;
		//int32_t cur_ax=0, cur_ay=0, cur_az=0;

		for(int n=0; n<imu_g[imu]->n; n++)
		{
			cur_gx += imu_g[imu]->xyz[n].x;
			cur_gy += imu_g[imu]->xyz[n].y;
			cur_gz += imu_g[imu]->xyz[n].z;
		}

		cur_gx /= imu_g[imu]->n;
		cur_gy /= imu_g[imu]->n;
		cur_gz /= imu_g[imu]->n;

		// With x256 resolution:
		int cur_gx_dccor = (cur_gx<<8) - (gyro_dc_x[imu]>>7);
		int cur_gy_dccor = (cur_gy<<8) - (gyro_dc_y[imu]>>7);
		int cur_gz_dccor = (cur_gz<<8) - (gyro_dc_z[imu]>>7);

		if(cur_gz < -G_DC_MOTDET_TH || cur_gz > G_DC_MOTDET_TH || cur_gx < -G_DC_MOTDET_TH || cur_gx > G_DC_MOTDET_TH || cur_gy < -G_DC_MOTDET_TH || cur_gy > G_DC_MOTDET_TH ||
		   ((ac_th_det_on>20000) && (cur_gz_dccor < -G_AC_MOTDET_TH || cur_gz_dccor > G_AC_MOTDET_TH || cur_gx_dccor < -G_AC_MOTDET_TH || cur_gx_dccor > G_AC_MOTDET_TH || cur_gy_dccor < -G_AC_MOTDET_TH || cur_gy_dccor > G_AC_MOTDET_TH)))
		{
			robot_moves();
		}
		else
		{
			robot_doesnt_move();
		}

		if(is_robot_moving())
		{
//			led_status(9, WHITE, LED_MODE_FADE);
//			led_status(0, WHITE, LED_MODE_FADE);
//			led_status(1, WHITE, LED_MODE_FADE);
		}
		else
		{
//			led_status(9, BLACK, LED_MODE_KEEP);
//			led_status(0, BLACK, LED_MODE_KEEP);
//			led_status(1, BLACK, LED_MODE_KEEP);
			if(ac_th_det_on < 100000) ac_th_det_on++;
			gyro_dc_x[imu] = ((cur_gx<<15) + ((1<<G_DC_FILT)-1)*gyro_dc_x[0])>>G_DC_FILT;
			gyro_dc_y[imu] = ((cur_gy<<15) + ((1<<G_DC_FILT)-1)*gyro_dc_y[0])>>G_DC_FILT;
			gyro_dc_z[imu] = ((cur_gz<<15) + ((1<<G_DC_FILT)-1)*gyro_dc_z[0])>>G_DC_FILT;
		}

		if(cur_gx_dccor < -GYRO_X_BLANKING_TH || cur_gx_dccor > GYRO_X_BLANKING_TH)
		{
			if(x_is_pitch_y_is_roll[imu])
			{
				g_pitch += (int)pitch_mult[imu] * cur_gx_dccor;
			}
			else
			{
				g_roll += (int)roll_mult[imu] * cur_gx_dccor;
			}
		}

		if(cur_gy_dccor < -GYRO_Y_BLANKING_TH || cur_gy_dccor > GYRO_Y_BLANKING_TH)
		{
			if(x_is_pitch_y_is_roll[imu])
			{
				g_roll += (int)roll_mult[imu] * cur_gy_dccor;
			}
			else
			{
				g_pitch += (int)pitch_mult[imu] * cur_gy_dccor;
			}
		}

		// Yaw is easy, all IMUs are mounted on the PCB top layer.
		if(cur_gz_dccor < -GYRO_Z_BLANKING_TH || cur_gz_dccor > GYRO_Z_BLANKING_TH)
		{
			g_yaw += cur_gz_dccor;
		}


		int32_t cur_ax=0, cur_ay=0, cur_az=0;

		for(int n=0; n<imu_g[imu]->n; n++)
		{
			cur_ax += imu_a[imu]->xyz[n].x;
			cur_ay += imu_a[imu]->xyz[n].y;
			cur_az += imu_a[imu]->xyz[n].z;
		}

		cur_ax /= imu_g[imu]->n;
		cur_ay /= imu_g[imu]->n;
		cur_az /= imu_g[imu]->n;

		if(x_is_pitch_y_is_roll[imu])
		{
			a_y += (int)y_mult[imu] * cur_ax;
		}
		else
		{
			a_x += (int)x_mult[imu] * cur_ax;
		}

		if(x_is_pitch_y_is_roll[imu])
		{
			a_x += (int)x_mult[imu] * cur_ay;
		}
		else
		{
			a_y += (int)y_mult[imu] * cur_ay;
		}

		// Z is easy, all IMUs point the same way.
		a_z += -1*cur_az;
	}

	g_pitch /= 6;
	g_roll /= 6;
	g_yaw /= 6;

	a_x /= 6;
	a_y /= 6;
	a_z /= 6;


	/*
		setting: 97495:
		8 rounds in positive direction:
		358.1 - too little
		1.9 deg -> 0.2375 per 360 too little
		New setting: 97559


		8 rounds in negative direction:
		353.5 - too much
		6.5 deg -> 0.8125 per 360 too much
		New setting: 97276

	*/

	if(ac_th_det_on>10000)
	{
/*
		if(g_yaw > 0)
			cur_pos.ang += (97559LL*(int64_t)g_yaw)>>16;
		else
			cur_pos.ang += (97276LL*(int64_t)g_yaw)>>16;
*/

		// Pos dir: drifts a little in pos dir with  97995    drifts in pos dir with 98095    drifts in pos dir with 98010
		// Neg dir: drifts in neg dir   97295    in  pos dir 96295
		if(g_yaw > 0)
			cur_pos.ang += (97695LL*(int64_t)g_yaw)>>16;
		else
			cur_pos.ang += (96895LL*(int64_t)g_yaw)>>16;

		cur_pos.pitch += (97495LL*(int64_t)g_pitch)>>16;
		cur_pos.roll += (97495LL*(int64_t)g_roll)>>16;

		int32_t pitch_by_acc = (uint32_t) (4294967296.0 * (M_PI + -1*atan2(a_x, a_z)) / (2.0*M_PI));
		int32_t roll_by_acc  = (uint32_t) (4294967296.0 * (M_PI + -1*atan2(a_y, a_z)) / (2.0*M_PI));

		int32_t pitch_err = pitch_by_acc - cur_pos.pitch;
		int32_t roll_err = roll_by_acc - cur_pos.roll;

		cur_pos.pitch += pitch_err/2048;
		cur_pos.roll += roll_err/2048;
	}
	else
	{
		if(is_robot_moving())
		{
			for(int i=0; i<10; i++)
				led_status(i, RED, LED_MODE_FADE);

			ac_th_det_on = 0;
		}
	}

#define WHEEL_DIAM_MM 790LL

	uint32_t mpos[2];
	mpos[0] = bldc_pos[0];
	mpos[1] = bldc_pos[1];

//	int32_t mpos_err[2];
//	mpos_err[0] = bldc_pos[0] - bldc_pos_set[0];
//	mpos_err[1] = bldc_pos[1] - bldc_pos_set[1];

	
	static uint32_t prevpos[2];
	static int initialized;
	int32_t deltapos[2];
	if(!initialized)
	{
		prevpos[0] = mpos[0];
		prevpos[1] = mpos[1];
		initialized=1;
		target_pos = cur_pos;
	}
	deltapos[0] = mpos[0] - prevpos[0];
	deltapos[1] = mpos[1] - prevpos[1];
	prevpos[0] = mpos[0];
	prevpos[1] = mpos[1];

	if(deltapos[0] < -1000 || deltapos[0] > 1000 || deltapos[1] < -1000 || deltapos[1] > 1000) error(132);

	deltapos[0] *= (WHEEL_DIAM_MM<<16)/(90*256);
	deltapos[1] *= -1*(WHEEL_DIAM_MM<<16)/(90*256);

	int32_t fwd = (deltapos[0] + deltapos[1])>>1;

	cur_pos.x += ((int64_t)lut_cos_from_u32(cur_pos.ang) * (int64_t)fwd)>>SIN_LUT_RESULT_SHIFT;
	cur_pos.y += ((int64_t)lut_sin_from_u32(cur_pos.ang) * (int64_t)fwd)>>SIN_LUT_RESULT_SHIFT;


	if(hw_pose)
	{
		hw_pose->ang = cur_pos.ang;
		hw_pose->pitch = cur_pos.pitch;
		hw_pose->roll = cur_pos.roll;
		hw_pose->x = cur_pos.x>>16;
		hw_pose->y = cur_pos.y>>16;
	}







	if(lock_processing)
		return;

	



	int64_t lin_err;

	int reverse = 0;


	if(mode_xy)
	{
		int64_t dx = cur_pos.x - target_pos.x;
		int64_t dy = cur_pos.y - target_pos.y;
		lin_err = sqrt(sq((double)dx) + sq((double)dy));

		if(lin_err > 400LL*65536LL || new_direction)
			ang_to_target  = (uint32_t)(((double)ANG_180_DEG*2.0*(M_PI+atan2((double)dy, (double)dx)))/(2.0*M_PI));
	}
	else
	{
		store_lin_err -= fwd;

		if(store_lin_err < 0)
		{
			lin_err = -1*store_lin_err;
			reverse = 1;
		}
		else
		{
			lin_err = store_lin_err;
			reverse = 0;
		}

	}


	new_direction = 0;

	int32_t ang_err = cur_pos.ang - ang_to_target;

	int auto_decision = 0;
	if((lin_err < 300*65536 && lin_err > -300*65536) || backmode==2)
	{
		auto_decision = 1;
	}

	if((auto_decision && (ang_err > 90*ANG_1_DEG || ang_err < -90*ANG_1_DEG)) || (backmode==1 && !auto_decision))
	{
		reverse = 1;
		ang_err = (uint32_t)ang_err + (uint32_t)(ANG_180_DEG);
	}

	static int prev_run;


	if(reverse)
	{
		lin_err *= -1;
	}

#ifdef LEDS_ON
	if(stop_indicators == 0)
	{
		if(reverse)
		{
			if(run)
			{
				led_status(4, WHITE, LED_MODE_FADE);
				led_status(5, WHITE, LED_MODE_FADE);
				led_status(6, WHITE, LED_MODE_FADE);
			}
			led_status(9, BLACK, LED_MODE_FADE);
			led_status(0, BLACK, LED_MODE_FADE);
			led_status(1, BLACK, LED_MODE_FADE);
		}
		else
		{
			led_status(4, BLACK, LED_MODE_FADE);
			led_status(5, BLACK, LED_MODE_FADE);
			led_status(6, BLACK, LED_MODE_FADE);
			if(run)
			{
				led_status(9, WHITE, LED_MODE_FADE);
				led_status(0, WHITE, LED_MODE_FADE);
				led_status(1, WHITE, LED_MODE_FADE);
			}
		}
	}
#endif

	// Over 100 meters is an error.
	if(lin_err < -6553600000LL || lin_err > 6553600000LL) error(133);


	static int correcting_angle = 1;
	static int correcting_linear = 0;


	if(accurot)
	{
		if(ang_err < -5*ANG_1_DEG || ang_err > 5*ANG_1_DEG)
			correcting_linear = 0;
		else if(ang_err > -3*ANG_1_DEG && ang_err < 3*ANG_1_DEG)
			correcting_linear = 1;
	}
	else
	{
		if(ang_err < -25*ANG_1_DEG || ang_err > 25*ANG_1_DEG)
			correcting_linear = 0;
		else if(ang_err > -8*ANG_1_DEG && ang_err < 8*ANG_1_DEG)
			correcting_linear = 1;
	}



	static double ang_speed;
	double max_ang_speed_by_ang_err;

	if(correcting_linear && abso(lin_err) > 100*65536)
		max_ang_speed_by_ang_err =       0.000000040*(double)abso(ang_err);
	else
		max_ang_speed_by_ang_err = 1.0 + 0.000000040*(double)abso(ang_err);


	double max_ang_speed_by_lin_err = 999.9; // do not use such limitation

	static double lin_speed;
	double max_lin_speed_by_lin_err = 5.0 + 0.05*(double)abso((lin_err>>16)); // 400mm error -> speed unit 20
	double max_lin_speed_by_ang_err;

	if(accurot)
		max_lin_speed_by_ang_err = ((double)ANG_1_DEG*50.0) / ((double)abso(ang_err)); // 10 deg error -> max lin speed 5 units.
	else
		max_lin_speed_by_ang_err = ((double)ANG_1_DEG*200.0) / ((double)abso(ang_err)); // 10 deg error -> max lin speed 20 units.

	// Calculate the target wheel positions to correct the measured angular error
	// In theory, this movement produces the "correct" end result automatically
	// However, only the _remaining_ error (by gyro!) is used on each cycle, so the target
	// wheel position gets better and better.
	// 180 degree positive turn is -18000 units on both wheels
	// 18 degree is -1800
	// 1.8 degree is -180 units
	// 1 degree is -100 units
	
	int32_t delta_mpos_ang[2];
	int32_t delta_mpos_lin[2];

	delta_mpos_ang[0] = (ang_err/ANG_0_01_DEG);
	delta_mpos_ang[1] = (ang_err/ANG_0_01_DEG);

	delta_mpos_lin[0] = lin_err / ((WHEEL_DIAM_MM<<16)/(90LL*256LL));
	delta_mpos_lin[1] = -1*lin_err / ((WHEEL_DIAM_MM<<16)/(90LL*256LL));


	if(correcting_angle)
	{	
		if(ang_speed < max_ang_speed) ang_speed *= 1.004;
	}

	if(ang_speed > max_ang_speed || ang_speed > max_ang_speed_by_ang_err || ang_speed > max_ang_speed_by_lin_err)
		ang_speed *= 0.985;

	if(correcting_linear)
	{	
		if(lin_speed < max_lin_speed) lin_speed *= 1.010;
	}

	if(lin_speed > max_lin_speed || lin_speed > max_lin_speed_by_lin_err || lin_speed > max_lin_speed_by_ang_err)
		lin_speed *= 0.985;


	if(correcting_linear)
	{
		if(reverse)
		{
			sensors_bwd(lin_speed);
		}
		else
		{
			sensors_fwd(lin_speed);
		}
	}
	else // Rotating only
	{
		if(ang_err < -6*ANG_1_DEG)
		{
			sensors_posang(ang_speed);
		}
		else if(ang_err > +6*ANG_1_DEG)
		{
			sensors_negang(ang_speed);
		}
		else
		{
			sensors_idle();
		}
	}



	static const int max_mpos_err = 4*256;
	static int max_pos_err_cnt;
	// We know the target wheel positions, but limit the rate of change
	int ang_speed_i = (int)ang_speed;
	int lin_speed_i = (int)lin_speed;
	int dbg5, dbg6;
	for(int m=0; m<2; m++)
	{
		int d_ang = delta_mpos_ang[m];
		int d_lin = delta_mpos_lin[m];

		if(d_ang > ang_speed_i) d_ang = ang_speed_i;
		else if(d_ang < -1*ang_speed_i) d_ang = -1*ang_speed_i;

		if(d_lin > lin_speed_i) d_lin = lin_speed_i;
		else if(d_lin < -1*lin_speed_i) d_lin = -1*lin_speed_i;

		int increment = d_ang + d_lin;
		if(m==0) dbg5 = increment; else dbg6 = increment;

		int32_t old_pos_err = mpos[m] - bldc_pos_set[m];
		uint32_t new_pos = bldc_pos_set[m] + (uint32_t)increment;
		int32_t new_pos_err = mpos[m] - new_pos;

		// If the error would grow too far, saturate the position. But, allow error-decreasing direction of change.
		if((new_pos_err > max_mpos_err && abso(new_pos_err) > abso(old_pos_err)) ||
		   (new_pos_err < -1*max_mpos_err && abso(new_pos_err) > abso(old_pos_err)))

		{
			max_pos_err_cnt++;
		}
		else
		{
			bldc_pos_set[m] = new_pos;
		}
	}



#define OBST_THRESHOLD 5

	// All stopping conditions first:
	if(!motors_enabled)
	{
		stop();
	}
	else if(run)
	{
		if(mode_xy)
		{
			if(lin_err < 30LL*65536LL && lin_err > -30LL*65536LL)
			{
				if(stop_indicators == 0)
					beep(100, 2000, 0, 50);
				run = 0;

			}
		}
		else
		{
			if(ang_err > -3*ANG_0_5_DEG && ang_err < 3*ANG_0_5_DEG && lin_err < 20LL*65536LL && lin_err > -20LL*65536LL)
			{
				if(stop_indicators == 0)
					beep(100, 2000, 0, 50);
				run = 0;

			}
		}

	}

	if(ignore_front==0 && run && correcting_linear && ((lin_err > 50*65536 && obstacle_front_near > OBST_THRESHOLD) || (lin_err > 180*65536 && obstacle_front_far > OBST_THRESHOLD)))
	{
		{
			beep(500, 100, 0, 100);
			stop_indicators = 125;
			#ifdef LEDS_ON
			led_status(9, RED, LED_MODE_FADE);
			led_status(0, RED, LED_MODE_FADE);
			led_status(1, RED, LED_MODE_FADE);
			#endif
		}
		micronavi_status |= 1UL<<0;
		stop();

	}
	else if(ignore_back==0 && run && correcting_linear && ((lin_err < -50*65536 && obstacle_back_near > OBST_THRESHOLD) || (lin_err < -180*65536 && obstacle_back_far > OBST_THRESHOLD)))
	{
		{
			beep(500, 100, 0, 100);
			stop_indicators = 125;
			#ifdef LEDS_ON
			led_status(4, RED, LED_MODE_FADE);
			led_status(5, RED, LED_MODE_FADE);
			led_status(6, RED, LED_MODE_FADE);
			#endif
		}

		micronavi_status |= 1UL<<0;
		stop();
	}
	else if(ignore_left==0 && run && correcting_angle && ((ang_err > 10*ANG_1_DEG && obstacle_left_near > OBST_THRESHOLD) || (ang_err > 18*ANG_1_DEG && obstacle_left_far > OBST_THRESHOLD)))
	{
		{
			beep(500, 100, 0, 100);
			stop_indicators = 125;
			#ifdef LEDS_ON
			led_status(2, RED, LED_MODE_FADE);
			led_status(3, RED, LED_MODE_FADE);
			#endif
		}

		micronavi_status |= 1UL<<2;
		stop();
	}
	else if(ignore_right==0 && run && correcting_angle && ((ang_err < -10*ANG_1_DEG && obstacle_right_near > OBST_THRESHOLD) || (ang_err < -18*ANG_1_DEG && obstacle_right_far > OBST_THRESHOLD)))
	{
		{
			beep(500, 100, 0, 100);
			stop_indicators = 125;
			#ifdef LEDS_ON
			led_status(7, RED, LED_MODE_FADE);
			led_status(8, RED, LED_MODE_FADE);
			#endif
		}
		micronavi_status |= 1UL<<2;

		stop();
	}

	if(ignore_left > 0) ignore_left--;
	if(ignore_right > 0) ignore_right--;
	if(ignore_front > 0) ignore_front--;
	if(ignore_back > 0) ignore_back--;

	int do_start = 0;

	// Then, starting conditions:
	if(accurot)
	{
		if(ang_err < -2*ANG_1_DEG || ang_err > 2*ANG_1_DEG || lin_err > 40LL*65536LL || lin_err < -40LL*65536LL)
		{
			if(motors_enabled)
				do_start = 1;
		}
	}
	else
	{
		if(ang_err < -7*ANG_1_DEG || ang_err > 7*ANG_1_DEG || lin_err > 60LL*65536LL || lin_err < -60LL*65536LL)
		{
			if(motors_enabled)
				do_start = 1;
		}
	}

	static int dbg1;
	if(do_start && !run)
	{
		if(stop_indicators == 0)
			beep(150, 800, -300, 50);

		motor_torque_lim(0, 90);
		motor_torque_lim(1, 90);
		motor_run(0);
		motor_run(1);
		ang_speed = min_ang_speed;
		lin_speed = min_lin_speed;
		run = 1;
	}

	if(!run)
	{
		motor_let_stop(0);
		motor_let_stop(1);
	}

	prev_run = run;

	static int nonrun_cnt;
	if(run)
		nonrun_cnt = 0;

	if(!run || motors_enabled < 1)
	{
		if(nonrun_cnt < 200)
		{
			nonrun_cnt++;
		}
		else
		{
			motor_release(0);
			motor_release(1);
//			bldc_pos_set[0] = bldc_pos[0];
//			bldc_pos_set[1] = bldc_pos[1];
		}
	}

	if(motors_enabled > 0) motors_enabled--;

	if(err_cnt > 0) err_cnt--;

	if(stop_indicators > 0) stop_indicators--;

	if(drive_diag)
	{
		drive_diag->ang_err = ang_err;
		drive_diag->lin_err = lin_err>>16;
		drive_diag->cur_x = cur_pos.x>>16;
		drive_diag->cur_y = cur_pos.y>>16;
		drive_diag->target_x = target_pos.x>>16;
		drive_diag->target_y = target_pos.y>>16;
		drive_diag->id = cur_id;
		drive_diag->remaining = abso((lin_err>>16));
		drive_diag->micronavi_stop_flags = micronavi_status;
		drive_diag->run = run;
		drive_diag->dbg1 = motors_enabled;
		drive_diag->dbg2 = mpos[0];
		drive_diag->dbg3 = bldc_pos_set[1];
		drive_diag->dbg4 = mpos[1];
		drive_diag->ang_speed_i = ang_speed_i;
		drive_diag->lin_speed_i = lin_speed_i;
		drive_diag->dbg5 = dbg5;
		drive_diag->dbg6 = dbg6;
	}



}

