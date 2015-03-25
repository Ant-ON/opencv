/* This is the contributed code:

 File:             cvcap_v4l.cpp
 Current Location: ../opencv-0.9.6/otherlibs/videoio

 Original Version: 2003-03-12  Magnus Lundin lundin@mlu.mine.nu
 Original Comments:

 ML:This set of files adds support for firevre and usb cameras.
 First it tries to install a firewire camera,
 if that fails it tries a v4l/USB camera
 It has been tested with the motempl sample program

 First Patch:  August 24, 2004 Travis Wood   TravisOCV@tkwood.com
 For Release:  OpenCV-Linux Beta4  opencv-0.9.6
 Tested On:    LMLBT44 with 8 video inputs
 Problems?     Post your questions at answers.opencv.org,
 Report bugs at code.opencv.org,
 Submit your fixes at https://github.com/Itseez/opencv/
 Patched Comments:

 TW: The cv cam utils that came with the initial release of OpenCV for LINUX Beta4
 were not working.  I have rewritten them so they work for me. At the same time, trying
 to keep the original code as ML wrote it as unchanged as possible.  No one likes to debug
 someone elses code, so I resisted changes as much as possible.  I have tried to keep the
 same "ideas" where applicable, that is, where I could figure out what the previous author
 intended. Some areas I just could not help myself and had to "spiffy-it-up" my way.

 These drivers should work with other V4L frame capture cards other then my bttv
 driven frame capture card.

 Re Written driver for standard V4L mode. Tested using LMLBT44 video capture card.
 Standard bttv drivers are on the LMLBT44 with up to 8 Inputs.

 This utility was written with the help of the document:
 http://pages.cpsc.ucalgary.ca/~sayles/VFL_HowTo
 as a general guide for interfacing into the V4l standard.

 Made the index value passed for icvOpenCAM_V4L(index) be the number of the
 video device source in the /dev tree. The -1 uses original /dev/video.

 Index  Device
 0    /dev/video0
 1    /dev/video1
 2    /dev/video2
 3    /dev/video3
 ...
 7    /dev/video7
 with
 -1   /dev/video

 TW: You can select any video source, but this package was limited from the start to only
 ONE camera opened at any ONE time.
 This is an original program limitation.
 If you are interested, I will make my version available to other OpenCV users.  The big
 difference in mine is you may pass the camera number as part of the cv argument, but this
 convention is non standard for current OpenCV calls and the camera number is not currently
 passed into the called routine.

 Second Patch:   August 28, 2004 Sfuncia Fabio fiblan@yahoo.it
 For Release:  OpenCV-Linux Beta4 Opencv-0.9.6

 FS: this patch fix not sequential index of device (unplugged device), and real numCameras.
 for -1 index (icvOpenCAM_V4L) i dont use /dev/video but real device available, because
 if /dev/video is a link to /dev/video0 and i unplugged device on /dev/video0, /dev/video
 is a bad link. I search the first available device with indexList.

 Third Patch:   December 9, 2004 Frederic Devernay Frederic.Devernay@inria.fr
 For Release:  OpenCV-Linux Beta4 Opencv-0.9.6

 [FD] I modified the following:
 - handle YUV420P, YUV420, and YUV411P palettes (for many webcams) without using floating-point
 - cvGrabFrame should not wait for the end of the first frame, and should return quickly
 (see videoio doc)
 - cvRetrieveFrame should in turn wait for the end of frame capture, and should not
 trigger the capture of the next frame (the user choses when to do it using GrabFrame)
 To get the old behavior, re-call cvRetrieveFrame just after cvGrabFrame.
 - having global bufferIndex and FirstCapture variables makes the code non-reentrant
 (e.g. when using several cameras), put these in the CvCapture struct.
 - according to V4L HowTo, incrementing the buffer index must be done before VIDIOCMCAPTURE.
 - the VID_TYPE_SCALES stuff from V4L HowTo is wrong: image size can be changed
 even if the hardware does not support scaling (e.g. webcams can have several
 resolutions available). Just don't try to set the size at 640x480 if the hardware supports
 scaling: open with the default (probably best) image size, and let the user scale it
 using SetProperty.
 - image size can be changed by two subsequent calls to SetProperty (for width and height)
 - bug fix: if the image size changes, realloc the new image only when it is grabbed
 - issue errors only when necessary, fix error message formatting.

 Fourth Patch: Sept 7, 2005 Csaba Kertesz sign@freemail.hu
 For Release:  OpenCV-Linux Beta5 OpenCV-0.9.7

 I modified the following:
 - Additional Video4Linux2 support :)
 - Use mmap functions (v4l2)
 - New methods are internal:
 try_palette_v4l2 -> rewrite try_palette for v4l2
 mainloop_v4l2, read_image_v4l2 -> this methods are moved from official v4l2 capture.c example
 try_init_v4l -> device v4l initialisation
 try_init_v4l2 -> device v4l2 initialisation
 autosetup_capture_mode_v4l -> autodetect capture modes for v4l
 autosetup_capture_mode_v4l2 -> autodetect capture modes for v4l2
 - Modifications are according with Video4Linux old codes
 - Video4Linux handling is automatically if it does not recognize a Video4Linux2 device
 - Tested succesful with Logitech Quickcam Express (V4L), Creative Vista (V4L) and Genius VideoCam Notebook (V4L2)
 - Correct source lines with compiler warning messages
 - Information message from v4l/v4l2 detection

 Fifth Patch: Sept 7, 2005 Csaba Kertesz sign@freemail.hu
 For Release:  OpenCV-Linux Beta5 OpenCV-0.9.7

 I modified the following:
 - SN9C10x chip based webcams support
 - New methods are internal:
 bayer2rgb24, sonix_decompress -> decoder routines for SN9C10x decoding from Takafumi Mizuno <taka-qce@ls-a.jp> with his pleasure :)
 - Tested succesful with Genius VideoCam Notebook (V4L2)

 Sixth Patch: Sept 10, 2005 Csaba Kertesz sign@freemail.hu
 For Release:  OpenCV-Linux Beta5 OpenCV-0.9.7

 I added the following:
 - Add capture control support (hue, saturation, brightness, contrast, gain)
 - Get and change V4L capture controls (hue, saturation, brightness, contrast)
 - New method is internal:
 icvSetControl -> set capture controls
 - Tested succesful with Creative Vista (V4L)

 Seventh Patch: Sept 10, 2005 Csaba Kertesz sign@freemail.hu
 For Release:  OpenCV-Linux Beta5 OpenCV-0.9.7

 I added the following:
 - Detect, get and change V4L2 capture controls (hue, saturation, brightness, contrast, gain)
 - New methods are internal:
 v4l2_scan_controls_enumerate_menu, v4l2_scan_controls -> detect capture control intervals
 - Tested succesful with Genius VideoCam Notebook (V4L2)

 8th patch: Jan 5, 2006, Olivier.Bornet@idiap.ch
 Add support of V4L2_PIX_FMT_YUYV and V4L2_PIX_FMT_MJPEG.
 With this patch, new webcams of Logitech, like QuickCam Fusion works.
 Note: For use these webcams, look at the UVC driver at
 http://linux-uvc.berlios.de/

 9th patch: Mar 4, 2006, Olivier.Bornet@idiap.ch
 - try V4L2 before V4L, because some devices are V4L2 by default,
 but they try to implement the V4L compatibility layer.
 So, I think this is better to support V4L2 before V4L.
 - better separation between V4L2 and V4L initialization. (this was needed to support
 some drivers working, but not fully with V4L2. (so, we do not know when we
 need to switch from V4L2 to V4L.

 10th patch: July 02, 2008, Mikhail Afanasyev fopencv@theamk.com
 Fix reliability problems with high-resolution UVC cameras on linux
 the symptoms were damaged image and 'Corrupt JPEG data: premature end of data segment' on stderr
 - V4L_ABORT_BADJPEG detects JPEG warnings and turns them into errors, so bad images
 could be filtered out
 - USE_TEMP_BUFFER fixes the main problem (improper buffer management) and
 prevents bad images in the first place

 11th patch: April 2, 2013, Forrest Reiling forrest.reiling@gmail.com
 Added v4l2 support for getting capture property CV_CAP_PROP_POS_MSEC.
 Returns the millisecond timestamp of the last frame grabbed or 0 if no frames have been grabbed
 Used to successfully synchonize 2 Logitech C310 USB webcams to within 16 ms of one another


 make & enjoy!

 */

/*M///////////////////////////////////////////////////////////////////////////////////////
//
//  IMPORTANT: READ BEFORE DOWNLOADING, COPYING, INSTALLING OR USING.
//
//  By downloading, copying, installing or using the software you agree to this license.
//  If you do not agree to this license, do not download, install,
//  copy or use the software.
//
//
//                        Intel License Agreement
//                For Open Source Computer Vision Library
//
// Copyright (C) 2000, Intel Corporation, all rights reserved.
// Third party copyrights are property of their respective owners.
//
// Redistribution and use in source and binary forms, with or without modification,
// are permitted provided that the following conditions are met:
//
//   * Redistribution's of source code must retain the above copyright notice,
//     this list of conditions and the following disclaimer.
//
//   * Redistribution's in binary form must reproduce the above copyright notice,
//     this list of conditions and the following disclaimer in the documentation
//     and/or other materials provided with the distribution.
//
//   * The name of Intel Corporation may not be used to endorse or promote products
//     derived from this software without specific prior written permission.
//
// This software is provided by the copyright holders and contributors "as is" and
// any express or implied warranties, including, but not limited to, the implied
// warranties of merchantability and fitness for a particular purpose are disclaimed.
// In no event shall the Intel Corporation or contributors be liable for any direct,
// indirect, incidental, special, exemplary, or consequential damages
// (including, but not limited to, procurement of substitute goods or services;
// loss of use, data, or profits; or business interruption) however caused
// and on any theory of liability, whether in contract, strict liability,
// or tort (including negligence or otherwise) arising in any way out of
// the use of this software, even if advised of the possibility of such damage.
//
//M*/

#include "precomp.hpp"

#if !defined WIN32 && (defined HAVE_CAMV4L2 || defined HAVE_VIDEOIO)

#define CLEAR(x) memset (&(x), 0, sizeof (x))

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/mman.h>

#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/stat.h>
#include <sys/ioctl.h>

#ifdef HAVE_CAMV4L2
#include <asm/types.h>          /* for videodev2.h */
#include <linux/videodev2.h>
#endif

#ifdef HAVE_VIDEOIO
#include <sys/videoio.h>
#endif

#define DEFAULT_V4L_WIDTH  640
#define DEFAULT_V4L_HEIGHT 480

#define CHANNEL_NUMBER 1
#define MAX_CAMERAS 8

// default and maximum number of V4L buffers, not including last, 'special' buffer
#define MAX_V4L_BUFFERS 10
#define DEFAULT_V4L_BUFFERS 4

// if enabled, then bad JPEG warnings become errors and cause NULL returned instead of image
#define V4L_ABORT_BADJPEG

#define MAX_DEVICE_DRIVER_NAME 20

/* V4L2 structure */
struct buffer
{
    void * start;
    size_t length;
};

/* Additional V4L2 pixelformats support for Sonix SN9C10x base webcams */
#ifndef V4L2_PIX_FMT_SBGGR8
#define V4L2_PIX_FMT_SBGGR8  v4l2_fourcc('B','A','8','1') /* 8 BGBG.. GRGR.. */
#endif
#ifndef V4L2_PIX_FMT_SN9C10X
#define V4L2_PIX_FMT_SN9C10X  v4l2_fourcc('S','9','1','0') /* SN9C10x cmpr. */
#endif

#ifndef V4L2_PIX_FMT_SGBRG
#define V4L2_PIX_FMT_SGBRG v4l2_fourcc('G','B','R','G') /* bayer GBRG   GBGB.. RGRG.. */
#endif

/* Pixel format */
enum PALETTE_TYPE
{
    PALETTE_BGR24 = 1,
    PALETTE_YVU420,
    PALETTE_YUV411P,
    PALETTE_JPEG,
    PALETTE_MJPEG,
    PALETTE_YUYV,
    PALETTE_UYVY,
    PALETTE_SBGGR8,
    PALETTE_SN9C10X,
    PALETTE_SGBRG,
    PALETTE_RGB24,

    PALETTE_MAX,
};

const unsigned int PIXEL_FORMAT[PALETTE_MAX] =
{
    0,
    V4L2_PIX_FMT_BGR24,
    V4L2_PIX_FMT_YVU420,
    V4L2_PIX_FMT_YUV411P,
    V4L2_PIX_FMT_JPEG,
    V4L2_PIX_FMT_MJPEG,
    V4L2_PIX_FMT_YUYV,
    V4L2_PIX_FMT_UYVY,
    V4L2_PIX_FMT_SBGGR8,
    V4L2_PIX_FMT_SN9C10X,
    V4L2_PIX_FMT_SGBRG,
    V4L2_PIX_FMT_RGB24,
};

/* Controls */
const unsigned int CONTROL_V4L2[] =
{
    V4L2_CID_BRIGHTNESS,
    V4L2_CID_CONTRAST,
    V4L2_CID_SATURATION,
    V4L2_CID_HUE,
    V4L2_CID_GAIN,
    V4L2_CID_EXPOSURE,
    V4L2_CID_EXPOSURE_ABSOLUTE,
    V4L2_CID_EXPOSURE_AUTO,
    V4L2_CID_ZOOM_ABSOLUTE,
    V4L2_CID_ZOOM_RELATIVE,
    V4L2_CID_FOCUS_ABSOLUTE,
    V4L2_CID_FOCUS_RELATIVE,
    V4L2_CID_FOCUS_AUTO,
    V4L2_CID_SHARPNESS,
    V4L2_CID_GAMMA,
    V4L2_CID_WHITE_BALANCE_TEMPERATURE,
    V4L2_CID_PAN_ABSOLUTE,
    V4L2_CID_PAN_RELATIVE,
    V4L2_CID_TILT_ABSOLUTE,
    V4L2_CID_TILT_RELATIVE,
    V4L2_CID_IRIS_ABSOLUTE,
    V4L2_CID_IRIS_RELATIVE,
};

const int CONTROL_CV[] =
{
    CV_CAP_PROP_BRIGHTNESS,
    CV_CAP_PROP_CONTRAST,
    CV_CAP_PROP_SATURATION,
    CV_CAP_PROP_HUE,
    CV_CAP_PROP_GAIN,
    CV_CAP_PROP_EXPOSURE,
    CV_CAP_PROP_EXPOSURE,
    CV_CAP_PROP_AUTO_EXPOSURE,
    CV_CAP_PROP_ZOOM,
    CV_CAP_PROP_ZOOM,
    CV_CAP_PROP_FOCUS,
    CV_CAP_PROP_FOCUS,
    CV_CAP_PROP_AUTO_FOCUS,
    CV_CAP_PROP_SHARPNESS,
    CV_CAP_PROP_GAMMA,
    CV_CAP_PROP_TEMPERATURE,
    CV_CAP_PROP_PAN,
    CV_CAP_PROP_PAN,
    CV_CAP_PROP_TILT,
    CV_CAP_PROP_TILT,
    CV_CAP_PROP_IRIS,
    CV_CAP_PROP_IRIS,
};

#define CONTROL_COUNT (sizeof(CONTROL_CV)/sizeof(CONTROL_CV[0]))

typedef struct CvCaptureCAM_V4L
{
    int deviceHandle;
    int bufferIndex;
    int FirstCapture;
    IplImage frame;

    enum PALETTE_TYPE palette;

    /* V4L2 variables */
    buffer buffers[MAX_V4L_BUFFERS + 1];
    unsigned int n_buffers;

    struct v4l2_capability cap;
    struct v4l2_input inp;
    struct v4l2_format form;
    struct v4l2_requestbuffers req;
    struct v4l2_control control;
    enum v4l2_buf_type type;
    struct v4l2_streamparm parm;

    struct timeval timestamp;

    /* V4L2 control variables */
    int v4l2_ctrl_min[CONTROL_COUNT];
    int v4l2_ctrl_max[CONTROL_COUNT];
    unsigned int v4l2_ctrl_type[CONTROL_COUNT];
} CvCaptureCAM_V4L;

static void v4l2_close(CvCaptureCAM_V4L* capture);

/*
 * Turn a YUV4:2:0 block into an RGB block
 *
 * Video4Linux seems to use the blue, green, red channel
 * order convention-- rgb[0] is blue, rgb[1] is green, rgb[2] is red.
 *
 * Color space conversion coefficients taken from the excellent
 * http://www.inforamp.net/~poynton/ColorFAQ.html
 * In his terminology, this is a CCIR 601.1 YCbCr -> RGB.
 * Y values are given for all 4 pixels, but the U (Pb)
 * and V (Pr) are assumed constant over the 2x2 block.
 *
 * To avoid floating point arithmetic, the color conversion
 * coefficients are scaled into 16.16 fixed-point integers.
 * They were determined as follows:
 *
 *  double brightness = 1.0;  (0->black; 1->full scale)
 *  double saturation = 1.0;  (0->greyscale; 1->full color)
 *  double fixScale = brightness * 256 * 256;
 *  int rvScale = (int)(1.402 * saturation * fixScale);
 *  int guScale = (int)(-0.344136 * saturation * fixScale);
 *  int gvScale = (int)(-0.714136 * saturation * fixScale);
 *  int buScale = (int)(1.772 * saturation * fixScale);
 *  int yScale = (int)(fixScale);
 */

/* LIMIT: convert a 16.16 fixed-point value to a byte, with clipping. */
#define LIMIT(x) ((x)>0xffffff?0xff: ((x)<=0xffff?0:((x)>>16)))

static inline void move_420_block(int yTL, int yTR, int yBL, int yBR, int u,
        int v, int rowPixels, unsigned char * rgb)
{
    const int rvScale = 91881;
    const int guScale = -22553;
    const int gvScale = -46801;
    const int buScale = 116129;
    const int yScale = 65536;
    int r, g, b;

    g = guScale * u + gvScale * v;
//  if (force_rgb) {
//      r = buScale * u;
//      b = rvScale * v;
//  } else {
    r = rvScale * v;
    b = buScale * u;
//  }

    yTL *= yScale;
    yTR *= yScale;
    yBL *= yScale;
    yBR *= yScale;

    /* Write out top two pixels */
    rgb[0] = LIMIT(b + yTL);
    rgb[1] = LIMIT(g + yTL);
    rgb[2] = LIMIT(r + yTL);

    rgb[3] = LIMIT(b + yTR);
    rgb[4] = LIMIT(g + yTR);
    rgb[5] = LIMIT(r + yTR);

    /* Skip down to next line to write out bottom two pixels */
    rgb += 3 * rowPixels;
    rgb[0] = LIMIT(b + yBL);
    rgb[1] = LIMIT(g + yBL);
    rgb[2] = LIMIT(r + yBL);

    rgb[3] = LIMIT(b + yBR);
    rgb[4] = LIMIT(g + yBR);
    rgb[5] = LIMIT(r + yBR);
}

static inline void move_411_block(int yTL, int yTR, int yBL, int yBR, int u,
        int v, int /*rowPixels*/, unsigned char * rgb)
{
    const int rvScale = 91881;
    const int guScale = -22553;
    const int gvScale = -46801;
    const int buScale = 116129;
    const int yScale = 65536;
    int r, g, b;

    g = guScale * u + gvScale * v;
//  if (force_rgb) {
//      r = buScale * u;
//      b = rvScale * v;
//  } else {
    r = rvScale * v;
    b = buScale * u;
//  }

    yTL *= yScale;
    yTR *= yScale;
    yBL *= yScale;
    yBR *= yScale;

    /* Write out top two first pixels */
    rgb[0] = LIMIT(b + yTL);
    rgb[1] = LIMIT(g + yTL);
    rgb[2] = LIMIT(r + yTL);

    rgb[3] = LIMIT(b + yTR);
    rgb[4] = LIMIT(g + yTR);
    rgb[5] = LIMIT(r + yTR);

    /* Write out top two last pixels */
    rgb += 6;
    rgb[0] = LIMIT(b + yBL);
    rgb[1] = LIMIT(g + yBL);
    rgb[2] = LIMIT(r + yBL);

    rgb[3] = LIMIT(b + yBR);
    rgb[4] = LIMIT(g + yBR);
    rgb[5] = LIMIT(r + yBR);
}

// Consider a YUV420P image of 8x2 pixels.
//
// A plane of Y values    A B C D E F G H
//                        I J K L M N O P
//
// A plane of U values    1   2   3   4
// A plane of V values    1   2   3   4 ....
//
// The U1/V1 samples correspond to the ABIJ pixels.
//     U2/V2 samples correspond to the CDKL pixels.
//
/* Converts from planar YUV420P to RGB24. */
static void yuv420p_to_rgb24(int width, int height, unsigned char *pIn0,
        unsigned char *pOut0)
{
    const int numpix = width * height;
    const int bytes = 24 >> 3;
    int i, j, y00, y01, y10, y11, u, v;
    unsigned char *pY = pIn0;
    unsigned char *pU = pY + numpix;
    unsigned char *pV = pU + numpix / 4;
    unsigned char *pOut = pOut0;

    for (j = 0; j <= height - 2; j += 2)
    {
        for (i = 0; i <= width - 2; i += 2)
        {
            y00 = *pY;
            y01 = *(pY + 1);
            y10 = *(pY + width);
            y11 = *(pY + width + 1);
            u = (*pU++) - 128;
            v = (*pV++) - 128;

            move_420_block(y00, y01, y10, y11, u, v, width, pOut);

            pY += 2;
            pOut += 2 * bytes;

        }
        pY += width;
        pOut += width * bytes;
    }
}

// Consider a YUV411P image of 8x2 pixels.
//
// A plane of Y values as before.
//
// A plane of U values    1       2
//                        3       4
//
// A plane of V values    1       2
//                        3       4
//
// The U1/V1 samples correspond to the ABCD pixels.
//     U2/V2 samples correspond to the EFGH pixels.
//
/* Converts from planar YUV411P to RGB24. */
/* [FD] untested... */
static void yuv411p_to_rgb24(int width, int height, unsigned char *pIn0,
        unsigned char *pOut0)
{
    const int numpix = width * height;
    const int bytes = 24 >> 3;
    int i, j, y00, y01, y10, y11, u, v;
    unsigned char *pY = pIn0;
    unsigned char *pU = pY + numpix;
    unsigned char *pV = pU + numpix / 4;
    unsigned char *pOut = pOut0;

    for (j = 0; j <= height; j++)
    {
        for (i = 0; i <= width - 4; i += 4)
        {
            y00 = *pY;
            y01 = *(pY + 1);
            y10 = *(pY + 2);
            y11 = *(pY + 3);
            u = (*pU++) - 128;
            v = (*pV++) - 128;

            move_411_block(y00, y01, y10, y11, u, v, width, pOut);

            pY += 4;
            pOut += 4 * bytes;

        }
    }
}

/* convert from 4:2:2 YUYV interlaced to RGB24 */
/* based on ccvt_yuyv_bgr32() from camstream */
#define SAT(c) \
        if (c & (~255)) { if (c < 0) c = 0; else c = 255; }

static void yuyv_to_rgb24(int width, int height, unsigned char *src,
        unsigned char *dst)
{
    unsigned char *s;
    unsigned char *d;
    int l, c;
    int r, g, b, cr, cg, cb, y1, y2;

    l = height;
    s = src;
    d = dst;
    while (l--)
    {
        c = width >> 1;
        while (c--)
        {
            y1 = *s++;
            cb = ((*s - 128) * 454) >> 8;
            cg = (*s++ - 128) * 88;
            y2 = *s++;
            cr = ((*s - 128) * 359) >> 8;
            cg = (cg + (*s++ - 128) * 183) >> 8;

            r = y1 + cr;
            b = y1 + cb;
            g = y1 - cg;
            SAT(r);
            SAT(g);
            SAT(b);

            *d++ = b;
            *d++ = g;
            *d++ = r;

            r = y2 + cr;
            b = y2 + cb;
            g = y2 - cg;
            SAT(r);
            SAT(g);
            SAT(b);

            *d++ = b;
            *d++ = g;
            *d++ = r;
        }
    }
}

static void uyvy_to_rgb24(int width, int height, unsigned char *src,
        unsigned char *dst)
{
    unsigned char *s;
    unsigned char *d;
    int l, c;
    int r, g, b, cr, cg, cb, y1, y2;

    l = height;
    s = src;
    d = dst;
    while (l--)
    {
        c = width >> 1;
        while (c--)
        {
            cb = ((*s - 128) * 454) >> 8;
            cg = (*s++ - 128) * 88;
            y1 = *s++;
            cr = ((*s - 128) * 359) >> 8;
            cg = (cg + (*s++ - 128) * 183) >> 8;
            y2 = *s++;

            r = y1 + cr;
            b = y1 + cb;
            g = y1 - cg;
            SAT(r);
            SAT(g);
            SAT(b);

            *d++ = b;
            *d++ = g;
            *d++ = r;

            r = y2 + cr;
            b = y2 + cb;
            g = y2 - cg;
            SAT(r);
            SAT(g);
            SAT(b);

            *d++ = b;
            *d++ = g;
            *d++ = r;
        }
    }
}

#ifdef HAVE_JPEG

/* convert from mjpeg to rgb24 */
static bool mjpeg_to_rgb24(int width, int height, unsigned char *src,
        int length, unsigned char *dst)
{
    cv::Mat temp = cv::imdecode(cv::Mat(std::vector<uchar>(src, src + length)),
            1);
    if (!temp.data || temp.cols != width || temp.rows != height)
        return false;
    memcpy(dst, temp.data, width * height * 3);
    return true;
}

#endif

/*
 * BAYER2RGB24 ROUTINE TAKEN FROM:
 *
 * Sonix SN9C10x based webcam basic I/F routines
 * Takafumi Mizuno <taka-qce@ls-a.jp>
 *
 */
static void bayer2rgb24(long int WIDTH, long int HEIGHT, unsigned char *src,
        unsigned char *dst)
{
    long int i;
    unsigned char *rawpt, *scanpt;
    long int size;

    rawpt = src;
    scanpt = dst;
    size = WIDTH * HEIGHT;

    for (i = 0; i < size; i++)
    {
        if ((i / WIDTH) % 2 == 0)
        {
            if ((i % 2) == 0)
            {
                /* B */
                if ((i > WIDTH) && ((i % WIDTH) > 0))
                {
                    *scanpt++ = (*(rawpt - WIDTH - 1) + *(rawpt - WIDTH + 1)
                            + *(rawpt + WIDTH - 1) + *(rawpt + WIDTH + 1)) / 4; /* R */
                    *scanpt++ = (*(rawpt - 1) + *(rawpt + 1) + *(rawpt + WIDTH)
                            + *(rawpt - WIDTH)) / 4; /* G */
                    *scanpt++ = *rawpt; /* B */
                } else
                {
                    /* first line or left column */
                    *scanpt++ = *(rawpt + WIDTH + 1); /* R */
                    *scanpt++ = (*(rawpt + 1) + *(rawpt + WIDTH)) / 2; /* G */
                    *scanpt++ = *rawpt; /* B */
                }
            } else
            {
                /* (B)G */
                if ((i > WIDTH) && ((i % WIDTH) < (WIDTH - 1)))
                {
                    *scanpt++ = (*(rawpt + WIDTH) + *(rawpt - WIDTH)) / 2; /* R */
                    *scanpt++ = *rawpt; /* G */
                    *scanpt++ = (*(rawpt - 1) + *(rawpt + 1)) / 2; /* B */
                } else
                {
                    /* first line or right column */
                    *scanpt++ = *(rawpt + WIDTH); /* R */
                    *scanpt++ = *rawpt; /* G */
                    *scanpt++ = *(rawpt - 1); /* B */
                }
            }
        } else
        {
            if ((i % 2) == 0)
            {
                /* G(R) */
                if ((i < (WIDTH * (HEIGHT - 1))) && ((i % WIDTH) > 0))
                {
                    *scanpt++ = (*(rawpt - 1) + *(rawpt + 1)) / 2; /* R */
                    *scanpt++ = *rawpt; /* G */
                    *scanpt++ = (*(rawpt + WIDTH) + *(rawpt - WIDTH)) / 2; /* B */
                } else
                {
                    /* bottom line or left column */
                    *scanpt++ = *(rawpt + 1); /* R */
                    *scanpt++ = *rawpt; /* G */
                    *scanpt++ = *(rawpt - WIDTH); /* B */
                }
            } else
            {
                /* R */
                if (i < (WIDTH * (HEIGHT - 1)) && ((i % WIDTH) < (WIDTH - 1)))
                {
                    *scanpt++ = *rawpt; /* R */
                    *scanpt++ = (*(rawpt - 1) + *(rawpt + 1) + *(rawpt - WIDTH)
                            + *(rawpt + WIDTH)) / 4; /* G */
                    *scanpt++ = (*(rawpt - WIDTH - 1) + *(rawpt - WIDTH + 1)
                            + *(rawpt + WIDTH - 1) + *(rawpt + WIDTH + 1)) / 4; /* B */
                } else
                {
                    /* bottom line or right column */
                    *scanpt++ = *rawpt; /* R */
                    *scanpt++ = (*(rawpt - 1) + *(rawpt - WIDTH)) / 2; /* G */
                    *scanpt++ = *(rawpt - WIDTH - 1); /* B */
                }
            }
        }
        rawpt++;
    }

}

// SGBRG to RGB24
// for some reason, red and blue needs to be swapped
// at least for  046d:092f Logitech, Inc. QuickCam Express Plus to work
//see: http://www.siliconimaging.com/RGB%20Bayer.htm
//and 4.6 at http://tldp.org/HOWTO/html_single/libdc1394-HOWTO/
static void sgbrg2rgb24(long int WIDTH, long int HEIGHT, unsigned char *src,
        unsigned char *dst)
{
    long int i;
    unsigned char *rawpt, *scanpt;
    long int size;

    rawpt = src;
    scanpt = dst;
    size = WIDTH * HEIGHT;

    for (i = 0; i < size; i++)
    {
        if ((i / WIDTH) % 2 == 0) //even row
        {
            if ((i % 2) == 0) //even pixel
            {
                if ((i > WIDTH) && ((i % WIDTH) > 0))
                {
                    *scanpt++ = (*(rawpt - 1) + *(rawpt + 1)) / 2; /* R */
                    *scanpt++ = *(rawpt); /* G */
                    *scanpt++ = (*(rawpt - WIDTH) + *(rawpt + WIDTH)) / 2; /* B */
                } else
                {
                    /* first line or left column */

                    *scanpt++ = *(rawpt + 1); /* R */
                    *scanpt++ = *(rawpt); /* G */
                    *scanpt++ = *(rawpt + WIDTH); /* B */
                }
            } else //odd pixel
            {
                if ((i > WIDTH) && ((i % WIDTH) < (WIDTH - 1)))
                {
                    *scanpt++ = *(rawpt); /* R */
                    *scanpt++ = (*(rawpt - 1) + *(rawpt + 1) + *(rawpt - WIDTH)
                            + *(rawpt + WIDTH)) / 4; /* G */
                    *scanpt++ = (*(rawpt - WIDTH - 1) + *(rawpt - WIDTH + 1)
                            + *(rawpt + WIDTH - 1) + *(rawpt + WIDTH + 1)) / 4; /* B */
                } else
                {
                    /* first line or right column */

                    *scanpt++ = *(rawpt); /* R */
                    *scanpt++ = (*(rawpt - 1) + *(rawpt + WIDTH)) / 2; /* G */
                    *scanpt++ = *(rawpt + WIDTH - 1); /* B */
                }
            }
        } else
        { //odd row
            if ((i % 2) == 0) //even pixel
            {
                if ((i < (WIDTH * (HEIGHT - 1))) && ((i % WIDTH) > 0))
                {
                    *scanpt++ = (*(rawpt - WIDTH - 1) + *(rawpt - WIDTH + 1)
                            + *(rawpt + WIDTH - 1) + *(rawpt + WIDTH + 1)) / 4; /* R */
                    *scanpt++ = (*(rawpt - 1) + *(rawpt + 1) + *(rawpt - WIDTH)
                            + *(rawpt + WIDTH)) / 4; /* G */
                    *scanpt++ = *(rawpt); /* B */
                } else
                {
                    /* bottom line or left column */

                    *scanpt++ = *(rawpt - WIDTH + 1); /* R */
                    *scanpt++ = (*(rawpt + 1) + *(rawpt - WIDTH)) / 2; /* G */
                    *scanpt++ = *(rawpt); /* B */
                }
            } else
            { //odd pixel
                if (i < (WIDTH * (HEIGHT - 1)) && ((i % WIDTH) < (WIDTH - 1)))
                {
                    *scanpt++ = (*(rawpt - WIDTH) + *(rawpt + WIDTH)) / 2; /* R */
                    *scanpt++ = *(rawpt); /* G */
                    *scanpt++ = (*(rawpt - 1) + *(rawpt + 1)) / 2; /* B */
                } else
                {
                    /* bottom line or right column */

                    *scanpt++ = (*(rawpt - WIDTH)); /* R */
                    *scanpt++ = *(rawpt); /* G */
                    *scanpt++ = (*(rawpt - 1)); /* B */
                }
            }
        }
        rawpt++;
    }
}

static void rgb24_to_rgb24(int width, int height, unsigned char *src,
        unsigned char *dst)
{
    const int size = width * height;
    for (int i = 0; i < size; ++i, src += 3, dst += 3)
    {
        *(dst + 0) = *(src + 2);
        *(dst + 1) = *(src + 1);
        *(dst + 2) = *(src + 0);
    }
}

#define CLAMP(x)        ((x)<0?0:((x)>255)?255:(x))

typedef struct
{
    int is_abs;
    int len;
    int val;
} code_table_t;

/* local storage */
static code_table_t table[256];
static int init_done = 0;

/*
 sonix_decompress_init
 =====================
 pre-calculates a locally stored table for efficient huffman-decoding.

 Each entry at index x in the table represents the codeword
 present at the MSB of byte x.

 */
static void sonix_decompress_init(void)
{
    int i;
    int is_abs, val, len;

    for (i = 0; i < 256; i++)
    {
        is_abs = 0;
        val = 0;
        len = 0;
        if ((i & 0x80) == 0)
        {
            /* code 0 */
            val = 0;
            len = 1;
        } else if ((i & 0xE0) == 0x80)
        {
            /* code 100 */
            val = +4;
            len = 3;
        } else if ((i & 0xE0) == 0xA0)
        {
            /* code 101 */
            val = -4;
            len = 3;
        } else if ((i & 0xF0) == 0xD0)
        {
            /* code 1101 */
            val = +11;
            len = 4;
        } else if ((i & 0xF0) == 0xF0)
        {
            /* code 1111 */
            val = -11;
            len = 4;
        } else if ((i & 0xF8) == 0xC8)
        {
            /* code 11001 */
            val = +20;
            len = 5;
        } else if ((i & 0xFC) == 0xC0)
        {
            /* code 110000 */
            val = -20;
            len = 6;
        } else if ((i & 0xFC) == 0xC4)
        {
            /* code 110001xx: unknown */
            val = 0;
            len = 8;
        } else if ((i & 0xF0) == 0xE0)
        {
            /* code 1110xxxx */
            is_abs = 1;
            val = (i & 0x0F) << 4;
            len = 8;
        }
        table[i].is_abs = is_abs;
        table[i].val = val;
        table[i].len = len;
    }

    init_done = 1;
}

/*
 sonix_decompress
 ================
 decompresses an image encoded by a SN9C101 camera controller chip.

 IN    width
 height
 inp         pointer to compressed frame (with header already stripped)
 OUT   outp    pointer to decompressed frame

 Returns 0 if the operation was successful.
 Returns <0 if operation failed.

 */
static int sonix_decompress(int width, int height, unsigned char *inp,
        unsigned char *outp)
{
    int row, col;
    int val;
    int bitpos;
    unsigned char code;
    unsigned char *addr;

    if (!init_done)
    {
        /* do sonix_decompress_init first! */
        return -1;
    }

    bitpos = 0;
    for (row = 0; row < height; row++)
    {

        col = 0;

        /* first two pixels in first two rows are stored as raw 8-bit */
        if (row < 2)
        {
            addr = inp + (bitpos >> 3);
            code = (addr[0] << (bitpos & 7)) | (addr[1] >> (8 - (bitpos & 7)));
            bitpos += 8;
            *outp++ = code;

            addr = inp + (bitpos >> 3);
            code = (addr[0] << (bitpos & 7)) | (addr[1] >> (8 - (bitpos & 7)));
            bitpos += 8;
            *outp++ = code;

            col += 2;
        }

        while (col < width)
        {
            /* get bitcode from bitstream */
            addr = inp + (bitpos >> 3);
            code = (addr[0] << (bitpos & 7)) | (addr[1] >> (8 - (bitpos & 7)));

            /* update bit position */
            bitpos += table[code].len;

            /* calculate pixel value */
            val = table[code].val;
            if (!table[code].is_abs)
            {
                /* value is relative to top and left pixel */
                if (col < 2)
                {
                    /* left column: relative to top pixel */
                    val += outp[-2 * width];
                } else if (row < 2)
                {
                    /* top row: relative to left pixel */
                    val += outp[-2];
                } else
                {
                    /* main area: average of left pixel and top pixel */
                    val += (outp[-2] + outp[-2 * width]) / 2;
                }
            }

            /* store pixel */
            *outp++ = CLAMP(val);
            col++;
        }
    }

    return 0;
}

static int v4l2_req_buf(CvCaptureCAM_V4L* capture)
{
    unsigned int min;

    /* Buggy driver paranoia. */
    min = capture->form.fmt.pix.width * 2;

    if (capture->form.fmt.pix.bytesperline < min)
        capture->form.fmt.pix.bytesperline = min;

    min = capture->form.fmt.pix.bytesperline * capture->form.fmt.pix.height;

    if (capture->form.fmt.pix.sizeimage < min)
        capture->form.fmt.pix.sizeimage = min;

    CLEAR(capture->req);

    unsigned int buffer_number = DEFAULT_V4L_BUFFERS;

    try_again:

    capture->req.count = buffer_number;
    capture->req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    capture->req.memory = V4L2_MEMORY_MMAP;

    if (-1 == ioctl(capture->deviceHandle, VIDIOC_REQBUFS, &capture->req))
    {
        if (EINVAL == errno)
        {
            fprintf(stderr, "Device does not support memory mapping\n");
        } else
        {
            perror("VIDIOC_REQBUFS");
        }
        /* free capture, and returns an error code */
        v4l2_close(capture);
        return -1;
    }

    if (capture->req.count < buffer_number)
    {
        if (buffer_number == 1)
        {
            fprintf(stderr, "Insufficient buffer memory\n");

            /* free capture, and returns an error code */
            v4l2_close(capture);
            return -1;
        } else
        {
            buffer_number--;
            fprintf(stderr,
                    "Insufficient buffer memory -- decreaseing buffers\n");

            goto try_again;
        }
    }

    for (capture->n_buffers = 0; capture->n_buffers < capture->req.count;
            ++capture->n_buffers)
    {
        struct v4l2_buffer buf;

        CLEAR(buf);

        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;
        buf.index = capture->n_buffers;

        if (-1 == ioctl(capture->deviceHandle, VIDIOC_QUERYBUF, &buf))
        {
            perror("VIDIOC_QUERYBUF");

            /* free capture, and returns an error code */
            v4l2_close(capture);
            return -1;
        }

        capture->buffers[capture->n_buffers].length = buf.length;
        capture->buffers[capture->n_buffers].start = mmap(
        NULL /* start anywhere */, buf.length,
        PROT_READ | PROT_WRITE /* required */,
        MAP_SHARED /* recommended */, capture->deviceHandle, buf.m.offset);

        if (MAP_FAILED == capture->buffers[capture->n_buffers].start)
        {
            perror("mmap");

            /* free capture, and returns an error code */
            v4l2_close(capture);
            return -1;
        }

        if (capture->n_buffers == 0)
        {
            capture->buffers[MAX_V4L_BUFFERS].start = malloc(buf.length);
            capture->buffers[MAX_V4L_BUFFERS].length = buf.length;
        }
    }

    /* Set up Image data */
    cvInitImageHeader(&capture->frame,
            cvSize(capture->form.fmt.pix.width, capture->form.fmt.pix.height),
            IPL_DEPTH_8U, 3, IPL_ORIGIN_TL, 4);
    /* Allocate space for RGBA data */
    capture->frame.imageData = (char *) cvAlloc(capture->frame.imageSize);

    return 0;
}

static int v4l2_read_frame(CvCaptureCAM_V4L* capture)
{
    struct v4l2_buffer buf;

    CLEAR(buf);

    buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    buf.memory = V4L2_MEMORY_MMAP;

    if (-1 == ioctl(capture->deviceHandle, VIDIOC_DQBUF, &buf))
    {
        switch (errno)
        {
        case EAGAIN:
            return 0;

        case EIO:
            if (!(buf.flags & (V4L2_BUF_FLAG_QUEUED | V4L2_BUF_FLAG_DONE)))
            {
                if (ioctl(capture->deviceHandle, VIDIOC_QBUF, &buf) == -1)
                {
                    return 0;
                }
            }
            return 0;

        default:
            /* display the error and stop processing */
            perror("VIDIOC_DQBUF");
            return 1;
        }
    }

    assert(buf.index < capture->req.count);

    memcpy(capture->buffers[MAX_V4L_BUFFERS].start,
            capture->buffers[buf.index].start,
            capture->buffers[MAX_V4L_BUFFERS].length);
    capture->bufferIndex = MAX_V4L_BUFFERS;
    //printf("got data in buff %d, len=%d, flags=0x%X, seq=%d, used=%d)\n",
    //	  buf.index, buf.length, buf.flags, buf.sequence, buf.bytesused);

    if (-1 == ioctl(capture->deviceHandle, VIDIOC_QBUF, &buf))
        perror("VIDIOC_QBUF");

    //set timestamp in capture struct to be timestamp of most recent frame
    capture->timestamp = buf.timestamp;

    return 1;
}

static void v4l2_try_read_frame(CvCaptureCAM_V4L* capture)
{
    unsigned int count;

    count = 1;

    while (count-- > 0)
    {
        for (;;)
        {
            fd_set fds;
            struct timeval tv;
            int r;

            FD_ZERO(&fds);
            FD_SET(capture->deviceHandle, &fds);

            /* Timeout. */
            tv.tv_sec = 10;
            tv.tv_usec = 0;

            r = select(capture->deviceHandle + 1, &fds, NULL, NULL, &tv);

            if (-1 == r)
            {
                if (EINTR == errno)
                    continue;

                perror("select");
            }

            if (0 == r)
            {
                fprintf(stderr, "select timeout\n");

                /* end the infinite loop */
                break;
            }

            if (v4l2_read_frame(capture))
                break;
        }
    }
}

static int v4l2_grab_frame(CvCaptureCAM_V4L* capture)
{
    if (!capture->n_buffers)
    {
        /* Request buffers */
        if (v4l2_req_buf(capture))
            return -1;
    }

    if (capture->FirstCapture)
    {
        /* Some general initialization must take place the first time through */

        /* This is just a technicality, but all buffers must be filled up before any
         staggered SYNC is applied.  SO, filler up. (see V4L HowTo) */

        for (capture->bufferIndex = 0;
                capture->bufferIndex < ((int) capture->req.count);
                ++capture->bufferIndex)
        {

            struct v4l2_buffer buf;

            CLEAR(buf);

            buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
            buf.memory = V4L2_MEMORY_MMAP;
            buf.index = (unsigned long) capture->bufferIndex;

            if (-1 == ioctl(capture->deviceHandle, VIDIOC_QBUF, &buf))
            {
                perror("VIDIOC_QBUF");
                return -1;
            }
        }

        /* enable the streaming */
        capture->type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        if (-1 == ioctl(capture->deviceHandle, VIDIOC_STREAMON, &capture->type))
        {
            /* error enabling the stream */
            perror("VIDIOC_STREAMON");
            return -1;
        }

#if defined(V4L_ABORT_BADJPEG)
        // skip first frame. it is often bad -- this is unnotied in traditional apps,
        //  but could be fatal if bad jpeg is enabled
        v4l2_try_read_frame(capture);
#endif

        /* preparation is ok */
        capture->FirstCapture = 0;
    }

    v4l2_try_read_frame(capture);

    return 0;
}

static int v4l2_set_control(CvCaptureCAM_V4L* capture, int prop_id,
        double value)
{
    /* find control which we want to set */
    unsigned int i;
    int v4l2_prop_id = -1;
    for (i = 0; i < CONTROL_COUNT; i++)
    {
        if (CONTROL_CV[i] == prop_id)
        {
            v4l2_prop_id = i;
            if (capture->v4l2_ctrl_type[i])
                break;
        }
    }

    /* check property */
    if (v4l2_prop_id < 0)
    {
        fprintf(stderr,
                "VIDEOIO ERROR: V4L2: setting property #%d is not supported\n",
                prop_id);
        return -1;
    } else if (!capture->v4l2_ctrl_type[v4l2_prop_id])
    {
        fprintf(stderr,
                "VIDEOIO ERROR: V4L2: device is not supported setting property #%d\n",
                prop_id);
        return -1;
    }

    /* initialisations */
    CLEAR(capture->control);
    capture->control.id = CONTROL_V4L2[i];

    /* set the value we want to set to the scaled the value */
    switch (capture->v4l2_ctrl_type[v4l2_prop_id])
    {
    case V4L2_CTRL_TYPE_INTEGER:
    case V4L2_CTRL_TYPE_INTEGER64:
        if (value <= 0.0)
            capture->control.value = capture->v4l2_ctrl_min[v4l2_prop_id];
        else if (value >= 1.0)
            capture->control.value = capture->v4l2_ctrl_max[v4l2_prop_id];
        else
        {
            capture->control.value = (int) (value
                    * (capture->v4l2_ctrl_max[v4l2_prop_id]
                            - capture->v4l2_ctrl_min[v4l2_prop_id])
                    + capture->v4l2_ctrl_min[v4l2_prop_id]);
        }
        break;
    case V4L2_CTRL_TYPE_BOOLEAN:
        capture->control.value = ((int) value) ? 1 : 0;
        break;
    case V4L2_CTRL_TYPE_MENU:
    case V4L2_CTRL_TYPE_INTEGER_MENU:
        capture->control.value = (int) value;
        break;
    case V4L2_CTRL_TYPE_BUTTON:
        if (((int) value) == 0)
            return 0;
        capture->control.value = 1;
        break;
    default:
        fprintf(stderr,
                "VIDEOIO ERROR: V4L2: type #%d of property #%d is not supported\n",
                capture->v4l2_ctrl_type[v4l2_prop_id], prop_id);
        return -1;
    }

    /* The driver may clamp the value or return ERANGE, ignored here */
    if (-1 == ioctl(capture->deviceHandle,
    VIDIOC_S_CTRL, &capture->control) && errno != ERANGE)
    {
        perror("VIDIOC_S_CTRL");
        return -1;
    }

    /* all was OK */
    return 0;
}

static double v4l2_get_control(CvCaptureCAM_V4L* capture, int prop_id)
{
    /* find control which we want to get */
    unsigned int i;
    int v4l2_prop_id = -1;
    for (i = 0; i < CONTROL_COUNT; i++)
    {
        if (CONTROL_CV[i] == prop_id)
        {
            v4l2_prop_id = i;
            if (capture->v4l2_ctrl_type[i])
                break;
        }
    }

    /* check property */
    if (v4l2_prop_id < 0)
    {
        fprintf(stderr,
                "VIDEOIO ERROR: V4L2: getting property #%d is not supported\n",
                prop_id);
        return -1;
    } else if (!capture->v4l2_ctrl_type[v4l2_prop_id])
    {
        fprintf(stderr,
                "VIDEOIO ERROR: V4L2: device is not supported getting property #%d\n",
                prop_id);
        return -1;
    }

    /* initialisations */
    CLEAR(capture->control);
    capture->control.id = CONTROL_V4L2[i];

    /* get values */
    if (-1 == ioctl(capture->deviceHandle, VIDIOC_G_CTRL, &capture->control))
    {
        perror("VIDIOC_G_CTRL");
        return -1;
    }

    /* all was OK, so convert, and return the value */
    switch (capture->v4l2_ctrl_type[v4l2_prop_id])
    {
    case V4L2_CTRL_TYPE_INTEGER:
    case V4L2_CTRL_TYPE_INTEGER64:
        /* convert to 0.0 - 1.0 range */
        return ((float) capture->control.value
                - capture->v4l2_ctrl_min[v4l2_prop_id] + 1)
                / (capture->v4l2_ctrl_max[v4l2_prop_id]
                        - capture->v4l2_ctrl_min[v4l2_prop_id]);
        break;
    case V4L2_CTRL_TYPE_BOOLEAN:
    case V4L2_CTRL_TYPE_MENU:
    case V4L2_CTRL_TYPE_INTEGER_MENU:
        return capture->control.value;
        break;
    default:
        fprintf(stderr,
                "VIDEOIO ERROR: V4L2: type #%d of property #%d is not supported\n",
                capture->v4l2_ctrl_type[v4l2_prop_id], prop_id);
        return -1;
    }
}

static int v4l2_set_fps(CvCaptureCAM_V4L* capture, int fps)
{
    CLEAR(capture->parm);

    capture->parm.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    capture->parm.parm.capture.timeperframe.numerator = 1;
    capture->parm.parm.capture.timeperframe.denominator = fps;
    if (-1 == ioctl(capture->deviceHandle, VIDIOC_S_PARM, &capture->parm))
        return -1;
    return 0;
}

static int v4l2_get_fps(CvCaptureCAM_V4L* capture)
{
    CLEAR(capture->parm);

    capture->parm.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if (-1 == ioctl(capture->deviceHandle, VIDIOC_G_PARM, &capture->parm))
        return -1;
    return capture->parm.parm.capture.timeperframe.denominator;
}

static int v4l2_reset_crop(CvCaptureCAM_V4L* capture)
{
    struct v4l2_crop crop;
    struct v4l2_cropcap cropcap;

    CLEAR(cropcap);
    cropcap.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

    if (ioctl(capture->deviceHandle, VIDIOC_CROPCAP, &cropcap) < 0)
    {
        fprintf(stderr, "VIDEOIO ERROR: V4L2: VIDIOC_CROPCAP\n");
        return -1;
    }

    CLEAR(crop);
    crop.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    crop.c = cropcap.defrect;

    /* set the crop area, but don't exit if the device don't support croping */
    if (ioctl(capture->deviceHandle, VIDIOC_S_CROP, &crop) < 0)
    {
        fprintf(stderr, "VIDEOIO ERROR: V4L2: VIDIOC_S_CROP\n");
        return -1;
    }

    return 0;
}

static int v4l2_set_video_size(CvCaptureCAM_V4L* capture, int w, int h)
{
    /* Reset the crop */
    v4l2_reset_crop(capture);

    CLEAR(capture->form);
    capture->form.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

    /* read the current setting, mainly to retreive the pixelformat information */
    ioctl(capture->deviceHandle, VIDIOC_G_FMT, &capture->form);

    /* set the values we want to change */
    capture->form.fmt.pix.width = w;
    capture->form.fmt.pix.height = h;
    capture->form.fmt.win.chromakey = 0;
    capture->form.fmt.win.field = V4L2_FIELD_ANY;
    capture->form.fmt.win.clips = 0;
    capture->form.fmt.win.clipcount = 0;
    capture->form.fmt.pix.field = V4L2_FIELD_ANY;

    /* ask the device to change the size
     * don't test if the set of the size is ok, because some device
     * don't allow changing the size, and we will get the real size
     * later */
    ioctl(capture->deviceHandle, VIDIOC_S_FMT, &capture->form);

    /* try to set framerate to 30 fps */
    v4l2_set_fps(capture, 30);

    /* we need to re-initialize some things, like buffers, because the size has
     * changed */
    capture->FirstCapture = 1;

    /* Get window info again, to get the real value */
    if (-1 == ioctl(capture->deviceHandle, VIDIOC_G_FMT, &capture->form))
    {
        fprintf(stderr,
                "VIDEOIO ERROR: V4L2: Could not obtain specifics of capture window.\n\n");
        v4l2_close(capture);
        return -1;
    }

    return 0;
}

static int v4l2_get_video_size(CvCaptureCAM_V4L* capture, bool width)
{
    CLEAR(capture->form);
    capture->form.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if (-1 == ioctl(capture->deviceHandle, VIDIOC_G_FMT, &capture->form))
    {
        /* display an error message, and return an error code */
        perror("VIDIOC_G_FMT");
        return -1;
    }

    if (width)
        return capture->form.fmt.pix.width;
    return capture->form.fmt.pix.height;
}

static PALETTE_TYPE v4l2_get_pallet(unsigned long pxl_fmt)
{
    int i;
    for (i = 1; i < PALETTE_MAX; i++)
        if (PIXEL_FORMAT[i] == pxl_fmt)
            return (PALETTE_TYPE) i;

    return (PALETTE_TYPE) 0;
}

static int v4l2_set_pix_fmt(CvCaptureCAM_V4L* capture, unsigned long pxl_fmt)
{
    CLEAR(capture->form);

    capture->form.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    capture->form.fmt.pix.pixelformat = pxl_fmt;
    capture->form.fmt.pix.field = V4L2_FIELD_ANY;
    capture->form.fmt.pix.width = DEFAULT_V4L_WIDTH;
    capture->form.fmt.pix.height = DEFAULT_V4L_HEIGHT;

    if (-1 == ioctl(capture->deviceHandle, VIDIOC_S_FMT, &capture->form))
        return -1;

    if (pxl_fmt != capture->form.fmt.pix.pixelformat)
        return -1;

    capture->palette = v4l2_get_pallet(capture->form.fmt.pix.pixelformat);
    return 0;
}

static int v4l2_set_pix_fmt_auto(CvCaptureCAM_V4L* capture)
{
    int i;

    for (i = 1; i < PALETTE_MAX; i++)
        if (!v4l2_set_pix_fmt(capture, PIXEL_FORMAT[i]))
            return 0;

    fprintf(stderr,
            "VIDEOIO ERROR: V4L2: Pixel format of incoming image is unsupported by OpenCV\n");
    v4l2_close(capture);
    return -1;
}

static void v4l2_scan_controls(CvCaptureCAM_V4L* capture)
{
    __u32 ctrl_id;
    unsigned int i;
    struct v4l2_queryctrl queryctrl;

    for (ctrl_id = V4L2_CID_BASE; ctrl_id < V4L2_CID_LASTP1; ctrl_id++)
    {
        /* set the id we will query now */
        CLEAR(queryctrl);
        queryctrl.id = ctrl_id;

        if (0 == ioctl(capture->deviceHandle, VIDIOC_QUERYCTRL, &queryctrl))
        {
            if (queryctrl.flags & V4L2_CTRL_FLAG_DISABLED)
                continue;

            for (i = 0; i < CONTROL_COUNT; i++)
            {
                if (queryctrl.id == CONTROL_V4L2[i])
                {
                    capture->v4l2_ctrl_type[i] = queryctrl.type;
                    capture->v4l2_ctrl_min[i] = queryctrl.minimum;
                    capture->v4l2_ctrl_max[i] = queryctrl.maximum;
                    break;
                }
            }
        } else
        {
            if (errno == EINVAL)
                continue;
            perror("VIDIOC_QUERYCTRL");
        }
    }

    for (ctrl_id = V4L2_CID_CAMERA_CLASS_BASE;
            ctrl_id <= V4L2_CID_AUTO_FOCUS_RANGE; ctrl_id++)
    {
        /* set the id we will query now */
        CLEAR(queryctrl);
        queryctrl.id = ctrl_id;

        if (0 == ioctl(capture->deviceHandle, VIDIOC_QUERYCTRL, &queryctrl))
        {
            if (queryctrl.flags & V4L2_CTRL_FLAG_DISABLED)
                continue;

            for (i = 0; i < CONTROL_COUNT; i++)
            {
                if (queryctrl.id == CONTROL_V4L2[i])
                {
                    capture->v4l2_ctrl_type[i] = queryctrl.type;
                    capture->v4l2_ctrl_min[i] = queryctrl.minimum;
                    capture->v4l2_ctrl_max[i] = queryctrl.maximum;
                    break;
                }
            }
        } else
        {
            if (errno == EINVAL)
                continue;
            perror("VIDIOC_QUERYCTRL");
        }
    }
}

static int v4l2_init(CvCaptureCAM_V4L* capture, char *deviceName)
{
    // Test device for V4L2 compability
    // Return value:
    // -1 then unable to open device
    //  0 then detected nothing
    //  1 then V4L2 device

    int deviceIndex;

    /* Test V4L2 device */
    CLEAR(capture->cap);
    if (-1 == ioctl(capture->deviceHandle, VIDIOC_QUERYCAP, &capture->cap))
    {
#ifndef NDEBUG
        fprintf(stderr, "(DEBUG) try_init_v4l2 VIDIOC_QUERYCAP \"%s\": %s\n",
                deviceName, strerror(errno));
#endif
        return -1;
    }

    if ((capture->cap.capabilities & V4L2_CAP_VIDEO_CAPTURE) == 0)
    {
        /* Nope. */
        fprintf( stderr,
                "VIDEOIO ERROR: V4L2: device %s is unable to capture video memory.\n",
                deviceName);
        return -1;
    }

    /* Query channels number */
    if (-1 == ioctl(capture->deviceHandle, VIDIOC_G_INPUT, &deviceIndex))
    {
#ifndef NDEBUG
        fprintf(stderr, "(DEBUG) try_init_v4l2 VIDIOC_G_INPUT \"%s\": %s\n",
                deviceName, strerror(errno));
#endif
        return -1;
    }

    /* Query information about current input */
    CLEAR(capture->inp);
    capture->inp.index = deviceIndex;
    if (-1 == ioctl(capture->deviceHandle, VIDIOC_ENUMINPUT, &capture->inp))
    {
#ifndef NDEBUG
        fprintf(stderr, "(DEBUG) try_init_v4l2 VIDIOC_ENUMINPUT \"%s\": %s\n",
                deviceName, strerror(errno));
#endif
        return -1;
    }

    /* The following code sets the CHANNEL_NUMBER of the video input.  Some video sources
     have sub "Channel Numbers".  For a typical V4L TV capture card, this is usually 1.
     I myself am using a simple NTSC video input capture card that uses the value of 1.
     If you are not in North America or have a different video standard, you WILL have to change
     the following settings and recompile/reinstall.  This set of settings is based on
     the most commonly encountered input video source types (like my bttv card) */

    if (capture->inp.index > 0)
    {
        CLEAR(capture->inp);
        capture->inp.index = CHANNEL_NUMBER;
        /* Set only channel number to CHANNEL_NUMBER */
        /* V4L2 have a status field from selected video mode */
        if (-1 == ioctl(capture->deviceHandle, VIDIOC_ENUMINPUT, &capture->inp))
        {
            fprintf(stderr,
                    "VIDEOIO ERROR: V4L2: Aren't able to set channel number\n");
            return -1;
        }
    }

    /* Find Window info */
    CLEAR(capture->form);
    capture->form.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

    if (-1 == ioctl(capture->deviceHandle, VIDIOC_G_FMT, &capture->form))
    {
        fprintf(stderr,
                "VIDEOIO ERROR: V4L2: Could not obtain specifics of capture window.\n\n");
        return -1;
    }

    /* Init V4L2 control variables */
    memset(capture->v4l2_ctrl_type, 0, sizeof(capture->v4l2_ctrl_type));
    memset(capture->v4l2_ctrl_min, 0, sizeof(capture->v4l2_ctrl_min));
    memset(capture->v4l2_ctrl_max, 0, sizeof(capture->v4l2_ctrl_max));

    capture->timestamp.tv_sec = 0;
    capture->timestamp.tv_usec = 0;

    /* Scan V4L2 controls */
    v4l2_scan_controls(capture);

    return 0;
}

static void v4l2_close(CvCaptureCAM_V4L* capture)
{
    /* Deallocate space - Hopefully, no leaks */
    if (capture)
    {
        capture->type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        if (-1
                == ioctl(capture->deviceHandle, VIDIOC_STREAMOFF,
                        &capture->type))
        {
            perror("Unable to stop the stream.");
        }

        for (unsigned int n_buffers = 0; n_buffers < capture->req.count;
                ++n_buffers)
        {
            if (-1
                    == munmap(capture->buffers[n_buffers].start,
                            capture->buffers[n_buffers].length))
            {
                perror("munmap");
            }
        }

        if (capture->buffers[MAX_V4L_BUFFERS].start)
        {
            free(capture->buffers[MAX_V4L_BUFFERS].start);
            capture->buffers[MAX_V4L_BUFFERS].start = 0;
        }

        if (capture->deviceHandle != -1)
            close(capture->deviceHandle);

        if (capture->frame.imageData)
            cvFree(&capture->frame.imageData);
    }
}

static int v4l2_get_index_auto()
{
    int dev;
    int n;
    char deviceName[MAX_DEVICE_DRIVER_NAME];

    n = 0;
    while (n < MAX_CAMERAS)
    {
        /* Print the CameraNumber at the end of the string with a width of one character */
        sprintf(deviceName, "/dev/video%1d", n);
        /* Test using an open to see if this new device name really does exists. */
        dev = open(deviceName, O_RDONLY);

        if (dev != -1)
        {
            close(dev);
            return n;
        }

        n++;
    }

    return 0;
}

static CvCaptureCAM_V4L * v4l2_open(int index)
{
    int dev;
    char deviceName[MAX_DEVICE_DRIVER_NAME];

    if (index < 0)
        index = v4l2_get_index_auto();

    /* Print the CameraNumber at the end of the string with a width of one character */
    sprintf(deviceName, "/dev/video%1d", index);

    /* Open and test V4L2 device */
    dev = open(deviceName, O_RDWR /* required */| O_NONBLOCK, 0);
    if (-1 == dev)
    {
        fprintf( stderr, "VIDEOIO ERROR: V4L2: index %d is not correct!\n",
                index);
        return NULL; /* Did someone ask for not correct video source number? */
    }
    /* Allocate memory for this humongus CvCaptureCAM_V4L structure that contains ALL
     the handles for V4L processing */
    CvCaptureCAM_V4L * capture = (CvCaptureCAM_V4L*) cvAlloc(
            sizeof(CvCaptureCAM_V4L));
    if (!capture)
    {
        fprintf( stderr,
                "VIDEOIO ERROR: V4L2: Could not allocate memory for capture process.\n");
        return NULL;
    }

    /* w/o memset some parts  arent initialized - AKA: Fill it with zeros so it is clean */
    memset(capture, 0, sizeof(CvCaptureCAM_V4L));
    /* Setup open device handler */
    capture->deviceHandle = dev;
    /* Present the routines needed for V4L funtionality.  They are inserted as part of
     the standard set of cv calls promoting transparency.  "Vector Table" insertion. */
    capture->FirstCapture = 1;

    if (v4l2_init(capture, deviceName))
    {
        v4l2_close(capture);
        return NULL;
    }

    if (v4l2_set_pix_fmt_auto(capture)
            || v4l2_set_video_size(capture, DEFAULT_V4L_WIDTH,
            DEFAULT_V4L_HEIGHT))
    {
        v4l2_close(capture);
        return NULL;
    }

    return capture;
}

static int v4l2_set_cv_property(CvCaptureCAM_V4L* capture, int property_id,
        double value)
{
    static int width = 0, height = 0;
    int retval;

    /* initialization */
    retval = 0;

    /* two subsequent calls setting WIDTH and HEIGHT will change
     the video size */
    /* the first one will return an error, though. */
    switch (property_id)
    {
    case CV_CAP_PROP_FRAME_WIDTH:
        width = cvRound(value);
        if (width != 0 && height != 0)
        {
            retval = v4l2_set_video_size(capture, width, height);
            width = height = 0;
        }
        break;
    case CV_CAP_PROP_FRAME_HEIGHT:
        height = cvRound(value);
        if (width != 0 && height != 0)
        {
            retval = v4l2_set_video_size(capture, width, height);
            width = height = 0;
        }
        break;
    case CV_CAP_PROP_FPS:
        retval = v4l2_set_fps(capture, (int) value);
        break;
    case CV_CAP_PROP_FOURCC:
        if (v4l2_set_pix_fmt(capture, (unsigned int) value))
        {
            v4l2_set_pix_fmt_auto(capture);
        }
        capture->FirstCapture = 1;
        break;
    default:
        retval = v4l2_set_control(capture, property_id, value);
        break;
    }

    /* return the the status */
    return retval;
}

static double v4l2_get_cv_property(CvCaptureCAM_V4L* capture, int prop_id)
{
    switch (prop_id)
    {
    case CV_CAP_PROP_FRAME_WIDTH:
        return v4l2_get_video_size(capture, true);
    case CV_CAP_PROP_FRAME_HEIGHT:
        return v4l2_get_video_size(capture, false);
    case CV_CAP_PROP_FPS:
        return v4l2_get_fps(capture);
    case CV_CAP_PROP_FOURCC:
        return PIXEL_FORMAT[capture->palette];
    case CV_CAP_PROP_POS_MSEC:
        if (capture->FirstCapture)
            return 0;
        return 1000 * capture->timestamp.tv_sec
                + ((double) capture->timestamp.tv_usec) / 1000;
        break;
    }

    return v4l2_get_control(capture, prop_id);
}

static IplImage* v4l2_get_cv_frame(CvCaptureCAM_V4L* capture, int)
{
    /* Now get what has already been captured as a IplImage return */

    /* First, reallocate imageData if the frame size changed */

    if (((unsigned long) capture->frame.width != capture->form.fmt.pix.width)
            || ((unsigned long) capture->frame.height
                    != capture->form.fmt.pix.height))
    {
        cvFree(&capture->frame.imageData);
        cvInitImageHeader(&capture->frame,
                cvSize(capture->form.fmt.pix.width,
                        capture->form.fmt.pix.height),
                IPL_DEPTH_8U, 3, IPL_ORIGIN_TL, 4);
        capture->frame.imageData = (char *) cvAlloc(capture->frame.imageSize);
    }

    switch (capture->palette)
    {
    case PALETTE_BGR24:
        memcpy((char *) capture->frame.imageData,
                (char *) capture->buffers[capture->bufferIndex].start,
                capture->frame.imageSize);
        break;

    case PALETTE_YVU420:
        yuv420p_to_rgb24(capture->form.fmt.pix.width,
                capture->form.fmt.pix.height,
                (unsigned char*) (capture->buffers[capture->bufferIndex].start),
                (unsigned char*) capture->frame.imageData);
        break;

    case PALETTE_YUV411P:
        yuv411p_to_rgb24(capture->form.fmt.pix.width,
                capture->form.fmt.pix.height,
                (unsigned char*) (capture->buffers[capture->bufferIndex].start),
                (unsigned char*) capture->frame.imageData);
        break;
#ifdef HAVE_JPEG
    case PALETTE_JPEG:
    case PALETTE_MJPEG:
        if (!mjpeg_to_rgb24(capture->form.fmt.pix.width,
                capture->form.fmt.pix.height,
                (unsigned char*) (capture->buffers[capture->bufferIndex].start),
                capture->buffers[capture->bufferIndex].length,
                (unsigned char*) capture->frame.imageData))
            return 0;
        break;
#endif
    case PALETTE_YUYV:
        yuyv_to_rgb24(capture->form.fmt.pix.width, capture->form.fmt.pix.height,
                (unsigned char*) (capture->buffers[capture->bufferIndex].start),
                (unsigned char*) capture->frame.imageData);
        break;
    case PALETTE_UYVY:
        uyvy_to_rgb24(capture->form.fmt.pix.width, capture->form.fmt.pix.height,
                (unsigned char*) (capture->buffers[capture->bufferIndex].start),
                (unsigned char*) capture->frame.imageData);
        break;
    case PALETTE_SBGGR8:
        bayer2rgb24(capture->form.fmt.pix.width, capture->form.fmt.pix.height,
                (unsigned char*) capture->buffers[capture->bufferIndex].start,
                (unsigned char*) capture->frame.imageData);
        break;

    case PALETTE_SN9C10X:
        sonix_decompress_init();
        sonix_decompress(capture->form.fmt.pix.width,
                capture->form.fmt.pix.height,
                (unsigned char*) capture->buffers[capture->bufferIndex].start,
                (unsigned char*) capture->buffers[(capture->bufferIndex + 1)
                        % capture->req.count].start);

        bayer2rgb24(capture->form.fmt.pix.width, capture->form.fmt.pix.height,
                (unsigned char*) capture->buffers[(capture->bufferIndex + 1)
                        % capture->req.count].start,
                (unsigned char*) capture->frame.imageData);
        break;

    case PALETTE_SGBRG:
        sgbrg2rgb24(capture->form.fmt.pix.width, capture->form.fmt.pix.height,
                (unsigned char*) capture->buffers[(capture->bufferIndex + 1)
                        % capture->req.count].start,
                (unsigned char*) capture->frame.imageData);
        break;
    case PALETTE_RGB24:
        rgb24_to_rgb24(capture->form.fmt.pix.width,
                capture->form.fmt.pix.height,
                (unsigned char*) capture->buffers[(capture->bufferIndex + 1)
                        % capture->req.count].start,
                (unsigned char*) capture->frame.imageData);
        break;

    case PALETTE_MAX:
        break;
    }

    return (&capture->frame);
}

class CvCaptureCAM_V4L2_CPP: CvCapture
{
public:
    CvCaptureCAM_V4L2_CPP()
    {
        captureV4L = 0;
    }
    virtual ~CvCaptureCAM_V4L2_CPP()
    {
        close();
    }

    virtual bool open(int index);
    virtual void close();

    virtual double getProperty(int) const;
    virtual bool setProperty(int, double);
    virtual bool grabFrame();
    virtual IplImage* retrieveFrame(int);
protected:

    CvCaptureCAM_V4L* captureV4L;
};

bool CvCaptureCAM_V4L2_CPP::open(int index)
{
    close();
    captureV4L = v4l2_open(index);
    return captureV4L != 0;
}

void CvCaptureCAM_V4L2_CPP::close()
{
    if (captureV4L)
    {
        v4l2_close(captureV4L);
        cvFree(&captureV4L);
    }
}

bool CvCaptureCAM_V4L2_CPP::grabFrame()
{
    return captureV4L ? v4l2_grab_frame(captureV4L) == 0 : false;
}

IplImage* CvCaptureCAM_V4L2_CPP::retrieveFrame(int)
{
    return captureV4L ? v4l2_get_cv_frame(captureV4L, 0) : 0;
}

double CvCaptureCAM_V4L2_CPP::getProperty(int propId) const
{
    return captureV4L ? v4l2_get_cv_property(captureV4L, propId) : 0.0;
}

bool CvCaptureCAM_V4L2_CPP::setProperty(int propId, double value)
{
    return captureV4L ?
            v4l2_set_cv_property(captureV4L, propId, value) == 0 : false;
}

CvCapture* cvCreateCameraCapture_V4L2(int index)
{
    CvCaptureCAM_V4L2_CPP* capture = new CvCaptureCAM_V4L2_CPP;

    if (capture->open(index))
        return (CvCapture*) capture;

    delete capture;
    return 0;
}

#endif
