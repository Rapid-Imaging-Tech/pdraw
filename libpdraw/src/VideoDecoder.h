// SourceInterfaceModule.h: interface for the SourceInterfaceModule class.
//
//////////////////////////////////////////////////////////////////////
//#define FUSION3DI_WIN32
//#if (!defined(CONFIG_3DI) && !defined(FUSION3DI_WIN32))
#if !defined(AFX_SOURCEINTERFACEMODULE_H__7459A6B9_F01F_42D6_8E93_7897FA895CED__INCLUDED_)
#define AFX_SOURCEINTERFACEMODULE_H__7459A6B9_F01F_42D6_8E93_7897FA895CED__INCLUDED_
#if !defined(CONFIG_3DI)
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include <vector>
//#include "Profiler.h"
#include "boost/thread.hpp"
#include <atlstr.h>
#include "SIL.h"
#include "ffmpeg\libavcodec\avcodec.h"
#include "ffmpeg\libavutil\samplefmt.h"
#include "ffmpeg_fas.h"
#include "video.h"
#include "DJIMetadata.h"
#define CHANNELS 26  /* was 11, 16 */
class VideoDecoder  : public SIL
{
public:
	virtual void Init(double& frameRate) ;
	virtual boost::shared_array<GLubyte> GetLatestVideo(int &width, int &height) ;
	virtual boost::shared_ptr<SViewData> GetLatestViewData()  ;
	//__declspec (dllexport)std::string SendSILCommand(std::vector<CString> sValueVector, 
	//std::vector<double> dValueVector, std::vector<int> iValueVector);
 private:
	 double _frameRate;
	 double _start;
	 DJIMetadata _djiMetadata;
	 //void SafireThread();
	// void StartSafireListenerThread();
	//  DataStatus dStat;
	// void SetupSafireListener();
	 //SafireListener _safireListener;
	 void ActiveImagerSelect(CString type);
	 int _baud;
	 int _bytesize ;
	 int _klvport;
	 std::string _comstring ;
	 double _stopbits ;
	 int _parity ;
	 std::string _klvip;
	 std::string _klvUID;
	 void GetComSettings();
	 void LogMessage(std::string message);
	 CString _originalConnectionString;
	 bool _firstInit;
	double _startFCLat;
	double _startFCLon;
	double _startFCEl;

	double _startSlantRange;
	double _startCamBrg ;
	double _startCamDepression ;
	double _startCamTwist ;
	void ReadFixPosFile();
	bool _startFixed;
	double _startLat;
	double _startLon;
	double _startAlt;
	double _startPitch;
	double _startHeading;
	double _startRoll;
	double _startVfov;
	bool _startMove;
        bool _doneGettingDictFile;
	double _avgFPS;
	int SetupKLVKeysToChannels();
	//using namespace Profiler;
	//Profiler::Profiler profiler;
	std::vector<WORD> fpsVector;
	double CalculateFPS();
	BOOL _verboseLogging;
	BOOL _this_time_valid;
	double _this_time;
	int activateLogFile();
	int SaveFileClose();
	bool _saveData;
	FILE* _logfile;
	std::vector<AVPacket*> _avpackets;
	double _frameSyncError ;

	int CalculateSkipFrames();

	fas_raw_image_type image[3];
	fas_raw_image_type *my_image_bufferp, *his_image_bufferp,
		*his_last_image_bufferp, *temp_image_bufferp;
	int _frameHorizontalSize;
	int _frameVerticalSize;
	bool _vfovPresent;
	void CheckForVFOV(double* valuesArray, double* currentValuesPtr, int index, int iret);
	int _vfovIndex;

	char archive_file[256];
	char playlistName[256];
	time_t utime;
	struct tm *utctime;

public:
#if defined(_WIN32)
	__declspec (dllexport)std::string SendSILCommand(std::vector<CString> sValueVector, 
		std::vector<double> dValueVector, std::vector<int> iValueVector);
#else
        std::string SendSILCommand(std::vector<CString> sValueVector, 
		std::vector<double> dValueVector, std::vector<int> iValueVector);
#endif
        
	char * GetStatus(double *p);
	char * Status(double *p);
	char * Send(double *p);
	void readerProc();
	//	CString Name;
	BOOL _busy;
	double Values[256];
	int _nChannels;
	double Realize(double time, double *values);
	char * GetChannelName(int n);
	VideoDecoder();
	virtual ~VideoDecoder();
	int Init(double *p);
	void StartListen();
	char * GetVehicleName(int n);
	void shut_down(void);
	void SetMissedValues(double *missedValues);

	// Live Stream Video and KLV Reading
	int _KLVPackets;
	int _KLVErrors;
	int _videoFrames;
	int _videoErrors;
	int _videoOverruns;
	double _latestValues[CHANNELS];
	AVPacket *_pPacket;  /* this is the most recently read KLV packet */
	video_frame _frames[3];
	video_frame *_my_frame;
	video_frame *_his_frame;
	video_frame *_his_last_frame;
	bool _stream_Initted;
	bool _KLV_Initted;
	bool _live_Stream;  /* only meaningful if Stream_Initted and KLV_Initted are true */
	bool _end_Reader_Proc;  /* if true tells reader_proc to end itself */
	bool _live_Stream_Hung;  /* if true means live stream is stalled */
	int _threads;  /* number of ffmpeg threads - default 2 */
//	thread synchronization
        boost::mutex _mutex_data;
	boost::mutex _mutex_video;
#if defined(_WIN32)
	unsigned long threadHandle;
	

							// allocated on it's stack in case it throws an exception
	HANDLE ghVideoEvent;	// wait object for readerProc to signal Send that video is available
#else
	pthread_t reader_thread;
//	pthread_t gui_thread;  // no longer needed
	pthread_t archive_thread;
	//pthread_mutex_t mutex_data;  // used for data critical section
	//pthread_mutex_t mutex_video;  // used for video critical section
	pthread_mutex_t mutex_video_signal;  // used to protect the video condition
	pthread_cond_t cond_video_signal;  // video signal condition
	pthread_mutex_t mutex_archive_signal;  // used to protect the video condition
	pthread_cond_t cond_archive_signal;  // video signal condition
#endif // defined(_WIN32)

//	archiving
	bool _end_Archive_Proc;  /* if true tells reader_proc to end itself */

#if defined(MPEGKLVSIL_ARCHIVE)
	void archiveProc(char *p);

#if defined(_WIN32)
	HANDLE archiveThreadHandle;
#endif // defined(_WIN32)
#endif // defined(MPEGKLVSIL_ARCHIVE)

// transport control gui
	int tc_handle_state(int got_frame);

#if defined(MPEGKLVSIL_GUI)
	static void guiProc(char* ptr);

#if defined(_WIN32)
	HANDLE guiThreadHandle;
#endif // defined(_WIN32)
#endif // defined(MPEGKLVSIL_GUI)

	BOOL last_time_valid;	// stuff for freshness
	double last_time;
	// Creates an archive file name from the current time.
	int MakeTimeName(void);
	// Returns true if it is time to segment
	int TimeToSegment(void);
	// enables archive segementation
	int _DoSegment;
	// start deleting with the oldest files if you are out of space and are segmenting
	int _RollOverArchive;
	// maximum amount of video to be stored in megabytes applied only to segmented
	float _MaxVideoSegmentStorage;
	// if archiving if hls, checks to make sure we are not over our alloted video storage space and if so deletes files until we are not
	int RollOverMgr(void);
	// Starts the playlist associated with an HLS collection
	int StartPlaylist(void);
	int ClosePlayList(void);
	// Adds a segment to the play list.
	int AddToPlayList(char * segmentName);
};
#else
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "sixdof.h"
class SourceInterfaceModule  
{
public:
#if defined(FUSION3DI_WIN32)

	void SetFramePtr(unsigned char* frame, int width, int height);
#endif
	char * GetStatus(double *p);
	char * status(double *p);
	char * Send(double *p);
	void readerProc(char *p);
//	CString Name;
	BOOL busy;
	double Values[256];
	int nChannels;
	double Realize(double time, double *values);
	char * GetChannelName(int n);
	SourceInterfaceModule();
	virtual ~SourceInterfaceModule();
	int Init(double *p);
	void StartListen();
	void SetValues(unsigned char *frame, int width, int height, SIXDOF *sixdofa, SIXDOF *sixdofb);
	char * GetVehicleName(int n);
	void shut_down(void);

#if defined(_WIN32)
	unsigned long threadHandle;
	CMutex mutex_data;		// used for data critical section
	CMutex mutex_video;		// used for video critical section
	CSingleLock *pLock_data;    // These are the locks which the app uses to access the mutexes.
	CSingleLock *pLock_video;	// Access to the mutexes by the thread should be via another lock
							// allocated on it's stack in case it throws an exception
	HANDLE ghVideoEvent;	// wait object for readerProc to signal Send that video is available
#else
	pthread_t reader_thread;
//	pthread_t gui_thread;  // no longer needed
	pthread_t archive_thread;
	pthread_mutex_t mutex_data;  // used for data critical section
	pthread_mutex_t mutex_video;  // used for video critical section
	pthread_mutex_t mutex_video_signal;  // used to protect the video condition
	pthread_cond_t cond_video_signal;  // video signal condition
	pthread_mutex_t mutex_archive_signal;  // used to protect the video condition
	pthread_cond_t cond_archive_signal;  // video signal condition
#endif // defined(_WIN32)

#if defined(MPEGKLVSIL_ARCHIVE)
//	void archiveProc(char *p);

#if defined(_WIN32)
	HANDLE archiveThreadHandle;
#endif // defined(_WIN32)
#endif // defined(MPEGKLVSIL_ARCHIVE)

#if defined(MPEGKLVSIL_GUI)
//	void guiProc(char *p);

#if defined(_WIN32)
	HANDLE guiThreadHandle;
#endif // defined(_WIN32)
#endif // defined(MPEGKLVSIL_GUI)

	BOOL last_time_valid;	// stuff for freshness
	double last_time;
	// diagnostime time
//	struct timeval l_time;  // last time
//	struct timeval c_time;  // current time
//	double d_time;  // delta time
};



#endif
#endif // !defined(AFX_SOURCEINTERFACEMODULE_H__7459A6B9_F01F_42D6_8E93_7897FA895CED__INCLUDED_)

