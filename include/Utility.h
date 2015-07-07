//Author: Xuepeng Fan, xuepeng_fan@163.com, 2015.07.07
#pragma once

#include <windows.h>
#include <vector>
#include <string>

typedef long long int milliseconds_t;
typedef unsigned short uint16_t;

namespace ff{
class HighResolutionTimer{
  public:
    static milliseconds_t current_time();
  protected:
    static LARGE_INTEGER s_frequency;
    static bool s_initialized;
};


class RTProfiler{
  public:
    typedef uint16_t event_id_t;
    RTProfiler(const char * fp);

    virtual ~RTProfiler();

    void record(event_id_t event_id, int event_value);

  protected:

    void dump_to_file();
    template<class T>
      struct RecordItem{
        milliseconds_t time_stamp;
        event_id_t event_id;
        T event_value;
      };

    typedef RecordItem<int> int_item_t;

    std::vector<int_item_t> m_oIntItems;
    std::string mstr_fp;
};

}//end namespace ff
