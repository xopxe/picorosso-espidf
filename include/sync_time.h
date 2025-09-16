#ifndef __SYNC_TIME_H
#define __SYNC_TIME_H

//    CTYPE(ros_TimeReference,
//        FIELD(ros_Header, header)
//        FIELD(ros_Time, time_ref)
//        FIELD(rstring, source)
//    )
//    CTYPE(ros_Header,
//        FIELD(ros_Time, stamp)
//        FIELD(rstring, frame_id)
//    )

#define MESSAGE_TIMEOUT_uS 100 * 1000 // time to wait for a response
#define MESSAGE_COUNT 20              // measures averaged during estimate_offset

enum SyncTimeStates
{
  SYNC_TIME_UNSYNCED,
  SYNC_TIME_SYNCING,
  SYNC_TIME_SYNCED
};

class SyncTime
{
public:
  SyncTime();
  static bool setup(const char *topic_request = "sync_time_request",
                    const char *topic_response = "sync_time_response",
                    const char *source = "sync_time");

  static bool synchronize_clock();

  static void trigger_sync();

  static SyncTimeStates sync_state;
};

#endif // __SYNC_TIME_H
