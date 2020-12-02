#include <iostream>
#include "SIL.h"
#include </usr/local/include/boost/shared_array.hpp>
#include </usr/local/include/boost/shared_ptr.hpp>
#include "SViewData.h"
#include "Metadata.h"

// Copyright 2015 Michael Aberanthy
// All rights reserved.

#pragma once
#define __STDC_CONSTANT_MACROS

extern "C" {
#include <libavutil/pixfmt.h>
    #include <libavformat/avformat.h>
    #include <libavcodec/avcodec.h>
    #include <libavutil/avutil.h>
    #include <libswscale/swscale.h>
};

class Stanag4609SIL : public SIL
{
public:
    virtual void Init(double& frameRate) ;
    virtual boost::shared_array<GLubyte> GetLatestVideo(int &width, int &height) ;
    virtual boost::shared_ptr<SViewData> GetLatestViewData()  ;
    virtual int SetVideoFileName(std::string fName);
    virtual double GetElapsedTime();
    virtual double GetDuration();

    Stanag4609SIL(void);
    ~Stanag4609SIL(void);
    int SetPlayTime(double playtime);

    Metadata _metadata;
    void SaveFrame(AVFrame *pFrame, int width, int height, int iFrame);
    int KeepLastRGB(AVFrame *p);
    int KeepLastKLV(AVPacket *p);
    int GetNextVideoFrame(); // get video frame nearest current playtime
    SwsContext		*_sws;
    AVFrame *_mostRecentFrameRGB;
    AVPacket *_mostRecentKLVPacket;
    int _readyToPlay;
    int _videoWidth;
    int _videoHeight;
    int _bitDepth;
    int _iFrameCount, _keyFrameCount, _FrameCount;
    double _startTime,_stopTime,_Duration,_frameRate,_startDTS;
    int _startFrameCount;
    int AtEnd;
    //int _readyToPlay;

    // opens a video to play
    int Init(char * fname);
    int HasKLV();
    int GetVideoFrame(); // get video frame nearest current playtime
    int GetMetadata(); // get metadata nearest current playtime
    int CloseOut();

    int Poll();
// STRICTLY INTERNAL STUFF
    std::string VideoFileName;
    AVCodecContext  *_pVideoCodecContext;
    AVCodec         *_pVideoCodec;
    AVFormatContext *_pFormatCtx;
    AVCodecContext  *_pAsyncKLVCodecContext, *_pKLVCodecContext;
    AVCodec         *_pvCodec, *_pKLVCodec;
    AVFrame         *_pFrame;
    AVRational vsTimebase,vsFramerate;
    AVStream *vstr;
    int _videoStreamIdx,_klvStreamIdx, _syncklvStreamIdx;
    enum AVPixelFormat _out_pix_fmt;
};
