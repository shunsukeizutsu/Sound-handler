#ifndef RECORDMUSIC_HPP
#define RECORDMUSIC_HPP

#define RECORDMUSIC_SNAME			"rec_music"
#define DATA_SIZE 1024


typedef struct {
	char data[DATA_SIZE];
} rec_music;

typedef struct {
/*	char ver[ 256 ];
	double gravity;
	double initAng[ 2 ];			// 設置角度 (roll, pitch)
	double angvel_offset[ 3 ];	// 角速度のオフセット
	double K_Coef;				// 相補フィルターの係数
*/
} rec_music_property;
#endif // RECORDMUSIC_HPP