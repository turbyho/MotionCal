#ifndef IMUread_h_
#define IMUread_h_

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#if defined(LINUX)
  #include <termios.h>
  #include <unistd.h>
  #include <GL/gl.h>
  #include <GL/glu.h>
#elif defined(WINDOWS)
  #include <windows.h>
  #include <GL/gl.h>
  #include <GL/glu.h>
#elif defined(MACOSX)
  #include <termios.h>
  #include <unistd.h>
  #include <OpenGL/gl.h>
  #include <OpenGL/glu.h>
#endif


#if defined(LINUX)
  #define PORT "/dev/ttyACM0"
#elif defined(WINDOWS)
  #define PORT "COM3"
#elif defined(MACOSX)
  #define PORT "/dev/cu.usbmodemfd132"
#endif

#define TIMEOUT_MSEC 40

#define MAGBUFFSIZEX 14
#define MAGBUFFSIZEY 28
#define MAGBUFFSIZE (MAGBUFFSIZEX * MAGBUFFSIZEY)

#ifdef __cplusplus
extern "C"{
#endif

typedef struct {
	float x;
	float y;
	float z;
	int valid;
	float distsqsum;
} magdata_t;
extern magdata_t caldata[MAGBUFFSIZE];
extern magdata_t hard_iron;
extern float soft_iron[9];
typedef struct {
	float w;
	float x;
	float y;
	float z;
} quat_t;
extern quat_t current_orientation;

extern int open_port(const char *name);
extern int read_serial_data(void);
extern void close_port(void);
void raw_data(const int *data);
void visualize_init(void);
void display_callback(void);
void resize_callback(int width, int height);



// magnetometer measurement buffer
struct MagneticBuffer
{
    int16_t iBpFast[3][MAGBUFFSIZEX][MAGBUFFSIZEY];   // uncalibrated magnetometer readings
    int32_t index[MAGBUFFSIZEX][MAGBUFFSIZEY];        // array of time indices
    int16_t tanarray[MAGBUFFSIZEX - 1];               // array of tangents of (100 * angle)
    int16_t iMagBufferCount;                          // number of magnetometer readings
};

// magnetic calibration structure
struct MagCalibration
{
    float fV[3];                  // current hard iron offset x, y, z, (uT)
    float finvW[3][3];            // current inverse soft iron matrix
    float fB;                     // current geomagnetic field magnitude (uT)
    float fFourBsq;               // current 4*B*B (uT^2)
    float fFitErrorpc;            // current fit error %
    float ftrV[3];                // trial value of hard iron offset z, y, z (uT)
    float ftrinvW[3][3];          // trial inverse soft iron matrix size
    float ftrB;                   // trial value of geomagnetic field magnitude in uT
    float ftrFitErrorpc;          // trial value of fit error %
    float fA[3][3];               // ellipsoid matrix A
    float finvA[3][3];            // inverse of ellipsoid matrix A
    float fmatA[10][10];          // scratch 10x10 matrix used by calibration algorithms
    float fmatB[10][10];          // scratch 10x10 matrix used by calibration algorithms
    float fvecA[10];              // scratch 10x1 vector used by calibration algorithms
    float fvecB[4];               // scratch 4x1 vector used by calibration algorithms
    int8_t iCalInProgress;        // flag denoting that a calibration is in progress
    int8_t iMagCalHasRun;         // flag denoting that at least one calibration has been launched
    int8_t iValidMagCal;          // integer value 0, 4, 7, 10 denoting both valid calibration and solver used
};

void fUpdateCalibration4INV(struct MagCalibration *pthisMagCal, struct MagneticBuffer *pthisMagBuffer);
void fUpdateCalibration7EIG(struct MagCalibration *pthisMagCal, struct MagneticBuffer *pthisMagBuffer);
void fUpdateCalibration10EIG(struct MagCalibration *pthisMagCal, struct MagneticBuffer *pthisMagBuffer);


void f3x3matrixAeqI(float A[][3]);
void fmatrixAeqI(float *A[], int16_t rc);
void f3x3matrixAeqScalar(float A[][3], float Scalar);
void f3x3matrixAeqInvSymB(float A[][3], float B[][3]);
void f3x3matrixAeqAxScalar(float A[][3], float Scalar);
void f3x3matrixAeqMinusA(float A[][3]);
float f3x3matrixDetA(float A[][3]);
void eigencompute(float A[][10], float eigval[], float eigvec[][10], int8_t n);
void fmatrixAeqInvA(float *A[], int8_t iColInd[], int8_t iRowInd[], int8_t iPivot[], int8_t isize);
void fmatrixAeqRenormRotA(float A[][3]);



#ifdef __cplusplus
} // extern "C"
#endif

#endif
