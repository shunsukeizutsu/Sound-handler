#ifndef SOUNDHANDLER_HPP
#define SOUNDHANDLER_HPP
#define SOUNDHANDLER_SNAME "sound"
#define SAMPLE_RATE 44100
//#define SAMPLE_RATE 48000
#define CHANNELS 1//モノラル
#define SAMPLE_SIZE sizeof(float)
#define BUF_SIZE (1024 * SAMPLE_SIZE)

typedef struct
{
	float buf[BUF_SIZE / SAMPLE_SIZE];

} rec_sound;

typedef struct
{
	/*	char ver[ 256 ];
		double gravity;
		double initAng[ 2 ];			// 設置角度 (roll, pitch)
		double angvel_offset[ 3 ];	// 角速度のオフセット
		double K_Coef;				// 相補フィルターの係数
	*/
} rec_sound_property;
#endif // SOUNDHANDLER_HPP